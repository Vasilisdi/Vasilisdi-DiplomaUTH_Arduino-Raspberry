import requests
import json
import logging
from sourceCode.utils import load_config, get_api_key, get_supabase_url_posting, get_supabase_url_selecting
from datetime import datetime, timezone
from zoneinfo import ZoneInfo

config = load_config()

class VibrationMonitoringAPI:
    def __init__(self):
        # Load API key and Supabase URL using the helper functions
        self.api_key = get_api_key()
        self.purl = get_supabase_url_posting()
        self.surl = get_supabase_url_selecting()
        
        # Ensure both the API key and Supabase URL are available
        if not self.api_key:
            logging.error("API Key is missing.")
            raise ValueError("API Key is required for the API.")
        
        if not self.purl:
            logging.error("Supabase URL is missing.")
            raise ValueError("Supabase URL is required for the API.")

        if not self.surl:
            logging.error("Supabase URL is missing.")
            raise ValueError("Supabase URL is required for the API.")



        # Initialize headers using the loaded API key
        self.headers = {
            'accept': 'application/json',
            'apikey': self.api_key,
            'Content-Type': 'application/json'
        }

        # Define the Athens time zone using ZoneInfo
        self.athens_tz = ZoneInfo('Europe/Athens')



    def get_time(self, start_measurement, end_measurement):

        # Convert the start and end times to Athens time zone
        start_measurement_athens = start_measurement.astimezone(self.athens_tz)
        end_measurement_athens = end_measurement.astimezone(self.athens_tz)

        # Format the timestamps in the required format with Athens time zone
        start_measurement_str = start_measurement_athens.strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3] + 'Z'
        end_measurement_str = end_measurement_athens.strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3] + 'Z'

        return start_measurement_str, end_measurement_str

    # end_measurement is now is not specified
    def send_measurement(self, sensor_name, coordinate, values, start_measurement = datetime.now(timezone.utc), end_measurement = datetime.now(timezone.utc)):
        start_measurement_str, end_measurement_str = self.get_time(start_measurement, end_measurement)

        # Lookup sensor_id from sensor_name
        sensor_id = self.get_sensor_id(sensor_name)
        if not sensor_id:
            logging.error(f"Sensor with name '{sensor_name}' not found.")
            return None

        payload = {
            "sensor_id":sensor_id,
            "startMeasurement": start_measurement_str,
            "endMeasurement": end_measurement_str,
            "coordinate": coordinate,
            "values": values
        }

#edw exoume thema me to rpm na travhksoume apo config
#na diorthosoume to out of range sta coordinates - na dw giati to kanei
#na doume giati ksekinaei 2 fores me to yalm 
#na doyme to cronjob
        logging.info(f"Payload being sent: {json.dumps(payload, indent=4)}")
        
        response = requests.post(self.purl, headers=self.headers, data=json.dumps(payload))
        
        # Log the response status and data
        logging.info(f"API Response Status Code: {response.status_code}")
        # ✅ Check if response has content before parsing JSON
        if response.text.strip():  # Ensure response is not empty
            try:
                logging.info(f"API Response JSON: {response.json()}")
            except requests.exceptions.JSONDecodeError:
                logging.warning("API Response is not JSON (empty or invalid format).")
        else:
            logging.info("API Response is empty (no JSON returned).")

        
        return response.status_code, response.json()


    # New method to get sensor_id from sensor_name
    def get_sensor_id(self, sensor_name):
        query_url = f"{self.surl}?sensor_name=eq.{sensor_name}"
        print(query_url)
        response = requests.get(query_url, headers=self.headers)
        print(response)
        if response.status_code == 200:
            data = response.json()
            if data:
                return data[0]['id']
        else:
            logging.error(f"Failed to fetch sensor_id for sensor_name '{sensor_name}': {response.status_code} - {response.text}")
        return None