import os
from skbuild import setup

setup(
    name="pyrocks11",
    version="0.1.0",
    description="Example package with pybind11 and scikit-build",
    author="Your Name",
    author_email="your.email@example.com",
    packages=["pyrocks11"],
    package_dir={"": "src"},
    cmake_source_dir=".",
    cmake_install_dir="src/pyrocks11",
    include_package_data=True,
    python_requires=">=3.10",
    cmake_args=[
        f"-DCMAKE_BUILD_PARALLEL_LEVEL={os.cpu_count() or 1}",
    ],
    package_data={
        "pyrocks11": ["_rocksdb_cpp.pyi"],
    }
)
