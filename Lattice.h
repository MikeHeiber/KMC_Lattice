// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software package.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef LATTICE_H
#define LATTICE_H

#include "Utils.h"
#include "Site.h"
#include <functional>
#include <stdexcept>

//! \brief This struct contains all of the main input parameters needed by the Lattice class.
//! \copyright MIT License.  For more information, see the LICENSE file that accompanies this software package.
//! \author Michael C. Heiber
//! \date 2017
struct Parameters_Lattice{
	//! Determines whether the x-direction periodic boundaries will be enabled. 
	bool Enable_periodic_x;
	//! Determines whether the y-direction periodic boundaries will be enabled. 
	bool Enable_periodic_y;
	//! Determines whether the z-direction periodic boundaries will be enabled. 
	bool Enable_periodic_z;
	//! Defines the desired x-direction size of the lattice.
	int Length;
	//! Defines the desired y-direction size of the lattice.
	int Width;
	//! Defines the desired z-direction size of the lattice.
	int Height;
	//! Defines the desired lattice unit size, which is used to convert lattice units into real space units.
	double Unit_size; // nm
};

//! \brief This class contains the properties of a three-dimensional lattice and the functions needed to interact with it.
//! \details The class makes use of the Parameters_Lattice struct to load the neccessary input parameters, the Coords struct
//! to record the Cartesian coordinates of each lattice site, and the Site class to assign properties to each site.
//! \copyright MIT License.  For more information, see the LICENSE file that accompanies this software package.
//! \author Michael C. Heiber
//! \date 2017
class Lattice{
    public:
		//! \brief Default constructor that creates an empty Lattice object.
		//! \warning An empty lattice object should not be used until initialized using the init function.
		Lattice();

		//! \brief Initializes the Lattice object using the provided Parameters_Lattice input parameter struct.
		//! \param params is a Parameters_Lattice struct that contains all of the required
		//! parameters to initialize the Lattice object. 
		//! \param generator_ptr is a pointer to a Mersenne twister number generator.
        void init(const Parameters_Lattice& params, std::mt19937* generator_ptr);

		//! \brief Calculates the destination coordinates when given the starting coordinates and the displacement vector (i,j,k).
		//! \details When the starting coordinates are near one or more of the lattice boundaries and periodic boundary conditions are enabled,
		//! the function detemines the destination coordinates across the periodic boundary and assigns the calculated Coords struct to the input
		//! coords_dest argument.
		//! \param coords_initial is the Coords struct tht designates the starting coordinates.
		//! \param i is the displacement in the x-direction.
		//! \param j is the displacement in the y-direction.
		//! \param k is the displacement in the z-direction.
		//! \param coords_dest is Coords struct that indicates the output destination coordinates.
		void calculateDestinationCoords(const Coords& coords_initial, const int i, const int j, const int k, Coords& coords_dest) const;

		//! \brief Calculates a coordinate adjustment factor if the x-direction periodic boundary is crossed.
		//! \param x is the starting x coordinate.
		//! \param i is the displacement in the x-direction.
		//! \return Length (the x-direction size of the lattice) if the x periodic boundary is crossed in the negative direction.
		//! \return -Length if the x periodic boundary is crossed in the positive direction.
		//! \return 0 if the x periodic boundary is not enabled or if the x periodic boundary is not crossed.
		int calculateDX(const int x, const int i) const;

		//! \brief Calculates a coordinate adjustment factor if the x-direction periodic boundary is crossed.
		//! \param coords_initial is the Coords struct that represents the starting coordinates.
		//! \param coords_dest is the Coords struct that represents the destination coordinates.
		//! \return Length (the x-direction size of the lattice) if the x periodic boundary is crossed in the negative direction.
		//! \return -Length if the x periodic boundary is crossed in the positive direction.
		//! \return 0 if the x periodic boundary is not enabled or if the x periodic boundary is not crossed.
		int calculateDX(const Coords& coords_initial, const Coords& coords_dest) const;

