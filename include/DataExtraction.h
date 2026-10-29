#pragma once

// Needed to parse .csv files
#include <TFile.h>
#include <TTree.h>
#include <csv/csv.h>

#include <string>
#include <vector>

#include "Observation.h"

// Function that takes .csv data file as input and creates a vector of
// observations
std::vector<Observation> parse_observation(const std::string& csv_file);

// Function that takes a vector of observations and writes a .root file.
// This will create a TTree object with a single branch called 'Measurements'
// which have five leafs: year, month, day, hour, temperature
void write_observation(const std::vector<Observation>& observations,
                       const std::string& tree_file);
