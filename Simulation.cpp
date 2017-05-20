// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#include "Simulation.h"

Simulation::~Simulation(){

}

list<Event*>::iterator Simulation::addEvent(Event* event_ptr){
    events.push_back(event_ptr);
    return --events.end();
}

list<Object*>::iterator Simulation::addObject(Object* object_ptr){
    // Add an event for the object to the event list and link the event to the object
    events.push_back(nullptr);
    object_ptr->setEventIt(--events.end());
    // Add new object to the object vector and link the object to the event
    objects.push_back(object_ptr);
    // Set occupancy of site
    (*getSiteIt(object_ptr->getCoords()))->setOccupied();
    (*getSiteIt(object_ptr->getCoords()))->setObjectIt(--objects.end());
    // Update counters
    N_objects++;
    N_objects_created++;
    N_events_executed++;
    return --objects.end();
}

void Simulation::addSite(Site* site_ptr){
    lattice.push_back(site_ptr);
}

Coords Simulation::calculateDestinationCoords(const Coords& coords_initial,const int i,const int j,const int k){
    Coords dest_coords;
    dest_coords.x = coords_initial.x+i+calculateDX(coords_initial.x,i);
    dest_coords.y = coords_initial.y+j+calculateDY(coords_initial.y,j);
    dest_coords.z = coords_initial.z+k+calculateDZ(coords_initial.z,k);
    return dest_coords;
}

//  This function calculates the coordinate adjustment term needed to account for periodic boundaries in the x-direction.
int Simulation::calculateDX(const int x,const int i){
    if(!Enable_periodic_x){
        return 0;
    }
    else{
        if(x+i<0){
            return Length;
        }
        else if(x+i>=Length){
            return -Length;
        }
        else{
            return 0;
        }
    }
}

//  This function calculates the coordinate adjustment term needed to account for periodic boundaries in the y-direction.
int Simulation::calculateDY(const int y,const int j){
    if(!Enable_periodic_y){
        return 0;
    }
    else{
        if(y+j<0){
            return Width;
        }
        else if(y+j>=Width){
            return -Width;
        }
        else{
            return 0;
        }
    }
}

//  This function calculates the coordinate adjustment term needed to account for possible periodic boundaries in the z-direction.
int Simulation::calculateDZ(const int z,const int k){
    if(!Enable_periodic_z){
        return 0;
    }
    else{
        if(z+k<0){ // Check for periodic z boundary
            return Height;
        }
        else if(z+k>=Height){
            return -Height;
        }
        else{
            return 0;
        }
    }
}

int Simulation::calculateLatticeDistanceSquared(const Coords& coords_start,const Object& object_target){
    int absx = abs(object_target.getCoords().x-coords_start.x);
    int absy = abs(object_target.getCoords().y-coords_start.y);
    int absz = abs(object_target.getCoords().z-coords_start.z);
    int dx,dy,dz;
    if(Enable_periodic_x && 2*absx>Length){
        dx = -Length;
    }
    else{
        dx = 0;
    }
    if(Enable_periodic_y && 2*absy>Width){
        dy = -Width;
    }
    else{
        dy = 0;
    }
    if(Enable_periodic_z && 2*absz>Height){
        dz = -Height;
    }
    else{
        dz = 0;
    }
    return (absx+dx)*(absx+dx)+(absy+dy)*(absy+dy)+(absz+dz)*(absz+dz);
}

bool Simulation::checkMoveEventValidity(const Coords& coords_initial,const int i,const int j,const int k){
    if(i==0 && j==0 && k==0){
        return false;
    }
    if(!isXPeriodic() && (coords_initial.x+i>=getLength() || coords_initial.x+i<0)){
        return false;
    }
    if(!isYPeriodic() && (coords_initial.y+j>=getWidth() || coords_initial.y+j<0)){
        return false;
    }
    if(!isZPeriodic() && (coords_initial.z+k>=getHeight() || coords_initial.z+k<0)){
        return false;
    }
    return true;
}

