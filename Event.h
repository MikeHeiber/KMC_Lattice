#ifndef EVENT_H
#define EVENT_H

#include "Utils.h"
#include "Object.h"
#include <list>
#include <memory>

using namespace std;

class Object;

class Event{
    public:
        virtual ~Event();
        Event();
        virtual void calculateEvent();
        virtual bool executeEvent();
        double getWaitTime();
        list<unique_ptr<Object>>::iterator getObjectIt();
        list<unique_ptr<Object>>::iterator getObjectTargetIt();
        void seedGenerator(const int id);
        void setDestCoords(const Coords& coords);
        void setWaitTime(const double time);
        void setObjectIt(const list<unique_ptr<Object>>::iterator it);
    protected:
        // Variables
        static constexpr double K_b = 8.6173325e-5;
        // Functions
        double rand01();
    private:
        // Variables and objects
        double wait_time;
        //double e_delta;
        list<unique_ptr<Object>>::iterator object_it;
        list<unique_ptr<Object>>::iterator object_target_it;
        Coords coords_dest;
        static boost::mt19937 gen;
};

#endif // EVENT_H
