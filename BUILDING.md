# Building with CMake

## Requirements
The following packages are required to build the Point cloud preparation tool:
* cmake >= 3.21
* GNU Make
* gcc
* g++
* libpng-dev 

For Ubuntu, you can run the following:
  ```shell
  sudo apt-get install cmake make gcc g++ libpng-dev
  
  # (OPTIONAL) to build the tool with desktop mode (GPU support)
  # install the following additional dependencies 
  sudo apt-get install libglfw3-dev libglew-dev libgl-dev

  # (OPTIONAL) dependencies for example binaries 
  sudo apt-get install libglm-dev
```

## Build 
### Linux (Tested on Ubuntu Desktop & Server)
 
```sh
# Remove the comments before running the below commands
cmake -S . -B build             \
    -D CMAKE_BUILD_TYPE=Release \ # Build type
    -D BUILD_SHARED_LIBS=YES    \ # Build shared library instead (OPTIONAL)
    -D BUILD_APP=YES            \ # Build `pcp` app (OPTIONAL)
    -D BUILD_EXAMPLES=YES       \ # Build examples (OPTIONAL)
    -D DESKTOP_MODE=YES         \ # Use OpenGL for rendering (OPTIONAL)
    -D BUILD_VPCC=YES             # Build along with VPCC's binaries for pcprep.sh (OPTIONAL)

cmake --build build -j$(nproc)
```

## Install
This project doesn't require any special command-line flags to install to keep things simple. As a prerequisite, the project has to be built with the above commands already.

```sh
cmake --install build
```

### CMake package

This project exports a CMake package to be used with the [`find_package`][3]
command of CMake:

* Package name: `pcprep`
* Target name: `pcprep::pcprep`

Example usage:

```cmake
find_package(pcprep REQUIRED)
# Declare the imported target as a build requirement using PRIVATE, where
# project_target is a target created in the consuming project
target_link_libraries(
    project_target PRIVATE
    pcprep::pcprep
)
```

### Note to packagers

The `CMAKE_INSTALL_INCLUDEDIR` is set to a path other than just `include` if
the project is configured as a top level project to avoid indirectly including
other libraries when installed to a common prefix. Please review the
[install-rules.cmake](cmake/install-rules.cmake) file for the full set of
install rules.

[3]: https://cmake.org/cmake/help/latest/command/find_package.html
