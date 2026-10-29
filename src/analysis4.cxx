#include <TCanvas.h>
#include <TFile.h>
#include <TH1I.h>
#include <TLegend.h>
#include <TSystem.h>
#include <TTree.h>

#include <iostream>
#include <map>
#include <utility>

#include "Observation.h"

using namespace std;

//===DATE FIXER FUNCTIONS===
// checks if year is leap year
// leap years are dividible by 4 & 400
// but not with 100
bool IsLeapYear(int year) {
  return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

int DayOfYear(int month, int day) {
  // array stores number of days in each month
  int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // doy:day of year
  int doy = day;

  // adds nr of days in perceeding months
  for (int m = 1; m < month; m++) {
    doy += days[m - 1];
  }
  return doy;
}
void Analysis4() {
  //===LOAD FILES AND MEASUREMENTS===
  if (gSystem->Load("./libObservation.so") < 0) {
    cout << "Could not load libObservation.so" << endl;
    return;
  }
  // opens root file and finds tree
  TFile file("temperature_tree.root", "READ");

  if (file.IsZombie()) {
    cout << "Could not open temperature_tree.root" << endl;
    return;
  }
  // TTree stores large datasets
  TTree* tree = nullptr;
  file.GetObject("measurements", tree);

  if (!tree) {
    cout << "Could not fins measurements tree" << endl;
    return;
  }
  // connect to observation branch
  Observation* measurement = nullptr;
  tree->SetBranchAddress("measurements",
                         &measurement);  // connects trees measurements branch
                                         // to measurement pointer

  //===COLLECT TEMPERATURES FOR EACH DAY===
  // store temp sum and nr of measurments per day
  // create nested map:
  map<int, map<int, pair<double, int>>> daily;

  // Read all entries and calc daily counts
  for (Long64_t i = 0; i < tree->GetEntries();
       i++) {  // returns number of entries in tree

    tree->GetEntry(i);  // load measurement nr i

    if (!measurement) {
      continue;
    }
    // Retrieve year, month day and temp from observations
    // from loop above. -> operator accessed functions through a pointer
    int year = measurement->Year();
    int month = measurement->Month();
    int day = measurement->Day();
    double temp = measurement->Temperature();

    if (IsLeapYear(year) && month == 2 && day == 29) {
      continue;
    }

    int doy = DayOfYear(month, day);

    // add current temp to dauly sum
    // then increase nr of measurement by one
    daily[year][doy].first += temp;
    daily[year][doy].second++;
  }
  //===CREATE THE HISTOGRAMS===
  // Create histograms with 365 bins
  TH1I* warmest = new TH1I(
      "Warmest", "Warmest and coldest days;Day of Year;Number of Years", 365,
      0.5, 365.5  // nr of bins and axis boundaries
  );
  TH1I* coldest = new TH1I(
      "Coldest", "Warmest and Coldest Days; Day of Year; Number of Years", 365,
      0.5, 365.5);

  // prevent root from associating histograms with input file
  // file closes at end of analysis
  warmest->SetDirectory(nullptr);
  coldest->SetDirectory(nullptr);

  //===FIND WARMEST AND COLDEST DAYS FOR EACH YEAR===
  for (auto& [year, days] : daily) {
    // skips years without enough data
    if (days.size() < 360) {
      continue;
    }

    double highest = -1e9;
    double lowest = 1e9;
    int warmestDay = 0;
    int coldestDay = 0;

    for (auto& entry : days) {
      int doy = entry.first;
      auto& values = entry.second;

      // calculate daily mean
      double average = values.first / values.second;

      // find warmest day:
      // if current days temp is higher than previous highest
      // temp it replaces it
      if (average > highest) {
        highest = average;
        warmestDay = doy;
      }

      if (average < lowest) {
        lowest = average;
        coldestDay = doy;
      }
    }

    // count how frequently days occur as warmest and coldest
    warmest->Fill(warmestDay);
    coldest->Fill(coldestDay);

    cout << year << ": Warmest day = " << warmestDay
         << ", Coldest day = " << coldestDay << endl;
  }

  //===DRAW THE HISTOGRAMS===
  warmest->SetLineColor(kRed);
  coldest->SetLineColor(kBlue);

  warmest->SetLineWidth(2);
  coldest->SetLineWidth(2);

  TCanvas* canvas =
      new TCanvas("canvas5", "Warmest and Coldest days", 1000, 600  // pixels
      );

  warmest->Draw("HIST L");
  coldest->Draw("HIST L SAME");

  TLegend* legend = new TLegend(0.15, 0.75, 0.35, 0.88);

  legend->AddEntry(warmest, "Warmest day", "l");
  legend->AddEntry(coldest, "Coldest day", "l");

  legend->Draw();

  //===SAVE THE PLOT===
  canvas->SaveAs("results/analysis4.pdf");

  //===SAVE RESULTS TO ROOT FILE===
  TFile outputFile("results/analysis4.root", "RECREATE");

  warmest->Write();
  coldest->Write();
  outputFile.Close();
  file.Close();
}