list<Event*>::iterator Simulation::chooseNextEvent(){
    auto event_target_it = events.begin();
    if(events.size()>1){
        for(auto it=++events.begin();it!=events.end();++it){
            if((*it)!=nullptr && ((*event_target_it)==nullptr || ((*it)->getExecutionTime() < (*event_target_it)->getExecutionTime()))){
                event_target_it = it;
            }
        }
    }
    return event_target_it;
}

vector<list<Object*>::iterator> Simulation::findRecalcNeighbors(const Coords& coords){
    vector<list<Object*>::iterator> object_its;
    object_its.reserve(10);
    Coords coords2;
    int dx,dy,dz;
    const static int recalc_cutoff_sq_lat = (Recalc_cutoff/Unit_size)*(Recalc_cutoff/Unit_size);
    int distance_sq_lat;
    for(auto object_it=objects.begin();object_it!=objects.end();++object_it){
        coords2 = (*object_it)->getCoords();
        if(Enable_periodic_x && 2*abs(coords2.x-coords.x)>Length){
            dx = -Length;
        }
        else{
            dx = 0;
        }
        if(Enable_periodic_y && 2*abs(coords2.y-coords.y)>Width){
            dy = -Width;
        }
        else{
            dy = 0;
        }
        if(Enable_periodic_z && 2*abs(coords2.z-coords.z)>Height){
            dz = -Height;
        }
        else{
            dz = 0;
        }
        distance_sq_lat = (abs(coords2.x-coords.x)+dx)*(abs(coords2.x-coords.x)+dx)+(abs(coords2.y-coords.y)+dy)*(abs(coords2.y-coords.y)+dy)+(abs(coords2.z-coords.z)+dz)*(abs(coords2.z-coords.z)+dz);
        if(distance_sq_lat<=recalc_cutoff_sq_lat){
            object_its.push_back(object_it);
        }
    }
    return object_its;
}

vector<list<Object*>::iterator> Simulation::getAllObjectIts(){
    vector<list<Object*>::iterator> object_its;
    object_its.reserve(objects.size());
    for(auto object_it=objects.begin();object_it!=objects.end();++object_it){
        object_its.push_back(object_it);
    }
    return object_its;
}

int Simulation::getHeight(){
    return Height;
}

int Simulation::getId(){
    return Id;
}

int Simulation::getLength(){
    return Length;
}

int Simulation::getNumSites(){
    return Length*Width*Height;
}

int Simulation::getN_events_executed(){
    return N_events_executed;
}

Coords Simulation::getRandomCoords(){
    Coords coords;
    coords.x = getRandomX();
    coords.y = getRandomY();
    coords.z = getRandomZ();
    return coords;
}

int Simulation::getRandomX(){
    uniform_int_distribution<int> distx(0,Length-1);
    auto randx = bind(distx,ref(gen));
    return randx();
}

int Simulation::getRandomY(){
    uniform_int_distribution<int> disty(0,Width-1);
    auto randy = bind(disty,ref(gen));
    return randy();
}

int Simulation::getRandomZ(){
    uniform_int_distribution<int> distz(0,Height-1);
    auto randz = bind(distz,ref(gen));
    return randz();
}

int Simulation::getSiteIndex(const Coords& coords){
    return coords.x*Width*Height+coords.y*Height+coords.z;
}

vector<Site*>::iterator Simulation::getSiteIt(const Coords& coords){
    auto site_it = lattice.begin();
    advance(site_it,getSiteIndex(coords));
    return site_it;
}

int Simulation::getTemperature(){
    return Temperature;
}

double Simulation::getTime(){
    return Time;
}

double Simulation::getUnitSize(){
    return Unit_size;
}

int Simulation::getWidth(){
    return Width;
}

