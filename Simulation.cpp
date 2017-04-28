// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

#include "Simulation.h"

Simulation::~Simulation(){
    //dtor
}

list<unique_ptr<Event>>::iterator Simulation::addEvent(unique_ptr<Event>& event_ptr){
    events.push_back(move(event_ptr));
    return --events.end();
}

list<unique_ptr<Object>>::iterator Simulation::addObject(unique_ptr<Object>& object_ptr){
    // Add an event for the object to the event list and link the event to the object
    events.push_back(unique_ptr<Event>(nullptr));
    object_ptr->setEventIt(--events.end());
    // Add new object to the object vector and link the object to the event
    objects.push_back(move(object_ptr));
    // Update counters
    N_objects++;
    N_objects_created++;
    N_events_executed++;
    return --objects.end();
}

void Simulation::addSite(unique_ptr<Site>& site_ptr){
    lattice.push_back(move(site_ptr));
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

list<unique_ptr<Event>>::iterator Simulation::chooseNextEvent(){
    list<unique_ptr<Event>>::iterator event_it_target = events.begin();
    for(auto event_it=++events.begin();event_it!=events.end();++event_it){
        if((*event_it)->getWaitTime() < (*event_it_target)->getWaitTime()){
            event_it_target = event_it;
        }
    }
    return event_it_target;
}

vector<list<unique_ptr<Object>>::iterator> Simulation::findRecalcNeighbors(const Coords& coords){
    vector<list<unique_ptr<Object>>::iterator> object_its;
    object_its.reserve(10);
    Coords coords2;
    int dx,dy,dz;
    const static int recalc_cutoff_sq_lat = (Recalc_cutoff/Unit_size)*(Recalc_cutoff/Unit_size);
    int distance_sq_lat;
    for(auto object_it=objects.begin();object_it!=objects.end();++object_it){
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
    static boost::uniform_int<> distx(0,Length-1);
    static boost::variate_generator<boost::mt19937&, boost::uniform_int<> > randx(gen, distx);
    static boost::uniform_int<> disty(0,Width-1);
    static boost::variate_generator<boost::mt19937&, boost::uniform_int<> > randy(gen, disty);
    static boost::uniform_int<> distz(0,Height-1);
    static boost::variate_generator<boost::mt19937&, boost::uniform_int<> > randz(gen, distz);
    coords.x = randx();
    coords.y = randy();
    coords.z = randz();
    return coords;
}

int Simulation::getSiteIndex(const Coords& coords){
    return coords.x*Width*Height+coords.y*Height+coords.z;
}

vector<unique_ptr<Site>>::iterator Simulation::getSiteIt(const Coords& coords){
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
    gen.seed(time(0)*(id+1));
    Event::seedGenerator(id);
    // Output files
    Logfile = params.Logfile;
}

void Simulation::incrementTime(const float added_time){
    Time += added_time;
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

void Simulation::logMSG(const ostringstream& msg){
    *Logfile << msg.str();
    Logfile->flush();
}

void Simulation::moveObject(const list<unique_ptr<Object>>::iterator object_it,const Coords& dest_coords){
    // clear occupancy of initial site
    (*getSiteIt((*object_it)->getCoords()))->clearOccupancy();
    // check for periodic boundary crossing
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
    // set object coords to new site
    (*object_it)->setCoords(dest_coords);
    // set occupancy of new site
    (*getSiteIt(dest_coords))->setOccupied();
    // update counter
    N_events_executed++;
}

void Simulation::outputLatticeOccupancy(){
    for(auto site_it=lattice.begin();site_it!=lattice.end();++site_it){
        if((*site_it)->isOccupied()){
            cout << "Site " << distance(lattice.begin(),site_it) << " is occupied." << endl;
        }
    }
}

void Simulation::removeObject(const list<unique_ptr<Object>>::iterator object_it){
    // clear occupancy of site
    (*getSiteIt((*object_it)->getCoords()))->clearOccupancy();
    // release and then delete event pointer
    ((*object_it)->getEventIt())->release();
    events.erase((*object_it)->getEventIt());
    // remove object pointer
    objects.erase(object_it);
    // Update counters
    N_objects--;
    N_events_executed++;
}

void Simulation::removeObjectItDuplicates(vector<list<unique_ptr<Object>>::iterator>& object_its){
    for(auto it1=object_its.begin();it1!=object_its.end(); ++it1){
        for(auto it2=it1+1;it2!=object_its.end(); ++it2){
            if((it2!=it1) && (*it2==*it1)){
                it2 = object_its.erase(it2);
                it2--;
            }
        }
    }
}

void Simulation::setEvent(const list<unique_ptr<Event>>::iterator event_it,unique_ptr<Event>& event_ptr){
    // Update events list
    // Clear current pointer
    event_it->release();
    // Assign new pointer
    *event_it = move(event_ptr);
}

