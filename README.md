# KMC_Lattice 

## General Information

This object-oriented C++ library contains a general framework for lattice kinetic Monte Carlo (KMC) simulations. 
This framework consists of a number of utility functions and base classes that must be extended to create a fully operational KMC simulation. 
The goal of this package is to be robust, reliable, and flexible so that users can easily develop KMC simulations for a wide variety of different scientific problems without the need to start from scratch.

KMC_Lattice implements the [BKL algorithm](https://doi.org/10.1016/0021-9991(75)90060-1) and the [Gillespie first-reaction method](https://doi.org/10.1016/0021-9991(76)90041-3) with several event recalculation options for creating computationally efficient simulations. 
The package is designed to be usable on a personal computer and on high performance computing clusters. 
For a simple, well-documented example of how to use this library, please see the [KMC_Lattice_example](https://github.com/MikeHeiber/KMC_Lattice_example) repository.
For an example of a more complex and fully functional computational research tool constructed using KMC_Lattice, check out the [Excimontec](https://github.com/MikeHeiber/Excimontec) software package, which is designed for simulating organic semiconductor devices.

For further reading about kinetic Monte Carlo simulations, nice overviews of the theory and algorithms can be found here:
- [Introduction to the Kinetic Monte Carlo Method by Arthur Voter, Los Alamos National Lab](http://www.fml.t.u-tokyo.ac.jp/~izumi/CMS/MC/Introduction_kMC.pdf)
- [Monte Carlo and kinetic Monte Carlo methods – a tutorial by Peter Kratzer, University of Duisburg-Essen](https://arxiv.org/pdf/0904.2556.pdf)

#### Major Features
- Can construct cubic lattice simulation models
- Adjustable periodic boundary conditions in all three directions allow users to perform 1D, 2D, or 3D simulations
- Choose between several KMC algorithms (first reaction method, selective recalculation method, or full recalculation method)
- Can define custom rate constant calculation functions to include particle-particle interactions or particle-environment interactions
- Can utilize MPI to create pleasingly parallel simulations to quickly gather statistics about the simulation model
- Data analysis utility functions can be used to generate and analyze histograms for describing the behavior of the simulation model

## Current Status

The latest stable release is KMC_Lattice v2.0.1.
Please report any remaining bugs or submit feature requests for future releases in the [Issues](https://github.com/MikeHeiber/KMC_Lattice/issues) section. 

To keep track of upcoming changes or review prior changes to the codebase, please see the [Changelog](./CHANGELOG.md).

#### Continuous Integration and Testing Status:

KMC_Lattice is currently being tested on [Ubuntu](https://www.ubuntu.com/) v14.04 with the [GCC compiler](https://gcc.gnu.org/) (versions 4.7, 4.8, 4.9, 5, 6, 7, and 8) and on both [Open MPI](http://www.open-mpi.org/) v1.6.5 and [MPICH](http://www.mpich.org/) v3.04 using [Travis CI](https://travis-ci.com/).

| Branch | Status |
| :------: | ------ |
| Master | [![Build Status](https://img.shields.io/travis/MikeHeiber/KMC_Lattice/master.svg?style=for-the-badge)](https://travis-ci.org/MikeHeiber/KMC_Lattice) |
| Development | [![Build Status](https://img.shields.io/travis/MikeHeiber/KMC_Lattice/development.svg?style=for-the-badge)](https://travis-ci.org/MikeHeiber/KMC_Lattice) |

Code is being tested using [googletest](https://github.com/google/googletest) with test coverage assessment by [Coveralls](https://coveralls.io/).

| Branch | Status |
| :------: | ------ |
| Master | [![Coveralls github branch](https://img.shields.io/coveralls/github/MikeHeiber/KMC_Lattice/master.svg?style=for-the-badge)](https://coveralls.io/github/MikeHeiber/KMC_Lattice?branch=master) |
| Development | [![Coveralls github branch](https://img.shields.io/coveralls/github/MikeHeiber/KMC_Lattice/development.svg?style=for-the-badge)](https://coveralls.io/github/MikeHeiber/KMC_Lattice?branch=development) |

## Contributing

If you would like to contribute to the development of this project, please see the [contributing instructions](./CONTRIBUTING.md).
If you would like some help in using or customizing the tool for your research, please contact me (heiber@mailaps.org) to discuss a collaboration. 
You can check out my research using this tool and other work on [Researchgate](https://www.researchgate.net/profile/Michael_Heiber).

## Using KMC_Lattice

#### Package Contents

Event class - This base class can be extended to represent any process/mechanism/transition that one would like to simulate. 
It could represent a hopping motion event, a reaction event, etc. depending on the application. 
Typically, derived events are associated with a particular derived object. 
The Event class contains the fundamental properties and back-end operations that any given state transition would require.

Lattice class - This class implements a lattice, its boundary conditions, and keeps track of its occupancy.

Object class - This base class can be extended to represent any entity that one would like to simulate. 
It could represent an electron, atom, molecule, organism, etc. depending on the application. 
The Object class contains the fundamental properties and back-end operations that any given entity simulation would require.

Parameters_Lattice class - This class contains all of the parameters needed to construct a Lattice object.

Parameters_Simulation class - This class contains all of the parameters needed to construct a Simulation object.

Simulation class - This base class can be extended to manage all derived objects and their associated events. 
The Simulation class contains the fundamental properties and back-end operations that most simulations would require.

Site class - This base class can be extended to represent the lattice sites that make up the simulation medium/environment. 
Added site properties can be used to implement interactions between the simulation environment and the objects, which then affect the events. 
For example, site energies can be assigned to derived site classes to account for inhomogeneous systems.

Utils - This file contains a number of useful utility functions, scientific constants, etc. that can then be used throughout the software package.

Version class - This class stores a valid software version string and has version comparison operators and can be used to compare different versions of things to check for compatibility.

#### API Documentation

Detailed API documentation for these classes and the entire KMC_Lattice package is generated using Doxygen and can be viewed [here](https://mikeheiber.github.io/KMC_Lattice/).

#### Building and Testing the KMC_Lattice Library

This software tool uses [Message Passing Interface (MPI)](https://computing.llnl.gov/tutorials/mpi/) to utilize parallel computing power. 
As a result, using KMC_Lattice requires that an MPI library is pre-installed on your system, and the final KMC_Lattice library must be built on your specific system. 
Contact your HPC admin to determine the protocols for building MPI applications on your specific HPC system. 
In many cases, the HPC system and environment will already be configured for you, and the package comes with a default makefile that can be used with the [GCC compiler](https://gcc.gnu.org/) or the [PGI compiler](https://www.pgroup.com/). 

If you wish, you can also install MPI on your own personal workstation and then build the KMC_Lattice library there as well. For development and preliminary simulation tests, sometimes it is more efficient to run on your own workstation instead of an HPC system. More information about common MPI packages can be found here:
- Open MPI, http://www.open-mpi.org/
- MPICH, http://www.mpich.org/
- MVAPICH, http://mvapich.cse.ohio-state.edu/

Once you have an MPI library installed, to build KMC_Lattice, first copy the KMC_Lattice directory to your machine.  On Linux this can be done using the command,

```git clone --recurse-submodules https://github.com/MikeHeiber/KMC_Lattice```

Then set KMC_Lattice as your working directory,

```cd KMC_Lattice```

and finally build the software package with the default makefile.

```make```

In the default makefile, compilation flags have been set for the GCC and PGI compilers.  If you are using another compiler, you will need to edit the makefile and define your own compiler options.
Once the normal build is successful, you should test KMC_Lattice on your own hardware using the unit and system tests provided before you use the tool. 
Build the testing executable by running

```make test```

Once the test build is complete, run the test executables.

```./test/KMC_Lattice_tests.exe```

and

```mpiexeec -n 4 ./test/KMC_Lattice_MPI_tests.exe```

Please report any build or testing errors in the [Issues](https://github.com/MikeHeiber/KMC_Lattice/issues) section. 

## Citing this Work

If you find KMC_Lattice to be helpful for your research, please cite the forthcoming publication...

## Acknowledgments

Development of v2.0.0 was supported by financial assistance award 70NANB14H012 from U.S. Department of Commerce, National Institute of Standards and Technology as part of the Center for Hierarchical Materials Design (CHiMaD).
