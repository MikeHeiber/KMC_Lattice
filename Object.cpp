// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#include "Object.h"

const string Object::name_base = "Object";

Object::~Object(){
    //dtor
}

Object::Object(){
    time_created = 0;
    tag = 0;
    coords.setXYZ(0,0,0);
    coords_initial.setXYZ(0,0,0);
    dx = 0;
    dy = 0;
    dz = 0;
}

Object::Object(const double time,const int tag_num,const Coords& start_coords){
    time_created = time;
    tag = tag_num;
    coords = start_coords;
    coords_initial = start_coords;
    dx = 0;
    dy = 0;
    dz = 0;
}

double Object::calculateDisplacement(){
    return sqrt((double)(coords.x+dx-coords_initial.x)*(coords.x+dx-coords_initial.x)+(coords.y+dy-coords_initial.y)*(coords.y+dy-coords_initial.y)+(coords.z+dz-coords_initial.z)*(coords.z+dz-coords_initial.z));
}

Coords Object::getCoords() const{
    return coords;
}

double Object::getCreationTime() const{
    return time_created;
}

int Object::getDX() const{
    return dx;
}

int Object::getDY() const{
    return dy;
}

int Object::getDZ() const{
    return dz;
}

list<Event*>::iterator Object::getEventIt() const{
    return event_it;
}

string Object::getName() const{
    return name_base;
}

int Object::getTag() const{
    return tag;
}

void Object::incrementDX(const int num){
    dx += num;
}

void Object::incrementDY(const int num){
    dy += num;
}

void Object::incrementDZ(const int num){
    dz += num;
}

void Object::setCoords(const Coords& new_coords){
    coords = new_coords;
}

void Object::setEventIt(const list<Event*>::iterator it){
    event_it = it;
}

