#pragma once

#include <iostream>
#include <map>
#include <utility>

#include <TFile.h>
#include <TTree.h>
#include <TH1I.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TSystem.h>

#include "Observation.h"

bool IsLeapYear(int year);

int DayOfYear(int month, int day);

void Analysis4();
