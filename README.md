# Raspberry Pi

## Installation

Install `poetry`.
Install the dependencies using `poetry`:

```sh
poetry install
```

## Environment Variables and Configuration

Create a `.env` similar to `.env.sample` and set the values in the file.
Modify the `config/config.yaml` file if needed to set the required values.

## Running the project

```sh
poetry run python ....
```

### Locate the project folder in raspberry Pi and run a command that ressembles the following (via ssh)
```sh
cd repos/Vibration-Analysis-Project/raspberry 
PYTHONPATH=/home/VASILIS/repos/Vibration-Analysis-Project/raspberry poetry run python3.12 tests/test_methods.py

or

cd repos/Diploma \Thesis/raspberry 
PYTHONPATH=/home/VASILIS/repos/Diploma \Thesis/raspberry poetry run python3.12 testing/test_methods.py

and 

in the case of probing on a PC use relative imports and ensure everything is set up as part of a package and python locates folder and the file.
poetry run python -m testing.test_methods
```


## Activating the virtual environment

```sh
poetry shell
```

# Project Setup and Testing Guide

## Prerequisites
Ensure you have [Poetry](https://python-poetry.org/) installed on your system. If Poetry is not installed, you can install it using the following command:

```sh
curl -sSL https://install.python-poetry.org | python3 -
```

Additionally, ensure that Poetry’s binary path is available in your system’s `PATH` variable:

```sh
export PATH="$HOME/.local/bin:$PATH"
```

To make this change permanent, add the above line to your `~/.bashrc` or `~/.zshrc` file:

```sh
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc  # or source ~/.zshrc if using Zsh
```

## Setting Up the Environment
Once Poetry is installed, you need to activate the virtual environment created by Poetry. Run the following command:

```sh
source $(poetry env info --path)/bin/activate
```

This ensures that your Python environment is properly configured for the project.

## Running Tests
After setting up the environment, you can execute the test suite by running:

```sh
poetry run python -m testing.test_methods
```

This command ensures that Python runs the `test_methods.py` module inside the `testing` package within the Poetry environment.

## Troubleshooting
- If `poetry` is not recognized, restart your terminal or re-run the `export PATH` command.
- If the virtual environment is not found, recreate it using:

  ```sh
  poetry env remove python
  poetry install
  ```

This will remove the existing environment and install all dependencies again.

---

Now you have a fully documented setup guide in your `README.md` file! 🚀





## Deactivating the virtual environment

```sh
exit
```