		//! \brief Calculates a coordinate adjustment factor if the y-direction periodic boundary is crossed.
		//! \param y is the starting y coordinate.
		//! \param j is the displacement in the y-direction.
		//! \return Width (the y-direction size of the lattice) if the y periodic boundary is crossed in the negative direction.
		//! \return -Width if the y periodic boundary is crossed in the positive direction.
		//! \return 0 if the y periodic boundary is not enabled or if the y periodic boundary is not crossed.
		int calculateDY(const int y, const int j) const;

		//! \brief Calculates a coordinate adjustment factor if the y-direction periodic boundary is crossed.
		//! \param coords_initial is the Coords struct that represents the starting coordinates.
		//! \param coords_dest is the Coords struct that represents the destination coordinates.
		//! \return Width (the y-direction size of the lattice) if the y periodic boundary is crossed in the negative direction.
		//! \return -Width if the y periodic boundary is crossed in the positive direction.
		//! \return 0 if the y periodic boundary is not enabled or if the y periodic boundary is not crossed.
		int calculateDY(const Coords& coords_initial, const Coords& coords_dest) const;

		//! \brief Calculates a coordinate adjustment factor if the z-direction periodic boundary is crossed.
		//! \param z is the starting z coordinate.
		//! \param k is the displacement in the z-direction.
		//! \return Height (the z-direction size of the lattice) if the z periodic boundary is crossed in the negative direction.
		//! \return -Height if the z periodic boundary is crossed in the positive direction.
		//! \return 0 if the z periodic boundary is not enabled or if the z periodic boundary is not crossed.
		int calculateDZ(const int z, const int k) const;

		//! \brief Calculates a coordinate adjustment factor if the z-direction periodic boundary is crossed.
		//! \param coords_initial is the Coords struct that represents the starting coordinates.
		//! \param coords_dest is the Coords struct that represents the destination coordinates.
		//! \return Height (the z-direction size of the lattice) if the z periodic boundary is crossed in the negative direction.
		//! \return -Height if the z periodic boundary is crossed in the positive direction.
		//! \return 0 if the z periodic boundary is not enabled or if the z periodic boundary is not crossed.
		int calculateDZ(const Coords& coords_initial, const Coords& coords_dest) const;

		//! \brief Calculates the shortest distance between a pair of coordinates in squared lattice units.
		//! \param coords_start is the Coords struct that represents the starting coordinates.
		//! \param coords_dest is the Coords struct that represents the destination coordinates.
		//! \return The distance between the two sets of coordinates in squared lattice units.
		int calculateLatticeDistanceSquared(const Coords& coords_start, const Coords& coords_dest) const;

		//! \brief Checks to see if a generic move operation from the designated initial coordinates to a destination
		//! position specified by the displacement vector (i,j,k) is possible.
		//! \details The main use of this function is used to check if a proposed move event crosses a non-periodic boundary.
		//! \param coords_initial is the Coords struct that represents the starting coordinates.
		//! \param i is the displacement in the x-direction.
		//! \param j is the displacement in the y-direction.
		//! \param k is the displacement in the z-direction.
		//! \return true if a move event is possible.
		//! \return false if a move event is not possible.
		bool checkMoveValidity(const Coords& coords_initial, const int i, const int j, const int k) const;

		//! \brief Clears the occupancy of the site located at the specified coordinates.
		//! \param coords is the Coords struct that represents the coordinates of the site to be cleared.
		void clearOccupancy(const Coords& coords);

		//! \brief Generates the coordinates for a randomly selected site in the lattice.
		//! \return A Coords struct containing the coordinates of a randomly selected site from the lattice.
		Coords generateRandomCoords();

		//! \brief Generates a random x coordinate that lies within the x-dimension size of the lattice.
		//! \return
		//! A randomly selected x coordinate value from in the range from to 0 to Length-1.
		int generateRandomX();

