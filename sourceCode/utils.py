import os
import yaml
import logging
from pathlib import Path
from dotenv import load_dotenv

# Configure logging with timestamps
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s - %(levelname)s - %(message)s",
)

def load_env_variables():
    """Load environment variables from .env file."""
    load_dotenv()

def load_config(config_path=None):
    """Load YAML configuration file."""
    if config_path is None:
        config_path = Path(__file__).resolve().parent.parent / 'config' / 'config.yaml'
    else:
        config_path = Path(config_path).resolve()

    logging.info(f"Loading config from: {config_path}")

    # Check if file exists
    if not config_path.is_file():
        raise FileNotFoundError(f"Config file not found at: {config_path}")

    try:
        with config_path.open('r') as file:  # More readable pathlib usage
            config = yaml.safe_load(file)
    except yaml.YAMLError as e:
        raise ValueError(f"Error parsing YAML file at {config_path}: {e}")
    except Exception as e:
        raise RuntimeError(f"Unexpected error loading config: {e}")

    return config

def get_api_key():
    """Retrieve API key from environment variables."""
    load_env_variables()  # Ensure environment variables are loaded
    api_key = os.getenv('API_KEY')
    if not api_key:
        logging.warning("API_KEY is not set in the environment.")
        return None  # Return None instead of raising an error
    return api_key

def get_supabase_url():
    """Retrieve Supabase URL from environment variables."""
    load_env_variables()  # Ensure environment variables are loaded
    surl = os.getenv('SUPABASE_URL')
    if not surl:
        logging.warning("SUPABASE_URL is not set in the environment.")
        return None  # Return None if SUPABASE_URL is not set
    return surl