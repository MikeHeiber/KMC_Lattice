# include "Site.h"

Site::~Site(){

}

Site::Site(){
    occupied = false;
}

void Site::clearOccupancy(){
    occupied = false;
}

bool Site::isOccupied(){
    return occupied;
}

void Site::setOccupied(){
    occupied = true;
}
