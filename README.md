# cvarda

Varda2 Variant frequency database C library with Python module

## Python

### Prerequisites for macOS (Mark):
- `export CFLAGS=-isysroot/Library/Developer/CommandLineTools/SDKs/MacOSX10.14.sdk`

### Installation:
- `python3 -m venv venv`
- `source venv/bin/activate`
- `pip install --upgrade -v .`

### Testing
- `pip install -r requirements-dev.txt`
- `pytest -sv`

## C

`make`

or for a release build:

`make release` (maybe clean the build first?)


### Tests

`make check`

when valgrind is installed:

`make check MEMCHECK=TRUE`


## Documentation

Prerequisites:
  - doxygen
  - an active virtualenv with:
    - breathe: `pip install breathe`
    - sphinx_rtd_theme: `pip install sphinx_rtd_theme`
    - cvarda: `pip install --upgrade .`

`make docs`
