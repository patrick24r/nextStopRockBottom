#pragma once

#include "fetcher.h"

#include <vector>

namespace rockbottom {

class Printer {
public:
  using BusTime = Fetcher::BusTime;
  static void PrintTimes(std::vector<BusTime> bus_times);

private:
  static constexpr int kPanels = 1;
  static constexpr int kPanelWidthPixels = 64;
  static constexpr int kPanelHeightPixels = 32;
  static constexpr int kFontHeightPixels = 8;
  // Panels are organized in a single, wide chain
  static constexpr int kWidthPixels = kPanels * kPanelWidthPixels;
  static constexpr int kHeightPixels = kPanelHeightPixels;
  Printer();

  static bool IsRaspberryPi();
};

} // namespace rockbottom