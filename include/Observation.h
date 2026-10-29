#pragma once

// Needed to make a derived class
#include <TObject.h>

class Observation : public TObject {
 public:
  // Default constructor with zero parameters
  // This is required to use I/O objects
  Observation();

  // Constructor with parameters
  Observation(Int_t year, Int_t month, Int_t day, Int_t hour,
              Double_t temperature);

  // Override destructor from base class
  ~Observation() override;

  // Getters
  Int_t Year() const;
  Int_t Month() const;
  Int_t Day() const;
  Int_t Hour() const;
  Double_t Temperature() const;

  // Set function
  void SetObservation(Int_t year, Int_t month, Int_t day, Int_t hour,
                      Double_t temperature);

  // Member variables
 private:
  Int_t _year;
  Int_t _month;
  Int_t _day;
  Int_t _hour;
  Double_t _temperature;

  // Observation class
  ClassDef(Observation, 1);
};