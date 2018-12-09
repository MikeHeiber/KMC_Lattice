# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

--------------------------------------------------------------------------------------------------------------------------------

## [Unreleased]

## [v2.0.0-rc.1] - 2018-12-08 - Release Candidate 1

### Added
- CONTRIBUTING.md - New file with instructions for how others can contribute to the project
- README.md - Link to new CONTRIBUTING file
- CHANGELOG.md - New file detailing the changes for each release
- README.md - Link to new CHANGELOG file
- README.md - More detailed instructions for building and testing KMC_Lattice
- Object (calculateDisplacement) - New optional direction parameter that allows users to tell the function to calculate the displacement along a specific Cartesian direction
- Simulation (getN_events) - Cast return of count_if function to int to prevent compiler warning
- Utils (calculateProbabilityHist) - Float versions of several calculateProbabilityHist functions
- Utils (calculateProbabilityHist) - Filtering of input data vectors to remove inf and nan entries
- Utils (calculateProbabilityHist) - Check for and correct index calculation error due to floating point rounding
- Utils (createExponentialDOSVector) - Float version of the createExponentialDOSVector function to allow the user to conserve memory
- Utils (createGaussianDOSVector) - Float version of the createExponentialDOSVector function to allow the user to conserve memory
- Utils (vector_which_median) - Cast return of distance function to int to prevent compiler warning
- test.cpp (BKL_determinePathwayTests) - Cast return of count_if function to int to prevent compiler warning
- test.cpp (CalculateProbabilityHistTests) - Tests of the float versions of calculateProbabilityHist
- test.cpp (CalculateProbabilityHistTests) - Tests of both double and float versions of calculateProbabilityHist where the input data vectors contain some non finite entries
= test.cpp (EventExecutionTests) - Cast return of count_if function to int to prevent compiler warning
- test.cpp (ExponentialDOSTests) - Test of the float version of the createExponentialDOSVector
- test.cpp (ExponentialDOSTests) - Cast return of count_if function to int to prevent compiler warning
- test.cpp (GaussianDOSTests) - Test of the float version of the createGaussianDOSVector
- test.cpp (LatticeTests) - Cast return of count_if functions to int to prevent compiler warning
- test.cpp (ObjectTests) - Tests of the new calculateDisplacement function behavior


### Changed
- Doxyfile - Updated the version number to v2.0.0-rc.1
- docs - Generated updated Doxygen documentation
- Parameters_Lattice - Refactored struct into a new class
- test.cpp (AlgorithmTests) - Increased N_tests to gather more statistics and reduce likelihood of test failure
- Utils (createExponentialDOSVector) - Refactored code to make selection of the Gaussian side or the exponential side of the dist more simple
- Utils (createGaussianDOSVector) - Refactored code to use generate function for filling the output data vector
- Utils (MPI_*) - Refactored functions by replacing for loops with copy, assign, and accumulate functions to make code more concise
- test.cpp (main) - Re-enabled command line output during the tests to make it easier to see when errors occur
- test.cpp (CalculateProbabilityHistTests) - Replaced generic unform_real_distribution by specifying use of the double version
- test.cpp (CalculateProbabilityHistTests) - Increasing the number of entries in the data vectors to increase statistical accuracy and reduce likelihood of test failure
- test.cpp (CalculateProbabilityHistTests) - Refactored tests to use the generate function for filling a vector with random numbers

### Fixed
- Event - Corrected the copyright years in the Doxygen documentation block for the class
- Lattice - Corrected the copyright years in the Doxygen documentation block for the class
- Object - Corrected the copyright years in the Doxygen documentation block for the class
- Simulation - Corrected the copyright years in the Doxygen documentation block for the class
- Site - Corrected the copyright years in the Doxygen documentation block for the class
