
## Introduction

A tool for loading multiple copies of a DLL/SO in memory at the same time.  In practice this interface is designed to work with the REFPROP library, but it provides a generic interface that can be used with a wide range of shared libraries.

Based upon https://github.com/fancycode/MemoryModule

Works on windows and linux

## Contact

Code of ``manyso`` written by Ian Bell, with help from Tobias Loew.  Please open an issue on github for assistance: https://github.com/usnistgov/manyso/issues/new

## License

MemoryModule is licensed with MPL v2.0, manyso is in the public domain.  Please see LICENSE.txt for more information.

## Build/run

```
mkdir build
cd build
cmake .. -G "Visual Studio 14 2015 Win64"
cmake --build . --config Release
Release\simple_test.exe
Release\refpropwrap_test.exe
```