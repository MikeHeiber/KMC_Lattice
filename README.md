# KMC_Lattice

## General Information
This object-oriented C++ software package contains a general framework for lattice kinetic Monte Carlo (KMC) simulations. 
This framework consists of a number of utility functions and base classes that must be extended to create a fully operational KMC simulation. 
The goal of this package is to be robust and flexible so that users can easily develop KMC simulations for a wide variety of different scientific problems without the need to start from scratch.

This KMC_Lattice package implements several event recalculation options for creating computationally efficient simulations. 
The package is designed to be usable on a personal computer and on high performance computing clusters. 
A simple example implementation of this general KMC framework can be found in the [KMC_Lattice_example](https://github.com/MikeHeiber/KMC_Lattice_example) package. 
To see a more complete implementation for simulating a complex system, check out the [Excimontec](https://github.com/MikeHeiber/Excimontec) software package that is used for simulating organic semiconductor materials and devices.

For further reading about kinetic Monte Carlo simulations, a nice overview of the theory and algorithm can be found here:

[Introduction to the Kinetic Monte Carlo Method by Arthur Voter, Los Alamos National Lab](http://www.fml.t.u-tokyo.ac.jp/~izumi/CMS/MC/Introduction_kMC.pdf)

## Work Together

If you would like to contribute to the development of this project or would like some help in building an efficient KMC simulation tool for your specific scientific problem, please contact me to discuss a collaboration. 
You can check out my KMC research and other work on [Researchgate](https://www.researchgate.net/profile/Michael_Heiber).

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

Utils - This file contain a number of useful utility functions, scientific constants, etc. that can then be used throughout the software package.

Detailed API documentation for these classes and the entire KMC_Lattice package can be viewed [here](https://mikeheiber.github.io/KMC_Lattice/).
