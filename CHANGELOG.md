# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

--------------------------------------------------------------------------------------------------------------------------------

## [v2.0.0-rc.1] - 2018-12-04 - Release Candidate 1

### Added
- CONTRIBUTING.md - New file with instructions for how others can contribute to the project
- README.md - Link to new CONTRIBUTING file
- CHANGELOG.md - New file detailing the changes for each release
- README.md - Link to new CHANGELOG file
- Object (calculateDisplacement) - New optional direction parameter that allows users to tell the function to calculate the displacement along a specific Cartesian direction
- Utils (calculateProbabilityHist) - Float versions of several calculateProbabilityHist functions
- Utils (createExponentialDOSVector) - Float version of the createExponentialDOSVector function to allow the user to conserve memory
- Utils (createGaussianDOSVector) - Float version of the createExponentialDOSVector function to allow the user to conserve memory
- test.cpp (ExponentialDOSTests) - Test of the float version of the createExponentialDOSVector
- test.cpp (GaussianDOSTests) - Test of the float version of the createGaussianDOSVector
- test.cpp (ObjectTests) - Tests of the new calculateDisplacement function behavior

### Changed
- Doxyfile - Updated the version number to v2.0.0-rc.1
- docs - Generated updated Doxygen documentation
- test.cpp (AlgorithmTests) - Increased N_tests to gather more statistics and reduce likelihood of test failure
- Utils (createExponentialDOSVector) - Refactored code to make selection of the Gaussian side or the exponential side of the dist more simple
- Utils (MPI_*) - Refactored functions by replacing for loops with copy, assign, and accumulate functions to make code more concise
- test.cpp (main) - Re-enabled command line output during the tests to make it easier to see when errors occur

### Fixed
- Event - Corrected the copyright years in the Doxygen documentation block for the class
- Lattice - Corrected the copyright years in the Doxygen documentation block for the class
- Object - Corrected the copyright years in the Doxygen documentation block for the class
- Simulation - Corrected the copyright years in the Doxygen documentation block for the class
- Site - Corrected the copyright years in the Doxygen documentation block for the class
