#pragma once

#include "Observation.h"
#include <string>
#include <vector>

// Count days above a temperature threshold during the summer period
// and save the result to a ROOT file
void analysis1(const std::vector<Observation> &observations,
               Double_t threshold,
               const std::string &root_file); 


