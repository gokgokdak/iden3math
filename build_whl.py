import os
import platform
import shutil
import sys
from pathlib import Path


OUTPUT_DIR: str = os.path.join(os.path.dirname(__file__), 'product/wheel').replace('\\', '/')

CMAKE_TEMPLATE: str = """
[build-system]
requires = ["scikit-build-core", "pybind11"]
build-backend = "scikit_build_core.build"

[project]
name = "iden3math"
version = "{}"
readme = {{ file = "README.md", content-type = "text/markdown" }}
license = {{ file = "LICENSE" }}
requires-python = ">=3.8,<3.15"

[tool.scikit-build]
cmake.minimum-version = "3.18"
cmake.verbose = false
cmake.build-type = "Release"
cmake.args = [
    "-DBUILD_PY=ON",
    "-DBUILD_WHL=ON"{}
]

[tool.cibuildwheel]
skip = ["*-win32", "*-ppc64le", "*-s390x"]
"""


if __name__ == "__main__":
    # Get version.txt
    with open('version.txt', 'r') as f:
        version: str = f.read().strip()

    # Get cmake options
    cmake_args: str = ',\n'
    for arg in sys.argv[1:]:
        arg = arg.replace('\\', '\\\\')
        cmake_args += f'    "{arg}",\n'
    cmake_args = cmake_args.rstrip(',\n')

    # Format the template with the cmake arguments
    print(CMAKE_TEMPLATE)
    pyproject_toml: str = CMAKE_TEMPLATE.format(version, cmake_args)

    # Write to pyproject.toml
    with open('pyproject.toml', 'w') as f:
        f.write(pyproject_toml)

    # Remove contents under output direcotry
    path: Path = Path(OUTPUT_DIR)
    if path.exists() and path.is_dir():
        for child in path.iterdir():
            if child.is_dir():
                shutil.rmtree(child)
            else:
                child.unlink()

    # Run cibuildwheel
    if 'win' in platform.system().lower():
        os.system(f'set PIP_NO_VERIFY_CERTS=1 & set PIP_TRUSTED_HOST=pypi.org & cibuildwheel --output-dir {OUTPUT_DIR}')
    elif 'linux' in platform.system().lower() or 'darwin' in platform.system().lower():
        os.system(f'export PIP_NO_VERIFY_CERTS=1; export PIP_TRUSTED_HOST=pypi.org; cibuildwheel --output-dir {OUTPUT_DIR}')
    else:
        os.system(f'cibuildwheel --output-dir {OUTPUT_DIR}')
