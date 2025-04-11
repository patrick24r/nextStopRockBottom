import argparse
from datetime import datetime, timezone
import json
import os.path
import urllib.request
from typing import List

MBTA_URL_ = "https://api-v3.mbta.com"
ROUTES_URL_ = MBTA_URL_ + "/routes?"
PREDICTIONS_URL_ = MBTA_URL_ + "/predictions?"
SCHEDULE_URL = MBTA_URL_ + "/schedules?"
FILTER_BASE_ = "&filter[{name}]={value}"


def main():
    parser = argparse.ArgumentParser("rockBottom")
    parser.add_argument("stop_id", help="MBTA stop ID")
    args = parser.parse_args()
    stop_id = args.stop_id
    # Find the routes connected to that stop and each direction identifier
    routes = get_routes_for_stop(stop_id)

    # Find the next times for that stop on those routes
    next_stop_times = []
    for route in routes:
        next_stop_times = next_stop_times + get_next_stop_times(
            stop_id, route["id"], route["direction_names"])

    # Sort by minutes until arrival
    next_stop_times = sorted(next_stop_times, key=lambda x: x["minutes"])
    for next in next_stop_times:
        print(str(next["route"]) + " " +
              next["direction"] + ", " + str(next["minutes"]) + " min")


def get_routes_for_stop(stop_id: int) -> List[dict]:
    # For the given stop ID, find the routes on that stop.
    routes_for_stops_file = "stop_" + str(stop_id) + ".json"
    routes_for_stops_json = []

    # If a file exists with that stop info, try to load from it
    if os.path.exists(routes_for_stops_file):
        with open(routes_for_stops_file) as json_data:
            routes_for_stops_json = json.load(json_data)

    # If the load failed or was empty, poll the MBTA API for info
    if not routes_for_stops_json:
        route_request_url = ROUTES_URL_ + \
            create_filter_string("stop", str(stop_id))
        with urllib.request.urlopen(route_request_url) as response:
            routes_for_stops_json = json.loads(response.read())["data"]
            # Save the data to a local file for next time
            with open(routes_for_stops_file, 'w', encoding='utf-8') as f:
                json.dump(routes_for_stops_json, f,
                          ensure_ascii=False, indent=4)

    return [{"id": int(route["id"]), "direction_names": route["attributes"]["direction_names"]} for route in routes_for_stops_json]


def get_next_stop_times(stop_id: int, route_id: int, directions: List[str]) -> List[dict]:
    request_url = (PREDICTIONS_URL_ + "page[limit]=3"
                   + create_filter_string("route", str(route_id))
                   + create_filter_string("stop", str(stop_id))
                   )

    return_info = []

    with urllib.request.urlopen(request_url) as response:
        data = json.loads(response.read())["data"]
        if (len(data) == 0):
            print("No predictions found for route: " +
                  str(route_id) + ", stop_id: " + str(stop_id))
            return

        for arrival in data:
            arrival_time = datetime.fromisoformat(
                arrival["attributes"]["arrival_time"]).replace(tzinfo=None)
            minutes_until_arrival = round((
                arrival_time - datetime.now()).total_seconds() / 60)
            direction = directions[int(arrival["attributes"]["direction_id"])]
            return_info.append(
                {"route": route_id, "minutes": minutes_until_arrival, "direction": direction})
    return return_info


def create_filter_string(filter_name: str, filter_value: str) -> str:
    return FILTER_BASE_.format(name=filter_name, value=filter_value)


if __name__ == '__main__':
    main()
