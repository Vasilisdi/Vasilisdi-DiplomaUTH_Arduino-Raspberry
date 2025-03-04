import requests
import json
import logging
from sourceCode.utils import load_config, get_api_key, get_supabase_url
from datetime import datetime, timedelta, timezone

config = load_config()

class VibrationMonitoringAPI:
    def __init__(self):
        # Load API key and Supabase URL using the helper functions
        self.api_key = get_api_key()
        self.surl = get_supabase_url()
        
        # Ensure both the API key and Supabase URL are available
        if not self.api_key:
            logging.error("API Key is missing.")
            raise ValueError("API Key is required for the API.")
        
        if not self.surl:
            logging.error("Supabase URL is missing.")
            raise ValueError("Supabase URL is required for the API.")

        # Initialize headers using the loaded API key
        self.headers = {
            'accept': 'application/json',
            'x-api-key': self.api_key,
            'Content-Type': 'application/json'
        }

        # Store the Supabase URL as an instance attribute
        self.url = self.surl  # Use the loaded Supabase URL
    
    def get_time(self, start_measurement, end_measurement):
        # Format the timestamp in the required format
        start_measurement_str = start_measurement.strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3] + 'Z'
        end_measurement_str = end_measurement.strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3] + 'Z'
        return start_measurement_str, end_measurement_str

    # end_measurement is now is not specified
    def send_measurement(self, coordinate, values, start_measurement = datetime.now(timezone.utc), end_measurement = datetime.now(timezone.utc)):  
        start_measurement_str, end_measurement_str = self.get_time(start_measurement, end_measurement)
        payload = {
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
        
        response = requests.post(self.url, headers=self.headers, data=json.dumps(payload))
        
        # Log the response status and data
        logging.info(f"API Response Status Code: {response.status_code}")
        logging.info(f"API Response JSON: {response.json()}")
        logging.info(f"API Key: {self.api_key}")
        logging.info(f"Headers: {self.headers}")
        
        return response.status_code, response.json()
