# Repository Introduction

This repository contains 2 different running sections. The first section is this of testing. There is the files test_data_from_datasets1.py and test_data_from_datasets2.py. These 2 files run the raw test data files, that are uploaded in the testing folder. The frequenct spectra of the defective machine waveforms are to be ploted on some figure for visual analysis and illustration. (following detailed instructions on this).

A results folder is stored in the testing folder. The aforementioned results will be saved in there.

In addition to this, there is the sourceCode folder, containing the main.py file and some other auxiliary files. This file is designed for the case of the continuous monitoring case, using cronjob set at collecting data four times a day. (further instructions bellow)

- test_data_from_datasets1.py for test datasets model validation.
- test_data_from_datasets2.py for test datasets model validation.
- main.py for real-life machine applications.


# Testing Phase Running


## Prerequisites

Ensure you have [Poetry](https://python-poetry.org/) installed on your system. If Poetry is not installed, you can install it using the following command:

```sh
curl -sSL https://install.python-poetry.org | python3 -
```

Additionally, ensure that Poetry’s binary path is available in your system’s `PATH` variable:

```sh
export PATH="$HOME/.local/bin:$PATH"
```

To make this change permanent (not mandatory), add the above line to your `~/.bashrc` or `~/.zshrc` file:

```sh
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc  # or source ~/.zshrc if using Zsh
```

Also ensure the an .env file is created following the .env.example file as a template. This is going to be filled with sensitive credentials of your supabase project.


## Installation

Install `poetry`. Install the dependencies using `poetry`:

```sh
poetry install
```

## Environment Variables and Configuration

Create a `.env` similar to `.env.sample` and set the values in the file. Modify the `config/config.yaml` file if needed to set the required values.

## Activating the virtual environment

```sh
poetry shell
```

An alternative to activating the virtual environment created by Poetry is running the following command (applicable in newer versions of poetry):

```sh
source $(poetry env info --path)/bin/activate
```

This ensures that your Python environment is properly configured for the project.


### Troubleshooting

- If `poetry` is not recognized, restart your terminal or re-run the `export PATH` command.
- If the virtual environment is not found, recreate it using:

### Reinstall all dependencies

Remove the existing Poetry environment
```sh
poetry env remove python
```
And reinstall
```sh
poetry install  # Installs all necessary libraries
```

### Test data posting to Supabase
```sh
poetry run python -c "from sourceCode.api import VibrationMonitoringAPI; api = VibrationMonitoringAPI(); api.send_measurement(sensor_name='Test Sensor', coordinate='TEST', values=[1, 2, 3, 4, 5])" # Verifies the ability to post data to Supabase
```


### Locate the project folder in Raspberry Pi and run a command that resembles the following (via SSH):

```sh
cd repos/Vibration-Analysis-Project/raspberry
PYTHONPATH=/home/VASILIS/repos/Vibration-Analysis-Project/raspberry poetry run python3.12 tests/test_methods.py

or

cd repos/Diploma\ Thesis/raspberry
PYTHONPATH=/home/VASILIS/repos/Diploma\ Thesis/raspberry poetry run python3.12 testing/test_methods.py

and

in the case of probing on a PC use relative imports and ensure everything is set up as part of a package and Python locates the folder and the file.
poetry run python -m testing.test_methods
```

## Debugging Poetry Import Issues

If you encounter issues with module imports, run the following command to debug:

```sh
poetry run python -c "from sourceCode.utils import load_config; print(load_config())"
```

This command directly imports and prints the result of `load_config()` from `sourceCode.utils`. If an error occurs:

- Ensure `sourceCode` exists and contains an `__init__.py` file.
- Check that `sourceCode.utils` is accessible within the Poetry environment.
- Verify `PYTHONPATH` includes the correct project directory.
- Run `poetry shell` before executing Python commands to ensure the correct environment is active.

### Debugging API Configuration

To verify that the API key and Supabase URL are correctly loaded, run:

```sh
poetry run python -c "from sourceCode.api import VibrationMonitoringAPI; api = VibrationMonitoringAPI(); print(api.purl, api.api_key)"
```

This command initializes the `VibrationMonitoringAPI` class and prints the loaded API key and Supabase URL.

As articulated above, there is also the option of checking the data posting possiblility:
```sh
poetry run python -c "from sourceCode.api import VibrationMonitoringAPI; api = VibrationMonitoringAPI(); api.send_measurement(sensor_name='Test Sensor', coordinate='TEST', values=[1, 2, 3, 4, 5])" # Verifies the ability to post data to Supabase
```


## Deactivating the virtual environment

```sh
exit
```



# Testing and Validation
## Running Tests files

After setting up the environment, you can execute the test suite by running:

```sh
poetry run python -m testing.test_methods
```

This command ensures that Python runs the `test_methods.py` module inside the `testing` package within the Poetry environment.



## Concept validation

To run tests for processing vibration datasets:

```sh
poetry run python -m testing.test_data_from_datasets1
poetry run python -m testing.test_data_from_datasets2
```

These commands are used for the faulty machinery datasets included in the folders:

-Test processing for the VBL-VA001 dataset "poetry run python -m testing.test_data_from_datasets1"
-Test processing for the HUST Bearing dataset "poetry run python -m testing.test_data_from_datasets2"

Data files are mixed but they can be easily identified since HUST Bearing dataset uses the following format "I802" while the VBL-VA001 dataset is formated like so: "ub_06_000-3_Ch08_100g_PE_Acceleration".

This executable file have as an input the time-domain waveforms and convert data into the frequency domain.







# Actual - Raspberry Connected to Arduino Phase

In this case, the main script is to be executed so as to post real data on the supabase:

```sh
poetry run python sourceCode/main.py
```


