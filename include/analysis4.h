#pragma once

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

bool IsLeapYear(int year);

int DayOfYear(int month, int day);

void Analysis4();
