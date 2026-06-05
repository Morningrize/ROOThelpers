# ROOThelpers

A C++ library providing helper functions for ROOT analysis.

## Features

- Simple helpers for ROOT objects (e.g., RAII wrapper, axis styling ...)

## Usage

### As an External Dependency (`find_package`, no install required)

Like `AnalysisCoreCpp`, this project exports a build-tree package config. This means users can build `ROOThelpers` however they want and consume it with `find_package` directly from the build directory.

1. **Configure/build ROOThelpers**
   ```bash
   cmake -S /path/to/ROOThelpers -B /path/to/ROOThelpers/build
   cmake --build /path/to/ROOThelpers/build
   ```

2. **Point your consumer project to ROOThelpers build package dir**
   ```bash
   cmake -S . -B build -DROOThelpers_DIR=/path/to/ROOThelpers/build/cmake
   ```

3. **In your consumer `CMakeLists.txt`:**
   ```cmake
   find_package(ROOThelpers CONFIG REQUIRED)
   target_link_libraries(your_target PRIVATE ROOThelpers::ROOThelpers)
   ```

4. **Include the headers in your code:**
    ```cpp
    #include "axis_style.h"
    #include <TH1F.h>

    TH1F h("h", "test", 10, 0, 1);
    ROOThelpers::styleAxes(&h, "X axis", "Y axis");
    ```

The `ROOThelpers` library will automatically add the correct include path for you, so you do **not** need to copy headers or manually set include directories.

### Dependency Notes

Users of `ROOThelpers` must make `AnalysisCoreCpp`, `cxxopts`, and `ROOT` discoverable to CMake in their environment (for example via `CMAKE_PREFIX_PATH`, `AnalysisCoreCpp_DIR`, `cxxopts_DIR`, and `ROOT_DIR` as needed).

`cxxopts` is pulled in through `AnalysisCoreCpp`, so in this workflow you should treat `cxxopts_DIR` as required unless your `CMAKE_PREFIX_PATH` already resolves it.

`ROOThelpers` is built with C++20, so your ROOT build must be C++20-compatible as well.

## Testing

Tests are in the `tests/` directory. To build and run them:

```bash
mkdir build
cd build
cmake ..
make tests
ctest
```

## License

This project is licensed under the GNU General Public License (GPL). See the [LICENSE](LICENSE) file for details.
