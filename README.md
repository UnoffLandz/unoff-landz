unoff-landz
===========

open source server for the eternal lands/other life client


## Building the project
### pre-requisites
    LibEV  
    LibSqlite3 

### Linux - CMake
Install cmake, plus relatively recent gcc/clang compiler.
The build process after acquiring/installing those should be straightforward:

```bash
    cd <cloned_repository_dir>
    mkdir build_dir
    cd build_dir
    cmake ..
    make
```