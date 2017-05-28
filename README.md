
## Introduction

A tool for loading multiple copies of a DLL/SO in memory at the same time

Derived from https://github.com/fancycode/MemoryModule

## LICENSE

MPL v2.0, because MemoryModule is licensed with MPL v2.0

## Build/run

```
mkdir build
cd build
cmake .. -G "Visual Studio 14 2015 Win64"
cmake --build . --config Release
Release\simple_tests.exe
```