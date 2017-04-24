// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#ifndef EVENT_H
#define EVENT_H

#include "Utils.h"
#include "Object.h"
#include <boost/random.hpp>
#include <list>
#include <memory>

using namespace std;

class Object;

class Event{
    public:
        static boost::mt19937 gen;
        virtual ~Event();
        Event();
        virtual void calculateEvent(const Coords& dest_coords,const double distance,const double E_delta,const int temperature, const double prefactor) = 0;
        Coords getDestCoords();
        virtual string getName();
        list<unique_ptr<Object>>::iterator getObjectIt();
        list<unique_ptr<Object>>::iterator getObjectTargetIt();
        float getWaitTime();
        static void seedGenerator(const int id);
        void setDestCoords(const Coords& coords);
        void setWaitTime(const float time);
        void setObjectIt(const list<unique_ptr<Object>>::iterator it);
    protected:
        // Variables
        static constexpr double K_b = 8.6173325e-5;
        // Functions
        double rand01();
    private:
        // Variables and objects
        static const string name_base;
        float wait_time;
        list<unique_ptr<Object>>::iterator object_it;
        list<unique_ptr<Object>>::iterator object_target_it;
        Coords coords_dest;
};

#endif // EVENT_H
