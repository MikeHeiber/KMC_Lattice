# include "Site.h"

Site::~Site(){

}

Site::Site(){
    occupied = false;
}

bool Site::isOccupied(){
    return occupied;
}
