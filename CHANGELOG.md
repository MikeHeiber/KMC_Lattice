# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

--------------------------------------------------------------------------------------------------------------------------------

## [Unreleased]

### Added
- README - Citation info and links for the JOSS paper
- .gitignore - Statement to ignore the packages directory created by MS Visual Studio
- Utils (MPI_calculatePairVectorAvg) - new function for calculating the average of x-y distribution data
- test_mpi.cpp (CalculatePairVectorAvgTests) - new test case with units tests for the Utils::MPI_calculatePairVectorAvg function
- test_mpi.cpp (CalculateProbabilityHistTests) - new units tests to check for the output histogram range and to check exception throwing when input histograms are too small or have different bin sizes

### Changed
- Doxyfile - Documentation version to v2.0.1-beta.1
- docs - Updated Doxygen documentation for v2.0.1-beta.1
- Utils (MPI_calculateProbHistAvg) - Updated function to throw invalid_argument exception on all processors if the input histogram on any proc is too small or if bin sizes do not match
- Version - Current version string to v2.1.0-beta.1

## [v2.0.1] - 2019-01-29

### Changed
- Version - Current version string to v2.0.1
- Doxyfile - Documentation version to v2.0.1
- docs - Updated Doxygen documentation for v2.0.1
- README - Information about latest stable release to v2.0.1

### Fixed
- CHANGELOG - v2.0.0 release date
- papers/v2_paper/paper.md - Fixed typo

## [v2.0.0] - 2019-01-23

### Added
- .gitignore - Ignore statement for the Visual Studio x64 build directory
- paper.bib - Missing reference info to several of the book chapter bibtex entries
- Parameters_Lattice - Default values for all parameters
- Parameters_Simulation - Default values for all parameters
- test.cpp (SimulationTest) - Test that default parameters object has invalid parameters
- test.cpp (LatticeTest) - Test that default parameters object has invalid parameters

### Changed
- README.md - Current status to note release of stable v2.0.0

### Fixed
- docs - Several spelling mistakes and typos in the documentation
- test.cpp - Copyright years in the Doxygen documentation block
- test_mpi.cpp - Copyright years in the Doxygen documentation block
- paper.md - Typos and formatting issues
- paper.bib - Formatting problems in bibtex entries

## [v2.0.0-rc.2] - 2019-01-07 - Release Candidate 2

### Added
- README.md - Information about the BKL algorithm and the Gillespie first-reaction method implemented in the tools with link to the original papers
- README.md - Information about and link to the KMC_Lattice_example repo
- README.md - Major features list
- README.md - Section heading for the API documentation, citation instructions, and acknowledgments
- papers/v2_paper/paper.md - Initial draft of v2.0 JOSS paper text
- papers/v2_paper/paper.bib - Initial draft of v2.0 JOSS paper references
- .gitignore - Ignore statements for Visual Studio project files

### Changed
- Many files - Copyright statement to show years 2017-2019
- README.md - Rearranged sections and moved the build instructions to the end of the usage instructions sections
- Version.cpp - Version string to v2.0.0
- Doxyfile - project version number to v2.0.0 and brief project description
- docs - Re-generated documentation using latest Doxygen v1.8.15

### Removed
- docs - Repository markdown files from the documentation build

## [v2.0.0-rc.1] - 2018-12-11 - Release Candidate 1

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
- test.cpp (CalculateProbabilityHistTests) - Replaced generic uniform_real_distribution by specifying use of the double version
- test.cpp (CalculateProbabilityHistTests) - Increasing the number of entries in the data vectors to increase statistical accuracy and reduce likelihood of test failure
- test.cpp (CalculateProbabilityHistTests) - Refactored tests to use the generate function for filling a vector with random numbers

### Fixed
- Event - Copyright years in the Doxygen documentation block for the class
- Lattice - Copyright years in the Doxygen documentation block for the class
- Object - Copyright years in the Doxygen documentation block for the class
- Simulation - Copyright years in the Doxygen documentation block for the class
- Site - Copyright years in the Doxygen documentation block for the class
