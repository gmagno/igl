## glad
This directory is reserved for `glad`, the OpenGL loader. For more details
check (https://github.com/Dav1dde/glad).

## Installation
The recommended way to install `glad` is using their python tool. Use a
virtual environment to keep things tidy:

```bash
# create a python virtual environment, activate it and upgrade pip
python3 -m venv .venv
source .venv/bin/activate
pip install -U pip

# install glad
pip install glad

# pull the loader files and install them in ext/glad
glad --generator c --out-path ext/glad

# there is no need to keep this venv activated
deactivate
```

You can now build the project!
