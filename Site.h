// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef SITE_H
#define SITE_H

using namespace std;

class Site{
    public:
        virtual ~Site();
        Site();
        void clearOccupancy();
        bool isOccupied();
        void setOccupied();
    private:
        bool occupied;
};

#endif // SITE_H
