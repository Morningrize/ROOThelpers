# ROOThelpers

A header-only C++ library providing helper functions for ROOT analysis.

## Features

- Simple helpers for ROOT objects (e.g., axis styling)

## Usage

### As an External Dependency

You can add ROOThelpers to your project without copying headers manually by using CMake’s `add_subdirectory()` 

1. **Add ROOThelpers to your project**  
   - Clone the repo

2. **Set the ${ROOThelpers_DIR} environment variable**
    ```bash
    export ROOThelpers_DIR="/path/to/ROOThelpers"
    ```

3. **In your CMakeLists.txt:**
    ```cmake
    add_subdirectory($ENV{ROOThelpers_DIR})
    target_link_libraries(your_target PRIVATE ROOThelpers)
    ```

4. **Include the headers in your code:**
    ```cpp
    #include "axis_style.h"
    #include <TH1F.h>

    TH1F h("h", "test", 10, 0, 1);
    ROOThelpers::style_axis(&h, "X axis", "Y axis");
    ```

The `ROOThelpers` INTERFACE library will automatically add the correct include path for you, so you do **not** need to copy headers or manually set include directories.

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
