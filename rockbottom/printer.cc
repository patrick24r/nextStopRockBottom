#include "printer.h"

#include <fstream>
#include <iostream>
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
  int rowsToPrint = kHeightPixels / kFontHeightPixels;

  if (IsRaspberryPi()) {
    std::cout << "Raspberry Pi detected!" << std::endl;

    RGBMatrix::Options matrix_options;
    matrix_options.hardware_mapping = kHardwareMapping;
    matrix_options.rows = kPanelHeightPixels;
    matrix_options.cols = kPanelWidthPixels;
    matrix_options.chain_length = kPanelCount;
    matrix_options.parallel = 1;

    RuntimeOptions runtime_options;
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

    Font font;
    if (!font.LoadFont("../fonts/9x15.bdf")) {
      std::cerr << "Failed to load font" << std::endl;
      return;
    }

    rgb_matrix::DrawText(rgb_matrix, font, 2, 15, Color(255, 0, 0),
                         "Testing text!");

  } else {
    // Just print to console
    for (auto& time : bus_times) {
      std::cout << time.route_id << " " << time.destination << " "
                << time.minutes_to_arrival << " min" << std::endl;
    }
  }
}

static void Clear() {
  if (rgb_matrix) {
    delete rgb_matrix;
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