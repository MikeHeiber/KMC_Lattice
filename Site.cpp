// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

# include "Site.h"

Site::~Site(){

}

Site::Site(){
    occupied = false;
}

void Site::clearOccupancy(){
    occupied = false;
}

list<Object*>::iterator Site::getObjectIt() const{
    return object_it;
}

bool Site::isOccupied(){
    return occupied;
}

void Site::setObjectIt(list<Object*>::iterator it){
    object_it = it;
}

void Site::setOccupied(){
    occupied = true;
}
