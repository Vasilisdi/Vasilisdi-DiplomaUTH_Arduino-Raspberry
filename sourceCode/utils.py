import os
import yaml
from dotenv import load_dotenv
load_dotenv()

def load_config(config_path=None):
    if config_path is None:
        config_path = os.path.join(os.path.dirname(__file__), '..', 'config', 'config.yaml')
    config_path = os.path.abspath(config_path)  # Ensure the path is absolute
    print(f"Loading config from: {config_path}")  # Debugging line
    with open(config_path, 'r') as file:
        config = yaml.safe_load(file)
    return config

def get_api_key():
    return os.getenv('API_KEY')