		//! \brief Generates a random y coordinate that lies within the y-dimension size of the lattice.
		//! \return
		//! A randomly selected y coordinate value in the range from 0 to Width-1.
		int generateRandomY();

		//! \brief Generates a random z coordinate that lies within the z-dimension size of the lattice.
		//! \return
		//! A randomly selected z coordinate value in the range from 0 to Height-1.
		int generateRandomZ();

		//! \brief Gets the z-direction size of the lattice, the height.
		//! \return The Height property of the lattice, which is the z-direction size.
		int getHeight() const;

		//! \brief Gets the x-direction size of the lattice, the length.
		//! \return The Length property of the lattice, which is the x-direction size.
		int getLength() const;

		//! \brief Gets the number of sites contained in the lattice.
		//! \return The number of sites in the lattice.
		long int getNumSites() const;

		//! \brief Gets the coordinates of the specified site.
		//! \param site_index is the vector index of the input site
		//! \return a Coords object that contains the coordinates of the site specified by the site index.
		Coords getSiteCoords(long int site_index);

		//! \brief Gets the vector index for the site corresponding to the input coordinates.
		//! \param coords is the Coords struct that represents the input coordinates.
		//! \return The vector index for the sites vector that is associated with the site located at the input coordinates.
		//! \return -1 if the coordinates are not located in the lattice.
		long int getSiteIndex(const Coords& coords) const;

		//! \brief Gets the vector iterator for the site corresponding to the input coordinates.
		//! \param coords is the Coords struct that represents the input coordinates.
		//! \return The vector iterator for the sites vector that is associated with the site located at the input coordinates.
		std::vector<Site*>::iterator getSiteIt(const Coords& coords);

		//! \brief Gets the lattice unit size, which is used to convert lattice units into real space units.
		//! \return The unit size property of the lattice.
		double getUnitSize() const;

		//! \brief Gets the y-direction size of the lattice, the width.
		//! \return The Width property of the lattice, which is the y-direction size.
		int getWidth() const;

		//! \brief Checks whether the site located at the input coordinates is occupied or not.
		//! \param coords is a Coords struct that represents the input coordinates.
		//! \return true if the specificed site is occupied
		//! \return false if the specified site is unoccupied
		bool isOccupied(const Coords& coords) const;

		//! \brief Checks whether the x-direction periodic boundaries are enabled or not.
		//! \return true if periodic boundaries are enabled in the x-direction.
		//! \return false if periodic boundaries are disabled in the x-direction.
		bool isXPeriodic() const;

		//! \brief Checks whether the y-direction periodic boundaries are enabled or not.
		//! \return true if periodic boundaries are enabled in the y-direction.
		//! \return false if periodic boundaries are disabled in the y-direction.
		bool isYPeriodic() const;

		//! \brief Checks whether the z-direction periodic boundaries are enabled or not.
		//! \return true if periodic boundaries are enabled in the z-direction.
		//! \return false if periodic boundaries are disabled in the z-direction.
		bool isZPeriodic() const;

		//! Prints to the command line which sites are occupied.
		void outputLatticeOccupancy() const;

		//! \brief Sets the site located at the input coordinates to the occupied state.
		//! \param coords is the Coords struct that represents the input coordinates.
		void setOccupied(const Coords& coords);

		//! \brief Sets the member site pointer vector to the input site pointer vector
		//! \param input_ptrs is the input site pointer vector
		//! \return false to indicate an error when the sizes of the input site pointer vector and the member site pointer vector are not equal.
		//! \return true when no error occurs.
		bool setSitePointers(const std::vector<Site*>& input_ptrs);
    protected:

    private:
		bool Enable_periodic_x;
		bool Enable_periodic_y;
		bool Enable_periodic_z;
		int Length; // nm
		int Width; // nm
		int Height; // nm
		double Unit_size; // nm
		std::vector<Site*> site_ptrs;
		std::mt19937* gen_ptr;
};

#endif // LATTICE_H