void Simulation::initializeSimulation(const Parameters_Simulation& params,const int id){
    Id = id;
    // General Parameters
    Enable_logging = params.Enable_logging;
    Enable_periodic_x = params.Enable_periodic_x;
    Enable_periodic_y = params.Enable_periodic_y;
    Enable_periodic_z = params.Enable_periodic_z;
    Length = params.Length;
    Width = params.Width;
    Height = params.Height;
    Unit_size = params.Unit_size;
    Temperature = params.Temperature;
    // First reaction method parameters
    Recalc_cutoff = params.Recalc_cutoff;
    // Initialize counters
    Time = 0;
    N_events_executed = 0;
    N_objects_created = 0;
    N_objects = 0;
    // Initialize data structures
    lattice.clear();
    objects.clear();
    events.clear();
    Event::seedGenerator(id);
    gen.seed(time(0)*(id+1));
    // Output files
    Logfile = params.Logfile;
}

bool Simulation::isOccupied(const Coords& coords){
    return (*getSiteIt(coords))->isOccupied();
}

bool Simulation::isXPeriodic(){
    return Enable_periodic_x;
}

bool Simulation::isYPeriodic(){
    return Enable_periodic_y;
}

bool Simulation::isZPeriodic(){
    return Enable_periodic_z;
}

bool Simulation::loggingEnabled(){
    return Enable_logging;
}

void Simulation::moveObject(const list<Object*>::iterator object_it,const Coords& dest_coords){
    // Clear occupancy of initial site
    (*getSiteIt((*object_it)->getCoords()))->clearOccupancy();
    // Check for periodic boundary crossing
    if(Enable_periodic_x){
        if(2*(dest_coords.x-(*object_it)->getCoords().x)<-Length){
            (*object_it)->incrementDX(Length);
        }
        else if(2*(dest_coords.x-(*object_it)->getCoords().x)>Length){
            (*object_it)->incrementDX(-Length);
        }
    }
    if(Enable_periodic_y){
        if(2*(dest_coords.y-(*object_it)->getCoords().y)<-Width){
            (*object_it)->incrementDY(Width);
        }
        else if(2*(dest_coords.y-(*object_it)->getCoords().y)>Width){
            (*object_it)->incrementDY(-Width);
        }
    }
    if(Enable_periodic_z){
        if(2*(dest_coords.z-(*object_it)->getCoords().z)<-Height){
            (*object_it)->incrementDZ(Height);
        }
        else if(2*(dest_coords.z-(*object_it)->getCoords().z)>Height){
            (*object_it)->incrementDZ(-Height);
        }
    }
    // Set object coords to new site
    (*object_it)->setCoords(dest_coords);
    // Set occupancy of new site
    (*getSiteIt(dest_coords))->setOccupied();
    (*getSiteIt(dest_coords))->setObjectIt(object_it);
    // Update counter
    N_events_executed++;
}

void Simulation::outputLatticeOccupancy(){
    for(auto site_it=lattice.begin();site_it!=lattice.end();++site_it){
        if((*site_it)->isOccupied()){
            cout << "Site " << distance(lattice.begin(),site_it) << " is occupied." << endl;
        }
    }
}

void Simulation::removeEvent(Event* event_ptr){
    events.remove(event_ptr);
}

void Simulation::removeObject(const list<Object*>::iterator object_it){
    // Clear occupancy of site
    (*getSiteIt((*object_it)->getCoords()))->clearOccupancy();
    // Delete the event pointer
    events.erase((*object_it)->getEventIt());
    // Delete the object pointer
    objects.erase(object_it);
    // Update counters
    N_objects--;
    N_events_executed++;
}

void Simulation::removeObjectItDuplicates(vector<list<Object*>::iterator>& object_its){
    for(auto it1=object_its.begin();it1!=object_its.end(); ++it1){
        for(auto it2=it1+1;it2!=object_its.end(); ++it2){
            if((it2!=it1) && (*it2==*it1)){
                it2 = object_its.erase(it2);
                it2--;
            }
        }
    }
}

void Simulation::setEvent(const list<Event*>::iterator event_it,Event* event_ptr){
    // Assign new pointer
    *event_it = event_ptr;
}

void Simulation::updateTime(const double new_time){
    Time = new_time;
}

