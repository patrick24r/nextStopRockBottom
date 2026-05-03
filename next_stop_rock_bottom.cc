
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "rockbottom/fetcher.h"
#include "rockbottom/printer.h"

using ::nlohmann::json;
using ::rockbottom::Fetcher;
using ::rockbottom::Printer;

int main() {
  // Get the Stop IDs that the config file requests
  std::ifstream file("config.json");

  if (!file.is_open()) {
    std::cerr << "Could not open config.json" << std::endl;
    return 1;
  }

  std::vector<std::string> stop_ids;
  try {
    json data = json::parse(file);
    stop_ids = data["stops"].get<std::vector<std::string>>();

  } catch (json::parse_error &e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
    return 1;
  }

  // Get the bus times from the MBTA website
  auto bus_times = Fetcher::FetchTimes(stop_ids);

  // Get times from the MBTA website
  Printer::PrintTimes(bus_times);
  return 0;
}
