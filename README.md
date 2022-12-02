[//]: <> (Comment: -*- coding: utf-8-with-signature -*-)
<img src="https://www.cea.fr/PublishingImages/cea.jpg" height="50" align="right" />
<img src="https://www.ifpenergiesnouvelles.fr/sites/ifpen.fr/files/logo_ifpen_2.jpg" height="50" align="right"/>

Written by CEA/IFPEN and Contributors

(C) Copyright 2000-2022 CEA/IFPEN. All rights reserved.

All content is the property of the respective authors or their employers.
For more information regarding authorship of content, please consult the listed source code repository logs.

## Compilation
To compile the application you will need the arcaneframework, alien opensource and alien legacy plugins (check the corresponding repos for prerequisites and compilation).

Here is the list of required (?) softwares to compile ShArc:
| Currently            | Loaded                           | Modules                             | :                        |
| ---------------------|----------------------------------|-------------------------------------|------------------------  |
|  1) GCCcore/7.3.0    | 15) libunwind/1.2.1              | 29) hwloc/1.11.10                   | 43) Ninja/1.9.0          |
|  2) zlib/1.2.11      | 16) ICU/61.1                     | 30) tbb/2018_U6                     | 44) Eigen/3.3.4          |
|  3) GCC/7.3.0-2.30   | 17) dotNET-SDK/6.0.101-linux-x64 | 31) CUDA/10.0.130                   | 45) GeometryKernel/2013  |
|  4) impi/2018.3.222  | 18) PAPI/5.6.0                   | 32) Boost/1.67.0                    | 46) googletest/1.10.0    |
|  5) imkl/2018.3.222  | 19) PCRE/8.41                    | 33) Szip/2.1.1                      | 47) APR/1.6.3            |
|  6) gimkl/2018b      | 20) SWIG/4.0.2                   | 34) HDF5/1.10.4                     | 48) libreadline/7.0      |
|  7) ncurses/6.1      | 21) expat/2.2.5                  | 35) ParMETIS/4.0.3                  | 49) Tcl/8.6.8            |
|  8) bzip2/1.0.6      | 22) UDUNITS/2.2.26               | 36) SCOTCH/6.0.5                    | 50) SQLite/3.24.0        |
|  9) cURL/7.60.0      | 23) Xerces-C++/3.2.2             | 37) Zoltan/3.83                     | 51) APR-util/1.6.1       |
| 10) libarchive/3.5.1 | 24) binutils/2.30                | 38) libffi/3.2.1                    | 52) Serf/1.3.9           |
| 11) OpenSSL/1.1      | 25) numactl/2.0.11               | 39) gettext/0.19.8.1                | 53) gperftools/2.6.3     |
| 12) CMake/3.21.1     | 26) libiconv/1.15                | 40) util-linux/2.32                 | 54) GMP/6.1.2            |
| 13) Mono/5.10.0.160  | 27) libxml2/2.9.8                | 41) GLib/2.54.3                     |                          |
| 14) XZ/5.2.4         | 28) libpciaccess/0.14            | 42) ifort/2018.3.222-GCC-7.3.0-2.30 |                          |

To configure ShArc:

    mkdir build && cd build
    cmake -S /my/path/to/sharc -B /my/path/to/sharc/build -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON -DArccon_DIR=/my/path/to/Arcane.framework/install/share/cmake/Arccon -DAxlstar_DIR=/my/path/to/Arcane.framework/install/share/cmake/Axlstar -DAlienProd_DIR=/my/path/to/Alien.legacy_plugins/install/lib/cmake -DUSE_ALIEN_V20=1 -DUSE_ARCANE_V3=1 -DArcane_DIR=/my/path/to/Arcane.framework/install/lib/cmake/Arcane -DAlienPath=/my/path/to/Alien.legacy_plugins/install/ -DArccore_DIR=/my/path/to/Arcane.framework/install/lib/cmake/Arccore

To compile it:

    make -j 8
