#include "printer.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "graphics.h"
#include "led-matrix.h"

namespace {

using rgb_matrix::RGBMatrix;

}  // namespace

namespace rockbottom {

void Printer::PrintTimes(std::vector<BusTime> bus_times) {
  int rowsToPrint = kHeightPixels / kFontHeightPixels;

  RGBMatrix::Options matrix_options;

  std::string error;
  if (matrix_options.Validate(&error)) {
    std::cout << "Valid RGB Options" << std::endl;
  } else {
    std::cout << "Invalid RGB Options" << std::endl;
  }

  if (IsRaspberryPi()) {
    std::cout << "Raspberry Pi detected!" << std::endl;
  } else {
  }
}

bool Printer::IsRaspberryPi() {
  std::ifstream file("/etc/os-release");
  if (!file.is_open()) {
    return false;
  }

  std::string line;
  while (std::getline(file, line)) {
    // Look for the ID key specifically
    if (line.find("ID=") == 0 || line.find("ID_LIKE=") == 0) {
      if (line.find("raspbian") != std::string::npos ||
          line.find("raspberrypi") != std::string::npos) {
        return true;
      }
    }
  }
  return false;
}

}  // namespace rockbottom