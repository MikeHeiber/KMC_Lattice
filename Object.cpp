#include "Object.h"

Object::~Object(){
    //dtor
}

Object::Object(){
    time_created = 0;
    tag = 0;
    coords.x = 0;
    coords.y = 0;
    coords.z = 0;
    coords_initial.x = 0;
    coords_initial.y = 0;
    coords_initial.z = 0;
    dx = 0;
    dy = 0;
    dz = 0;
}

double Object::calculateDisplacement(){
    return sqrt((double)(coords.x+dx-coords_initial.x)*(coords.x+dx-coords_initial.x)+(coords.y+dy-coords_initial.y)*(coords.y+dy-coords_initial.y)+(coords.z+dz-coords_initial.z)*(coords.z+dz-coords_initial.z));
}

Coords Object::getCoords(){
    return coords;
}

double Object::getCreationTime(){
    return time_created;
}

list<unique_ptr<Event>>::iterator Object::getEventIt(){
    return event_it;
}

int Object::getTag(){
    return tag;
}

void Object::setEventIt(const list<unique_ptr<Event>>::iterator it){
    event_it = it;
}

