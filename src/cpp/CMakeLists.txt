# Get the pybind11 cmake path from the Python environment
execute_process(
    COMMAND python -c "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE pybind11_DIR
)

set(CMAKE_PREFIX_PATH "${pybind11_DIR}" ${CMAKE_PREFIX_PATH})

# Find Python
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# Find pybind11
find_package(pybind11 CONFIG REQUIRED)

# Create the pybind11 module
pybind11_add_module(_rocksdb_cpp     
    rocksdb_module.cpp
    db_wrapper.cpp
    iterator_wrapper.cpp
    batch_wrapper.cpp
)

# Add include directories for our code
target_include_directories(_rocksdb_cpp PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp
    ${ROCKSDB_SOURCE_DIR}/include
)

add_dependencies(_rocksdb_cpp rocksdb)
target_link_libraries(_rocksdb_cpp PRIVATE rocksdb)

# Set output name
set_target_properties(_rocksdb_cpp PROPERTIES OUTPUT_NAME _rocksdb_cpp)

# Define where to install the module
install(TARGETS _rocksdb_cpp DESTINATION .)
