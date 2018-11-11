Duplicated Folders Finder
=========================

# Summary
- [Description](#description)
- [Compilation](#compilation-requirements)
- [Install](#install)
- [Screenshots](#screenshots)
- [Repository information](#repository-information)

# Description
Little tool to search and list the names and sizes of folders with the same name hanging from the same root in the directory tree. So basically a tool to report duplicates in storage
folders that follow some naming rules. It doesn't compare directory contents, only reports directories with the same name. 

# Compilation requirements
## To build the tool:
* cross-platform build system: [CMake](http://www.cmake.org/cmake/resources/software.html).
* compiler: [Mingw64](http://sourceforge.net/projects/mingw-w64/) on Windows.

## External dependencies
The following libraries are required:
* [Qt opensource framework](http://www.qt.io/).
* [Boost libraries](https://www.boost.org/).

# Install
The only current option is build from source as binaries are not provided.

# Screenshots
No screenshots.

# Repository information

**Version**: 1.0.0

**Status**: finished

**cloc statistics**

| Language                     |files          |blank        |comment           |code  |
|:-----------------------------|--------------:|------------:|-----------------:|-----:|
| C++                          |   3           | 89          | 67               | 268  |
| C/C++ Header                 |   2           | 34          | 94               |  60  |
| CMake                        |   1           | 19          |  6               |  46  |
| **Total**                    | **6**         | **142**     | **167**          | **374** |
