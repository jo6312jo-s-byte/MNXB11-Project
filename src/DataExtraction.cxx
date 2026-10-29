#include "DataExtraction.h"

// Definition of function that takes .csv data file as input and creates a
// vector of observations
std::vector<Observation> parse_observation(const std::string &csv_file) {
  // We use the external csv parser library
  // We have five columns in the parsed .csv data file
  io::CSVReader<5> input{csv_file};
  input.read_header(io::ignore_extra_column, "year", "month", "day", "hour",
                    "temperature");

  // Initialize variables to add contents of .csv file to
  Int_t year{};
  Int_t month{};
  Int_t day{};
  Int_t hour{};
  Double_t temperature{};

  // Initialize vector of Observation objects
  std::vector<Observation> rows{};

  while (input.read_row(year, month, day, hour, temperature)) {
    // Add each row at the end of rows
    rows.push_back(Observation{year, month, day, hour, temperature});
  }

  // Return vector filled with Observation objects
  return rows;
}

// Definition of function that writes a .root file from a vector of Observation
// objects
void write_observation(const std::vector<Observation> &observations,
                       const std::string &tree_file) {
  // .c_str() needed to convert from std::string to C-string
  // The RECREATE parameter makes it so that any file with name tree_file
  // gets overwritten if it already exists
  TFile f{tree_file.c_str(), "RECREATE"};

  // Initialize TTree object
  TTree *tree = new TTree("measurements", "A TTree with temperature data");

  // Initialize Observation pointer
  Observation *measurement{nullptr};

  // Make a branch
  tree->Branch("measurements", &measurement);

  Int_t year, month, day, hour;
  Double_t temperature;

  // Loop over the observations
  // Use reference type to prevent copying
  for (const auto &observation : observations) {
    year = observation.Year();
    month = observation.Month();
    day = observation.Day();
    hour = observation.Hour();
    temperature = observation.Temperature();

    measurement = new Observation(year, month, day, hour, temperature);
    tree->Fill();
    delete measurement;
  }

  // Save tree
  tree->AutoSave();
}
