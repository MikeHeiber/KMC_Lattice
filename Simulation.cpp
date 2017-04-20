#include "Simulation.h"

Simulation::~Simulation(){
    //dtor
}

void Simulation::addObject(unique_ptr<Object>& object_ptr){
    // Add an event for the object to the event list and link the event to the object
    events.push_back(unique_ptr<Event>(nullptr));
    object_ptr->setEventIt(--events.end());
    // Add new object to the object vector and link the object to the event
    objects.push_back(move(object_ptr));
    // Update counters
    N_objects++;
    N_objects_created++;
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

list<unique_ptr<Event>>::iterator Simulation::chooseNextEvent(){
    list<unique_ptr<Event>>::iterator event_it;
    list<unique_ptr<Event>>::iterator event_it_target = events.begin();
    for(event_it=++events.begin();event_it!=events.end();++event_it){
        if((*event_it)->getWaitTime() < (*event_it_target)->getWaitTime()){
            event_it_target = event_it;
        }
    }
    return event_it_target;
}

vector<list<unique_ptr<Object>>::iterator> Simulation::findRecalcNeighbors(const Coords& coords){
    vector<list<unique_ptr<Object>>::iterator> object_its;
    object_its.reserve(10);
    list<unique_ptr<Object>>::iterator object_it;
    Coords coords2;
    int dx,dy,dz;
    const static int recalc_cutoff_sq_lat = (Recalc_cutoff/Unit_size)*(Recalc_cutoff/Unit_size);
    int distance_sq_lat;
    for(object_it=objects.begin();object_it!=objects.end();++object_it){
        coords2 = (*object_it)->getCoords();
        if(Enable_periodic_x && abs(coords2.x-coords.x)>Length/2){
            dx = -Length;
        }
        else{
            dx = 0;
        }
        if(Enable_periodic_y && abs(coords2.y-coords.y)>Width/2){
            dy = -Width;
        }
        else{
            dy = 0;
        }
        if(Enable_periodic_z && abs(coords2.z-coords.z)>Height/2){
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

int Simulation::getId(){
    return Id;
}

int Simulation::getNumSites(){
    return Length*Width*Height;
}

int Simulation::getN_events_executed(){
    return N_events_executed;
}

int Simulation::getSiteIndex(const Coords& coords){
    return coords.x*Width*Height+coords.y*Height+coords.z;
}

vector<unique_ptr<Site>>::iterator Simulation::getSiteIt(const Coords& coords){
    return lattice.begin()+getSiteIndex(coords);
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

void Simulation::initializeSimulation(const Parameters_Simulation& params,const int id){
    Id = id;
    Time = 0;
    N_events_executed = 0;
    N_objects_created = 0;
    N_objects = 0;
    // Clear all remaining data structures
    events.clear();
    objects.clear();
    gen.seed(time(0)*id);
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
    Enable_recalc = params.Enable_recalc;
    Recalc_cutoff = params.Recalc_cutoff;
    // Output files
    Logfile = params.Logfile;
}

void Simulation::removeObjectItDuplicates(vector<list<unique_ptr<Object>>::iterator>& object_its){
    vector<list<unique_ptr<Object>>::iterator>::iterator it1;
    vector<list<unique_ptr<Object>>::iterator>::iterator it2;
    for(it1=object_its.begin();it1!=object_its.end(); ++it1){
        for(it2=it1+1;it2!=object_its.end(); ++it2){
            if((it2!=it1) && (*it2==*it1)){
                it2 = object_its.erase(it2);
                it2--;
            }
        }
    }
}

void Simulation::setEvent(const list<unique_ptr<Event>>::iterator event_it,unique_ptr<Event> event_ptr){
    // Update events list
    event_it->release();
    *event_it = move(event_ptr);
}

