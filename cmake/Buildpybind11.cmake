include(ExternalProject)

# pybind11 source
set(pybind11_URL https://github.com/pybind/pybind11.git)
set(pybind11_TAG v2.13.6)

# Download pybind11
ExternalProject_Add(
  pybind11
  PREFIX pybind11
  GIT_REPOSITORY ${pybind11_URL}
  GIT_TAG ${pybind11_TAG}
  BUILD_IN_SOURCE 0
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(pybind11 SOURCE_DIR)
set(pybind11_INCLUDE_DIR "${SOURCE_DIR}/include")

find_package(pybind11 REQUIRED)
