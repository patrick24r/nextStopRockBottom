#include "fetcher.h"

#include <curl/curl.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

namespace {

using ::nlohmann::json;

}  // namespace

namespace rockbottom {

std::vector<Fetcher::BusTime> Fetcher::FetchTimes(
    std::vector<std::string>& stop_ids) {
  std::vector<BusTime> return_times;

  // For each stop, find all routes that go through it
  for (const auto& stop : stop_ids) {
    // For each route through the stop, find the next predicted stop times
    for (auto& route : GetRoutesForStopId(stop)) {
      auto new_predictions = GetPredictedTimes(stop, route);

      // Append the new_predictions to the total return_times
      return_times.insert(return_times.end(), new_predictions.begin(),
                          new_predictions.end());
    }
  }

  // Sort return_times by minutes until departure
  std::sort(return_times.begin(), return_times.end(),
            [](const BusTime& a, const BusTime& b) {
              return a.minutes_to_arrival < b.minutes_to_arrival;
            });
  return return_times;
}

std::vector<Fetcher::BusRoute> Fetcher::GetRoutesForStopId(
    std::string stop_id) {
  std::vector<BusRoute> return_routes;
  std::string request_url =
      GetRequestUrl("routes") + CreateFilterString("stop", stop_id);
  std::string readBuffer;
  readBuffer.reserve(kUrlBufferSize);

  if (CURLcode res = GetCurlResponse(request_url, &readBuffer);
      res != CURLE_OK) {
    std::cerr << "GetCurlResponse() failed: " << curl_easy_strerror(res)
              << std::endl;
    return return_routes;
  }

  json routes_json = json::parse(readBuffer)["data"];

  for (auto& route : routes_json) {
    return_routes.push_back({
        .id = route["id"],
        .destinations = std::vector<std::string>(
            route["attributes"]["direction_destinations"]),
    });
  }

  return return_routes;
}

std::vector<Fetcher::BusTime> Fetcher::GetPredictedTimes(std::string stop_id,
                                                         BusRoute route) {
  std::vector<BusTime> return_times;
  std::string readBuffer;
  readBuffer.reserve(kUrlBufferSize);

  std::string request_url = GetRequestUrl("predictions") +
                            CreateFilterString("stop", stop_id) +
                            CreateFilterString("route", route.id);

  if (CURLcode res = GetCurlResponse(request_url, &readBuffer);
      res != CURLE_OK) {
    std::cerr << "GetCurlResponse() failed: " << curl_easy_strerror(res)
              << std::endl;
    return return_times;
  }

  json predictions_json = json::parse(readBuffer)["data"];
  auto timestamp_now = std::chrono::system_clock::now();
  for (auto& prediction : predictions_json) {
    // Find the minutes until departure
    std::string departure_time = prediction["attributes"]["departure_time"];
    std::istringstream is{departure_time};
    is.imbue(std::locale("en_US.UTF-8"));
    std::chrono::sys_seconds departure_timestamp;
    is >> std::chrono::parse(kDateFormat, departure_timestamp);
    if (!is.fail()) {
      auto minutes_to_departure =
          std::chrono::duration_cast<std::chrono::minutes>(
              departure_timestamp -
              std::chrono::floor<std::chrono::seconds>(timestamp_now));
      if (minutes_to_departure.count() < 0) {
        // Skip any predictions that are in the past, they are irrelevant now
        continue;
      }

      int direction_index = prediction["attributes"]["direction_id"];
      std::string destination = route.destinations[direction_index];

      return_times.push_back(
          {.route_id = route.id,
           .destination = destination,
           .minutes_to_arrival = minutes_to_departure.count()});

    } else {
      std::cerr << "Failed to parse '" << departure_time << "'" << std::endl;
      continue;
    }
  }

  return return_times;
}

CURLcode Fetcher::GetCurlResponse(std::string url, std::string* response) {
  CURL* curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
  CURLcode status = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  return status;
}

size_t Fetcher::CurlWriteCallback(void* contents, size_t size, size_t nmemb,
                                  std::string* userp) {
  userp->append((char*)contents, size * nmemb);
  return size * nmemb;
}

}  // namespace rockbottom