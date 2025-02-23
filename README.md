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



## Deactivating the virtual environment

```sh
exit
```

