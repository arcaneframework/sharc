# Changelog

## [2026.1] - ArcGeoSim v2026.1.0.0 (2026-01-19)

### Added
- Last developments done on ArcGeoSim svn: MR#22.

### Changed
- Split tests directories, MR#24.

### Fixed
- Problem to use installed ArcGeoSim libraries on Windows (Visual 2017), MR#26, MR#27.
- Problem in Alien::AsciiDumper define and include, MR#25.

### Removed
- Deprecated boost-timer, MR#23.



## [2025.2] - ArcGeoSim v2025.2.1.0 (2025-12-01)

### Added

- Last developments done on ArcGeoSim svn: MR#19, MR#21.
- New CI-CD pipelines MR#16.
- preCICE unit tests, MR#15.
- Support for the Arcane GitHub version, MR#13.
- Support for monorepo Windows build, Visual 2017, MR#9. 


### Fixed

- Build: relative-path in the header includes, MR#12.
- Regression with ArcGeoSim libraries creation, MR#10.

### Removed

- Old build system, MR#8.

### CI-CD

- Support for foss2025, MR#20.
- Disable Windows jobs, MR#18.
- Separate between build and test jobs, MR#16, MR#17.
- Disable gimkl18 for builds based on the Arcane GitHub version, MR#14.

## [2025.1] - 2025-06-30

### Added

Creation of ArcGeoSim/ArcGeoSimR and ArcaneDemo monorepo gitlab version, MR#1 to MR#7.

- This version has been incubated on gitlab ArcGeoSim-framework. 
- The principal modification is the creation of the ArcGeoSim and ArcGeoSimR libraries
- Creation of the CICD pipeline.