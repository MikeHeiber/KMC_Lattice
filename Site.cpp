// Copyright (c) 2017 Michael C. Heiber
// This source file is part of the KMC_Lattice project, which is subject to the MIT License.
// For more information, see the LICENSE file that accompanies this software.
// The KMC_Lattice project can be found on Github at https://github.com/MikeHeiber/KMC_Lattice

# include "Site.h"

using namespace std;

Site::~Site(){

}

Site::Site(){

}

void Site::clearOccupancy(){
    occupied = false;
	object_ptr = nullptr;
}

Object* Site::getObjectPtr() const{
    return object_ptr;
}

bool Site::isOccupied() const{
    return occupied;
}

void Site::setObjectPtr(Object* input_ptr){
    object_ptr = input_ptr;
	occupied = true;
}

void Site::setOccupied(){
    occupied = true;
}
