#pragma once
#include <pybind11/pybind11.h>
#include <rocksdb/db.h>

namespace py = pybind11;

inline rocksdb::Slice toslice(const py::bytes& v) {
    char *buffer;
    Py_ssize_t length;
    // Get the raw buffer from bytes
    if (PYBIND11_BYTES_AS_STRING_AND_SIZE(v.ptr(), &buffer, &length) == -1) {
        throw py::error_already_set();
    }
    return rocksdb::Slice(buffer, length);
}
