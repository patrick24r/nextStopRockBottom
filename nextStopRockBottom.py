import argparse
import busStopPrinter
from datetime import datetime, timezone
import json
import os.path
import urllib.request
from typing import Any
from typing import List


MBTA_URL_ = "https://api-v3.mbta.com"
MBTA_REQUEST_URL_ = MBTA_URL_ + "/{request_type}?"
ROUTES_URL_ = MBTA_URL_ + "/routes?"
FILTER_BASE_ = "&filter[{name}]={value}"


def main():
    parser = argparse.ArgumentParser("rockBottom")
    parser.add_argument("-c", "--config", help="Path to config file", default="config.json")
    args = parser.parse_args()
    
    if not os.path.exists(args.config):
        raise FileNotFoundError(f"Config file '{args.config}' does not exist")

    with open(args.config) as json_data:
        stops = json.load(json_data)
        stop_ids = [s["stop"] for s in stops]
        next_stop_rock_bottom(stop_ids)

def next_stop_rock_bottom(stop_ids: List[str]) -> None:
    predictions = []

    for stop_id in stop_ids:
        # Find the common routes for the list of stops
        predictions = sorted(predictions +
            get_predictions_for_stop(stop_id), key=lambda x: x["minutes"])

    for next in predictions:
        print(str(next["route"]) + " " + next["direction"] + ", " + str(next["minutes"]) + " min")
    busStopPrinter.display_times(predictions)
    return

def get_predictions_for_stop(stop_id: str) -> List[dict]:
    routes = get_routes_for_stop(stop_id)

    # Find the next times for that stop on those routes
    next_stop_times = []
    for route in routes:
        stop_times_to_append = get_predicted_stop_times(
            stop_id, route["id"], route["direction_names"])
        if stop_times_to_append is not None:
            next_stop_times = next_stop_times + stop_times_to_append

    # Remove next stop times that are not in the future
    next_stop_times = [
        time for time in next_stop_times if time["minutes"] > 0 and time["minutes"] < 120]
    
    # Sort by minutes until arrival
    next_stop_times = sorted(next_stop_times, key=lambda x: x["minutes"])
    return next_stop_times    

def get_routes_for_stop(stop_id: str) -> List[dict]:
    # For the given stop ID, find the routes on that stop.
    routes_for_stops_json = get_stop_json(stop_id)

    return [{"id": route["id"], "direction_names": route["attributes"]["direction_names"]} for route in routes_for_stops_json]


def get_predicted_stop_times(stop_id: str, route_id: str, directions: List[str]) -> List[dict]:
    return_info = []

    request_url = (MBTA_REQUEST_URL_.format(request_type="predictions") + "page[limit]=3"
                    + create_filter_string("route", route_id)
                    + create_filter_string("stop", stop_id)
                    )
    with urllib.request.urlopen(request_url) as response:
        data = json.loads(response.read())["data"]
        if (len(data) == 0):
            print("No predictions found for route: " +
                    route_id + ", stop_id: " + stop_id)
            return return_info

        for arrival in data:
            arrival_time = datetime.fromisoformat(
                arrival["attributes"]["arrival_time"]).replace(tzinfo=None)
            minutes_until_arrival = round((
                arrival_time - datetime.now()).total_seconds() / 60)
            direction = directions[int(
                arrival["attributes"]["direction_id"])]
            return_info.append(
                {"route": route_id, "minutes": minutes_until_arrival, "direction": direction})

    return return_info


def create_filter_string(filter_name: str, filter_value: str) -> str:
    return FILTER_BASE_.format(name=filter_name, value=filter_value)

def get_stop_json(stop_id: int) -> Any:
    local_json_file = "stop_" + str(stop_id) + ".json"
    local_json = []

    # If a file exists with that stop info, try to load from it
    if os.path.exists(local_json_file):
        with open(local_json_file) as json_data:
            local_json = json.load(json_data)

    # If the load failed or was empty, poll the MBTA API for info
    if not local_json:
        route_request_url = ROUTES_URL_ + \
            create_filter_string("stop", str(stop_id))
        with urllib.request.urlopen(route_request_url) as response:
            local_json = json.loads(response.read())["data"]
            # Save the data to a local file for next time
            with open(local_json_file, 'w', encoding='utf-8') as f:
                json.dump(local_json, f, ensure_ascii=False, indent=4)
    return local_json

if __name__ == '__main__':
    main()
