# KMC_Lattice

## General Information
This object-oriented C++ software package contains a general framework for lattice kinetic Monte Carlo (KMC) simulations. 
This framework consists of a number of utility functions and base classes that must be extended to create a fully operational KMC simulation. 
The goal of this package is to be robust, reliable, and flexible so that users can easily develop KMC simulations for a wide variety of different scientific problems without the need to start from scratch.

This KMC_Lattice package implements several event recalculation options for creating computationally efficient simulations. 
The package is designed to be usable on a personal computer and on high performance computing clusters. 
Check out the [Excimontec](https://github.com/MikeHeiber/Excimontec) software package for an example of a complex tool constructed using KMC_Lattice, which is used for simulating organic semiconductor devices.

For further reading about kinetic Monte Carlo simulations, a nice overview of the theory and algorithms can be found here:

[Introduction to the Kinetic Monte Carlo Method by Arthur Voter, Los Alamos National Lab](http://www.fml.t.u-tokyo.ac.jp/~izumi/CMS/MC/Introduction_kMC.pdf)

## Current Status

The current release is KMC_Lattice [![GitHub (pre-)release](https://img.shields.io/github/release/MikeHeiber/KMC_Lattice/all.svg?style=flat-square)](https://github.com/MikeHeiber/KMC_Lattice/releases).
Most major planned features that are to be included in v2.0 are now implemented and have undergone preliminary testing. 
However, this software tool is still under development, and as such, there may still be bugs that need to be squashed. 
Please report any bugs or submit feature requests in the [Issues](https://github.com/MikeHeiber/KMC_Lattice/issues) section. 

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

## Contact

If you would like to contribute to the development of this project or would like some help in building an efficient KMC simulation tool for your specific scientific problem, please contact me (heiber@mailaps.org) to discuss a collaboration. 
You can check out my KMC research and other work on [Researchgate](https://www.researchgate.net/profile/Michael_Heiber).

## How to try KMC_Lattice?

#### Building and Testing the KMC_Lattice Library

This software tool uses [Message Passing Interface (MPI)](https://computing.llnl.gov/tutorials/mpi/) to utilize parallel computing power. 
As a result, using KMC_Lattice requires that an MPI library is pre-installed on your system, and the final KMC_Lattice library must be built on your specific system. 
Contact your HPC admin to determine the protocols for building MPI applications on your specific HPC system. 
In many cases, the HPC system and environment will already be configured for you, and the package comes with a default makefile that can be used with the [GCC compiler](https://gcc.gnu.org/) or the [PGI compiler](https://www.pgroup.com/). 

If you wish, you can also install MPI on your own personal workstation and then build the KMC_Lattice library there as well. For development and preliminary simulation tests, sometimes it is more efficient to run on your own workstation instead of an HPC system. More information about common MPI packages can be found here:
- Open MPI, http://www.open-mpi.org/
- MPICH, http://www.mpich.org/
- MVAPICH, http://mvapich.cse.ohio-state.edu/

Once you have an MPI library installed, to build the KMC_Lattice library, copy the KMC_Lattice directory to your machine, set it as your working directory, and run `make`. 
Compilation flags have been set for the GCC and PGI compilers.  If you are using another compiler, you may need to edit the makefile and define your own compiler options.
However, before you use the KMC_Lattice library, you should test it on your own hardware using the unit and system tests provided. 
Build the testing executable by running `make test`. 
Once the test build is complete, run `./test/KMC_Lattice_tests.exe`.
Please report any build or testing errors in the [Issues](https://github.com/MikeHeiber/KMC_Lattice/issues) section. 

## Package Contents

Object class - This base class can be extended to represent any entity that one would like to simulate. 
It could represent an electron, atom, molecule, organism, etc. depending on the application. 
The Object class contains the fundamental properties and back-end operations that any given entity simulation would require.

Lattice class - This class implements a lattice, its boundary conditions, and keeps track of its occupancy.

Event class - This base class can be extended to represent any process/mechanism/transition that one would like to simulate. 
It could represent a hopping motion event, a reaction event, etc. depending on the application. 
Typically, derived events are associated with a particular derived object. 
The Event class contains the fundamental properties and back-end operations that any given state transition would require.

Site class - This base class can be extended to represent the lattice sites that make up the simulation medium/environment. 
Added site properties can be used to implement interactions between the simulation environment and the objects, which then affect the events. 
For example, site energies can be assigned to derived site classes to account for inhomogeneous systems.

Simulation class - This base class can be extended to manage all derived objects and their associated events. 
The Simulation class contains the fundamental properties and back-end operations that most simulations would require.

Utils - This contains a number of useful utility functions, scientific constants, etc. that can then be used throughout the software package.

Detailed API documentation for these classes and the entire KMC_Lattice package can be viewed [here](https://mikeheiber.github.io/KMC_Lattice/).
