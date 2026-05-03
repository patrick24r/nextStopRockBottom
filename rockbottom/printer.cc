#include "printer.h"

#include "graphics.h"
#include "led-matrix.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

using rgb_matrix::RuntimeOptions;
using rgb_matrix::RGBMatrix::Options;

} // namespace

namespace rockbottom {

void Printer::PrintTimes(std::vector<BusTime> bus_times) {
  int rowsToPrint = kHeightPixels / kFontHeightPixels;
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

} // namespace rockbottom