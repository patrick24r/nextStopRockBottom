#include "printer.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "graphics.h"
#include "led-matrix.h"
namespace {

using rgb_matrix::Color;
using rgb_matrix::Font;
using rgb_matrix::RGBMatrix;
using rgb_matrix::RuntimeOptions;

}  // namespace

namespace rockbottom {

static RGBMatrix* rgb_matrix = nullptr;

void Printer::PrintTimes(std::vector<BusTime> bus_times) {
  bool isRaspberryPi = IsRaspberryPi();
  int rowsToPrint = kHeightPixels / kFontHeightPixels;

  // Each row is structured '[Route ID] [Destination Name] [Minutes] m'
  // Some route IDs are longer than others, so dynamically detect the
  // length of what we're printing and account for it
  int columnsToPrint = kWidthPixels / kFontWidthPixels;
  size_t routeIdColumns = 0;
  for (auto& time : bus_times) {
    routeIdColumns = std::max(routeIdColumns, time.route_id.length());
  }
  size_t destinationColumns = columnsToPrint - (routeIdColumns + 1) - 5;
  std::string formatString = "%-" + std::to_string(routeIdColumns) + "s %-" +
                             std::to_string(destinationColumns) + "s %2d m";

  Font font;
  if (IsRaspberryPi()) {
    RGBMatrix::Options matrix_options;
    matrix_options.hardware_mapping = kHardwareMapping;
    matrix_options.rows = kPanelHeightPixels;
    matrix_options.cols = kPanelWidthPixels;
    matrix_options.chain_length = kPanelCount;
    matrix_options.parallel = 1;
    matrix_options.pwm_bits = 8;

    RuntimeOptions runtime_options;
    runtime_options.gpio_slowdown = 2;
    runtime_options.drop_privileges = 1;

    std::string error;
    if (!matrix_options.Validate(&error)) {
      std::cerr << "Invalid RGB Options" << std::endl;
      return;
    }

    // If RGB matrix is not initialized, try to initialize it
    if (!rgb_matrix) {
      rgb_matrix =
          RGBMatrix::CreateFromOptions(matrix_options, runtime_options);
    }

    // If still not initialized, return failure
    if (!rgb_matrix) {
      std::cerr << "Failed to create RGB Matrix" << std::endl;
      return;
    }

    rgb_matrix->Clear();
    if (!font.LoadFont("./rockbottom/rpi-rgb/fonts/5x8.bdf")) {
      std::cerr << "Failed to load font" << std::endl;
      return;
    }
  }

  for (int row = 0; row < rowsToPrint && row < bus_times.size(); row++) {
    char lineBuffer[100];
    snprintf(lineBuffer, sizeof(lineBuffer), formatString.c_str(),
             bus_times[row].route_id.c_str(),
             bus_times[row].destination.c_str(),
             bus_times[row].minutes_to_arrival);
    if (isRaspberryPi) {
      rgb_matrix::DrawText(rgb_matrix, font, 0,
                           row * kFontHeightPixels + kFontHeightPixels,
                           Color(255, 0, 0), lineBuffer);
    } else {
      std::cout << lineBuffer << std::endl;
    }
  }
}

// std::string Printer::GenerateLine(BusTime& bus_time) {}

void Printer::Clear() {
  if (rgb_matrix) {
    rgb_matrix->Clear();
  }
}

bool Printer::IsRaspberryPi() {
  std::ifstream file("/proc/device-tree/model");
  if (!file.is_open()) {
    return false;
  }

  std::string line;
  while (std::getline(file, line)) {
    // Look for the ID key specifically
    if (line.find("Raspberry Pi") == 0) {
      return true;
    }
  }
  return false;
}

}  // namespace rockbottom