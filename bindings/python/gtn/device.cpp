#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include "gtn/gtn.h"

using namespace gtn;

namespace py = pybind11;
using namespace py::literals;

PYBIND11_MODULE(device, m) {
  py::enum_<DeviceType>(m, "DeviceType")
    .value("CPU", DeviceType::CPU)
    .value("CUDA", DeviceType::CUDA);

  m.attr("CPU") = DeviceType::CPU;
  m.attr("CUDA") = DeviceType::CUDA;

  // Declare device_class as a named variable of py::class_<Device>
  // and then chain the def calls
  py::class_<Device> device_class(m, "Device");

  device_class.def(py::init<DeviceType>(), "type"_a)
              .def(py::init<DeviceType, int>(), "type"_a, "index"_a)
              .def(py::self == py::self) // Now calling .def on the named variable
              .def(py::self != py::self); // And here
}
