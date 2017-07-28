// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#include "Simulation.h"

Simulation::Simulation() {

}

Simulation::~Simulation(){

}

void Simulation::init(const Parameters_Simulation& params, const int id) {
	Id = id;
	// General Parameters
	Enable_logging = params.Enable_logging;
	// Lattice Parameters
	Parameters_Lattice params_lattice;
	params_lattice.Enable_periodic_x = params.Enable_periodic_x;
	params_lattice.Enable_periodic_y = params.Enable_periodic_y;
	params_lattice.Enable_periodic_z = params.Enable_periodic_z;
	params_lattice.Length = params.Length;
	params_lattice.Width = params.Width;
	params_lattice.Height = params.Height;
	params_lattice.Unit_size = params.Unit_size;
	Temperature = params.Temperature;
	// First reaction method parameters
	Recalc_cutoff = params.Recalc_cutoff;
	// Initialize data structures
	lattice.init(params_lattice, &gen);
	objects.clear();
	events.clear();
	gen.seed(time(0)*(id + 1));
	// Output files
	Logfile = params.Logfile;
}


list<Event*>::iterator Simulation::addEvent(Event* event_ptr){
    events.push_back(event_ptr);
    return --events.end();
}

void Simulation::addObject(Object* object_ptr){
    // Add an event for the object to the event list and link the event to the object
    events.push_back(nullptr);
    object_ptr->setEventIt(--events.end());
    // Add new object to the object vector and link the object to the event
    objects.push_back(object_ptr);
    // Set occupancy of site
    lattice.setOccupied(object_ptr->getCoords());
    (*lattice.getSiteIt(object_ptr->getCoords()))->setObjectPtr(*(--objects.end()));
    // Update counters
    N_objects++;
    N_objects_created++;
    N_events_executed++;
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

vector<Object*> Simulation::findRecalcNeighbors(const Coords& coords) const{
    vector<Object*> object_ptrs;
    object_ptrs.reserve(10);
    Coords coords2;
    int dx,dy,dz;
    const static int recalc_cutoff_sq_lat = (Recalc_cutoff/lattice.getUnitSize())*(Recalc_cutoff/lattice.getUnitSize());
    int distance_sq_lat;
    for(auto object_it=objects.begin();object_it!=objects.end();++object_it){
        coords2 = (*object_it)->getCoords();
        if(lattice.isXPeriodic() && 2*abs(coords2.x-coords.x)>lattice.getLength()){
            dx = -lattice.getLength();
        }
        else{
            dx = 0;
        }
        if(lattice.isYPeriodic() && 2*abs(coords2.y-coords.y)>lattice.getWidth()){
            dy = -lattice.getWidth();
        }
        else{
            dy = 0;
        }
        if(lattice.isZPeriodic() && 2*abs(coords2.z-coords.z)>lattice.getHeight()){
            dz = -lattice.getHeight();
        }
        else{
            dz = 0;
        }
        distance_sq_lat = (abs(coords2.x-coords.x)+dx)*(abs(coords2.x-coords.x)+dx)+(abs(coords2.y-coords.y)+dy)*(abs(coords2.y-coords.y)+dy)+(abs(coords2.z-coords.z)+dz)*(abs(coords2.z-coords.z)+dz);
        if(distance_sq_lat<=recalc_cutoff_sq_lat){
            object_ptrs.push_back(*object_it);
        }
    }
    return object_ptrs;
}

vector<Object*> Simulation::getAllObjectPtrs() const{
    vector<Object*> object_ptrs;
    object_ptrs.reserve(objects.size());
    for(auto object_it=objects.begin();object_it!=objects.end();++object_it){
        object_ptrs.push_back(*object_it);
    }
    return object_ptrs;
}

int Simulation::getId() const{
    return Id;
}

long int Simulation::getN_events_executed() const{
    return N_events_executed;
}

int Simulation::getTemp() const{
    return Temperature;
}

double Simulation::getTime() const{
    return Time;
}

bool Simulation::isLoggingEnabled() const{
    return Enable_logging;
}

void Simulation::moveObject(Object* object_ptr,const Coords& coords_dest){
	Coords coords_initial = object_ptr->getCoords();
    // Clear occupancy of initial site
	lattice.clearOccupancy(coords_initial);
    // Check for periodic boundary crossing
	object_ptr->incrementDX(-lattice.calculateDX(coords_initial,coords_dest));
	object_ptr->incrementDY(-lattice.calculateDY(coords_initial,coords_dest));
	object_ptr->incrementDZ(-lattice.calculateDZ(coords_initial,coords_dest));
    // Set object coords to new site
    object_ptr->setCoords(coords_dest);
    // Set occupancy of new site
	lattice.setOccupied(coords_dest);
    (*lattice.getSiteIt(coords_dest))->setObjectPtr(object_ptr);
    // Update counter
    N_events_executed++;
}

double Simulation::rand01() {
	return generate_canonical<double, std::numeric_limits<double>::digits>(gen);
}

void Simulation::removeEvent(Event* event_ptr){
    events.remove(event_ptr);
}

void Simulation::removeObject(Object* object_ptr){
    // Clear occupancy of site
	lattice.clearOccupancy(object_ptr->getCoords());
    // Delete the event pointer
    events.remove(*object_ptr->getEventIt());
    // Delete the object pointer
    objects.remove(object_ptr);
    // Update counters
    N_objects--;
    N_events_executed++;
}

void Simulation::setEvent(const list<Event*>::iterator event_it,Event* event_ptr){
    // Assign new pointer
    *event_it = event_ptr;
}

void Simulation::setGeneratorSeed(int seed) {
	gen.seed(seed);
}

void Simulation::updateTime(const double new_time){
    Time = new_time;
}

