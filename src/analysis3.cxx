#include <TAxis.h>
#include <TBox.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TLine.h>
#include <TParameter.h>
#include <TStyle.h>
#include <TTree.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Observation.h"

double tempPerYear(int predictionYear);

double tempPerYear(int predictionYear) {
  // style for TGraphs
  gStyle->SetTitleFont(42, "T");
  gStyle->SetTitleSize(0.05, "T");

  // This whole thing is basically homework 6 and lecture/tutorial 7
  // open root file
  TFile *file = TFile::Open("temperature_tree.root", "READ");

  // did the file open sucessfully?
  if (file == nullptr || file->IsZombie()) {
    std::cout << "Error opening file" << std::endl;
    return 0.0;
  }

  TTree *tree = file->Get<TTree>("measurements");

  Observation *measurement = nullptr;

  tree->SetBranchAddress("measurements", &measurement);

  std::map<int, double> tempSum;
  std::map<int, int> measurementCount;
  std::map<int, std::map<int, int>> daily;

  // number of entries in tree
  Long64_t nEntries = tree->GetEntries();

  // loop over all measurements
  for (Long64_t i = 0; i < nEntries; i++) {
    tree->GetEntry(i);

    int day = measurement->Day();
    int month = measurement->Month();
    int year = measurement->Year();
    double temperature = measurement->Temperature();

    // gives each date a unique number (eq feb 23 is 223)
    int date = month * 100 + day;
    // uses the nested map to see if there are any measurements that day
    daily[year][date] = 1;

    // adds temp to the sum of that year
    tempSum[year] += temperature;
    // increases the number of measurements for that year
    measurementCount[year]++;
  }

  // store year and corresponding temperature
  std::vector<int> years;
  std::vector<double> meanTemperatures;

  //===ANALYSIS===

  // calculate mean temp
  for (const auto &entry : tempSum) {
    int year = entry.first;

    int daysCount = static_cast<int>(daily[year].size());

    // skips years without 360 days of data
    if (daysCount < 360) {
      continue;
    }

    double meanTemp = tempSum[year] / measurementCount[year];

    // two new vectors containing year and corresponding mean temperature
    years.push_back(year);
    meanTemperatures.push_back(meanTemp);
  }

  //===GRAPHS===

  // annual mean temperature
  TGraph meanTempGraph;

  for (int i = 0; i < static_cast<int>(years.size());
       i++)  // for i=0 (first year of data) to the last year recorded
  {
    meanTempGraph.SetPoint(i, years[i], meanTemperatures[i]);  // set points
  }

  // a straight fitted line to the datapoints
  TF1 fit("linearFit", "[0] + [1]*(x-2000)", years.front(), years.back());

  meanTempGraph.Fit(&fit, "Q");

  // create graph
  TCanvas meanTempCanvas("meanTempCanvas", "Annual Mean Temperature", 800, 600);

  // title
  meanTempGraph.SetTitle(
      "Annual Mean Temperature;Year;Mean Temperature [Celsius]");

  // font things
  meanTempGraph.GetXaxis()->SetTitleFont(42);
  meanTempGraph.GetYaxis()->SetTitleFont(42);
  meanTempGraph.GetXaxis()->SetTitleSize(0.045);
  meanTempGraph.GetYaxis()->SetTitleSize(0.045);
  meanTempGraph.GetXaxis()->SetLabelFont(42);
  meanTempGraph.GetYaxis()->SetLabelFont(42);
  meanTempGraph.GetXaxis()->SetLabelSize(0.035);
  meanTempGraph.GetYaxis()->SetLabelSize(0.035);

  // other style things
  meanTempGraph.SetMarkerStyle(8);
  meanTempGraph.SetMarkerColor(41);
  meanTempGraph.SetLineColor(kBlue);

  meanTempGraph.Draw("AP");

  meanTempCanvas.SaveAs("results/annual_mean.png");

  // mean temperature divergence from average
  double sumMean = 0.0;

  for (int i = 0; i < static_cast<int>(meanTemperatures.size()); i++) {
    sumMean += meanTemperatures[i];
  }

  double totMean = sumMean / meanTemperatures.size();

  // deviation from mean (bar charts)
  TH1D devHist("devHist", "Deviation from average", years.size(), 0,
               years.size());

  // title
  devHist.SetTitle(
      "Annual Mean Temperature Deviation From Average;Year;Mean Temperature "
      "[Celsius]");
  devHist.SetTitleFont(42, "");
  devHist.SetTitleSize(0.05, "");

  // font things (titles)
  devHist.GetXaxis()->SetTitleFont(42);
  devHist.GetYaxis()->SetTitleFont(42);
  devHist.GetXaxis()->SetTitleSize(0.045);
  devHist.GetYaxis()->SetTitleSize(0.045);

  // font things (labels)
  devHist.GetXaxis()->SetLabelFont(42);
  devHist.GetYaxis()->SetLabelFont(42);
  devHist.GetXaxis()->SetLabelSize(0.035);
  devHist.GetYaxis()->SetLabelSize(0.035);

  // y-axis mina and max
  devHist.SetMinimum(totMean - 2.5);
  devHist.SetMaximum(totMean + 2.5);
  devHist.SetStats(false);

  TCanvas devCanvas("devCanvas", "Temperature", 1000, 600);

  devHist.Draw("AXIS");

  for (int i = 0; i < static_cast<int>(years.size()); i++) {
    if (years[i] % 20 == 0) {
      devHist.GetXaxis()->SetBinLabel(i + 1, std::to_string(years[i]).c_str());
    }

    TBox *bar = new TBox(i, std::min(totMean, meanTemperatures[i]), i + 1,
                         std::max(totMean, meanTemperatures[i]));

    if (meanTemperatures[i] < totMean) {
      bar->SetFillColor(kBlue);
    } else {
      bar->SetFillColor(kRed);
    }

    bar->SetLineColor(kBlack);
    bar->SetLineWidth(2);

    bar->Draw("same");
  }

  // avg. temp. line
  TLine averageLine(0, totMean, years.size(), totMean);
  averageLine.SetLineColor(kBlack);
  averageLine.SetLineStyle(1);
  averageLine.Draw("same");

  devCanvas.SaveAs("results/deviation_from_average.png");

  // predict temperature for the requested year
  double prediction = fit.Eval(predictionYear);

  TFile outputFile("results/analysis3.root", "RECREATE");
  TParameter<double> result("predictedTemperature", prediction);
  result.Write();
  outputFile.Close();

  file->Close();
  delete file;

  std::cout << "Predicted annual mean temperature in " << predictionYear << ": "
            << prediction << " C" << std::endl;

  return prediction;
}