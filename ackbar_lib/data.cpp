/*#############################################################################
#
#	Copyright 2020 Nelson R. Salinas
#
#
#	This file is part of Ackbar.
#
#   Ackbar is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
# 	(at your option) any later version.
#
#	Ackbar is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with Ackbar. If not, see <http://www.gnu.org/licenses/>.
#
##############################################################################*/

#include "data.hpp"

Mesh::Mesh(){
}

Mesh::Mesh(int size, string inname, string threatCat){

	values.resize(size);
	neighborhood.resize(size);
	for (int i = 0; i < size; i++) {
		neighborhood.at(i).resize(size);
		for (int j = 0; j < size; j++) {
			neighborhood.at(i).at(j) = 0;
		}
	}
	name = inname;
	threatStatus = threatCat;
}

Mesh::~Mesh(){
}

void Mesh::setValue(int index, double value){
	values.at(index) = value;
}

double Mesh::getValue(int index){
	return values.at(index);
}

void Mesh::linkNeighs(int indexA, int indexB){ 
	// indexes in vector values
	neighborhood.at(indexA).at(indexB) = 1;
	neighborhood.at(indexB).at(indexA) = 1;

}

vector< vector<int> > Mesh::getNeighborhood(){ //not used in search.cpp
	vector< vector<int> > out;
	out = neighborhood;
	return out;
}

vector<int> Mesh::getCellNeighs(int index){
	vector <int> out;
	for (uint i = 0; i < neighborhood.at(index).size(); i++) {
		if (neighborhood.at(index).at(i) > 0) {
			out.push_back(i);
		}
	}
	return out;
}

void Mesh::neighsFromList(vector<vector<int> > origNeighs){  //not used in search.cpp
	neighborhood.resize(origNeighs.size());
	for (uint i = 0; i < origNeighs.size(); i++){
		neighborhood.at(i).resize(origNeighs.at(i).size());
		for (uint j = 0; j < origNeighs.at(i).size(); j++){
			neighborhood.at(i).at(j) = origNeighs.at(i).at(j);
		}
	}	
}

void Mesh::setName(string newName){
	name = newName;
}

string Mesh::getName(){
	return name;
}

int Mesh::getSize(){
	return values.size();
}

void Mesh::setRange(double newRange){
	range = newRange;
}

double Mesh::getRange(){
	return range;
}


void Mesh::setThreatStatus(string newStatus){
	threatStatus = newStatus;
}

string Mesh::getThreatStatus(){
	return threatStatus;
}

void Mesh::newThreatSubcriteriaA(int subcri){
	threatSubcriteriaA.push_back(subcri);
}

vector<int> Mesh::getThreatSubcriteriaA(){
	vector<int> out;
	for (uint i = 0; i < threatSubcriteriaA.size(); i++){
		out.push_back(threatSubcriteriaA[i]);
	}
	return out;
}

bool Mesh::isNull(){
	bool out = true;
	for (uint i = 0; i < values.size(); i++){
		if (values[i] > 0){
			out = false;
			break;
		}
	}
	return out;
}

void Mesh::nullMe () {
	for (uint i = 0; i < values.size(); i++){
		values[i] = 0;
	}
}

void Mesh::randomize(){
	for (uint i = 0; i < values.size(); i++){
		values[i] = (double) (rand() % 2) ;
	}
}


Mesh * Mesh::copy(){
	Mesh * thisCopy = new Mesh(this->values.size());
	//thisCopy->values.resize( values.size() );
	for (uint i = 0; i < values.size(); i++){
		thisCopy->values[i] = values[i];
	}
	thisCopy->neighsFromList( this->getNeighborhood() );

	return thisCopy;
}


/*****************************************************/

Solution::Solution(): Mesh(){
}


Solution::Solution(int size): Mesh(size){
	critA = 0;
	critB = 0;
	score = 0;
	ndmScore = 0.0;
	extent = 0;
	origin = 0;
}

Solution::Solution(Mesh * mother): Mesh(mother->getSize()) {
	critA = 0;
	critB = 0;
	score = 0;
	ndmScore = 0.0;
	aggrScore = 0.0;
	extent = 0;
	origin = 0;
	this->neighsFromList(mother->getNeighborhood());

	for (int i = 0; i < mother->getSize(); i++){
		this->setValue(i, mother->getValue(i));
	}
	
}

Solution::~Solution(){
}
