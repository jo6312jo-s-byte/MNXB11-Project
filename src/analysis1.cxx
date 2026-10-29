#include "analysis1.h"

#include <TFile.h>
#include <TTree.h>

void analysis1(const std::vector<Observation> &observations, Double_t threshold,
               const std::string &root_file) {
  // Create output ROOT file
  TFile f{root_file.c_str(), "RECREATE"};

  Int_t year{};
  Int_t warm_days{};
  Int_t summer_days{};

  // Create tree for the analysis result
  TTree *tree = new TTree("analysis1", "Number of warm summer days per year");

  tree->Branch("year", &year);
  tree->Branch("warm_days", &warm_days);

  // Stop if there are no observations
  if (observations.empty()) {
    return;
  }

  // Start with the first observation
  Int_t current_year{observations[0].Year()};
  Int_t current_month{observations[0].Month()};
  Int_t current_day{observations[0].Day()};
  Double_t daily_max{observations[0].Temperature()};

  // Loop over the remaining observations
  for (std::size_t i{1}; i < observations.size(); i++) {
    const Observation &observation{observations[i]};

    // Check if this observation belongs to the same day
    if (observation.Year() == current_year &&
        observation.Month() == current_month &&
        observation.Day() == current_day) {
      // Keep the highest temperature measured during the day
      if (observation.Temperature() > daily_max) {
        daily_max = observation.Temperature();
      }

    } else {
      // Count the previous day if it was during summer
      if (current_month >= 6 && current_month <= 8) {
        summer_days++;

        // Count it as a warm day if it reached the threshold
        if (daily_max >= threshold) {
          warm_days++;
        }
      }

      // If a new year starts, save the result for the previous year
      if (observation.Year() != current_year) {
        // Only save years with complete summer data
        if (summer_days == 92) {
          year = current_year;
          tree->Fill();
        }

        warm_days = 0;
        summer_days = 0;
      }

      // Start processing the new day
      current_year = observation.Year();
      current_month = observation.Month();
      current_day = observation.Day();
      daily_max = observation.Temperature();
    }
  }

  tree->AutoSave();
}
