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
        int getTag();
        void setCoords(const Coords& new_coords);
        void setEventIt(const list<unique_ptr<Event>>::iterator it);
    private:
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
