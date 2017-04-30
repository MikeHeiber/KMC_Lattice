// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef OBJECT_H
#define OBJECT_H

#include "Utils.h"
#include "Event.h"
#include <list>
#include <memory>

using namespace std;

class Event;

class Object{
    public:
        virtual ~Object();
        Object();
        Object(const double time,const int tag_num,const Coords& start_coords);
        double calculateDisplacement();
        Coords getCoords();
        double getCreationTime();
        list<unique_ptr<Event>>::iterator getEventIt();
        virtual string getName();
        int getTag();
        void incrementDX(const int num);
        void incrementDY(const int num);
        void incrementDZ(const int num);
        void setCoords(const Coords& new_coords);
        void setEventIt(const list<unique_ptr<Event>>::iterator it);
    private:
        static const string name_base;
        double time_created;
        int tag;
        // Current coords
        Coords coords;
        // Initial coords
        Coords coords_initial;
        list<unique_ptr<Event>>::iterator event_it;
        // Accounts for passes across periodic boundaries
        int dx;
        int dy;
        int dz;
};

#endif // OBJECT_H
