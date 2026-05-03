#pragma once

#include <curl/curl.h>

#include <cstdint>
#include <ctime>
#include <sstream>
#include <string>
#include <vector>

namespace rockbottom {

class Fetcher {
 public:
  struct BusRoute {
    std::string id;
    std::vector<std::string> destinations;
  };

  struct BusTime {
    std::string route_id;
    std::string destination;
    int minutes_to_arrival;
  };

  static std::vector<BusTime> FetchTimes(std::vector<std::string>& stop_ids);

 private:
  static constexpr int kUrlBufferSize = 2048;
  static constexpr const char* kDateFormat = "%FT%T%z";
  static const std::string GetRequestUrl(const std::string request_type) {
    return "https://api-v3.mbta.com/" + request_type + "?";
  }

  static const std::string CreateFilterString(std::string filter_name,
                                              std::string filter_value) {
    return "&filter[" + filter_name + "]=" + filter_value;
  }

  static std::vector<BusRoute> GetRoutesForStopId(std::string stop_id);
  static std::vector<BusTime> GetPredictedTimes(std::string stop_id,
                                                BusRoute route);
  static CURLcode GetCurlResponse(std::string url, std::string* response);
  static size_t CurlWriteCallback(void* contents, size_t size, size_t nmemb,
                                  std::string* userp);

  Fetcher();
};

}  // namespace rockbottom