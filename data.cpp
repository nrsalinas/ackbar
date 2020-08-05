#include "data.hpp"

Mesh::Mesh(){
}

Mesh::Mesh(int size, string inname, string threatCat){

	values.resize(size);
	neighborhood.resize(size);
	for (int i = 0; i < size; i++) {
		neighborhood[i].resize(size);
		for (int j = 0; j < size; j++) {
			neighborhood[i][j] = 0;
		}
	}
	name = inname;
	threatStatus = threatCat;
}

Mesh::~Mesh(){
}

void Mesh::setValue(int index, double value){
	values[index] = value;
}

double Mesh::getValue(int index){
	return values[index];
}

/*
void Mesh::resetNeighborhood(){ // Not used in search.cpp
	neighborhood.resize(values.size());
	for (int i = 0; i < values.size(); i++){
		neighborhood[i].resize(0);
	}
}
*/

void Mesh::linkNeighs(int indexA, int indexB){ 
	// indexes in vector values
	neighborhood[indexA][indexB] = 1;
	neighborhood[indexB][indexA] = 1;

	/*
	bool AinB = false;
	bool BinA = false;

	for (int a = 0; a < neighborhood[indexA].size(); a++){
		if (neighborhood[indexA][a] == indexB){
			BinA = true;
			break;
		}
	} 

	for (int b = 0; b < neighborhood[indexB].size(); b++){
		if (neighborhood[indexB][b] == indexA){
			AinB = true;
			break;
		}
	} 

	if (BinA == false) {
		neighborhood[indexA].push_back(indexB);
	}

	if (AinB == false) {
		neighborhood[indexB].push_back(indexA);
	}
	*/
}

vector< vector<int> > Mesh::getNeighborhood(){ //not used in search.cpp
	vector< vector<int> > out;
	out = neighborhood;
	return out;
}

vector<int> Mesh::getCellNeighs(int index){
	vector <int> out;
	for (int i = 0; i < neighborhood[index].size(); i++) {
		if (neighborhood[index][i] > 0) {
			out.push_back(i);
		}
	}
	return out;
}

void Mesh::neighsFromList(vector<vector<int> > origNeighs){  //not used in search.cpp
	neighborhood.resize(origNeighs.size());
	for (int i = 0; i < origNeighs.size(); i++){
		neighborhood[i].resize(origNeighs[i].size());
		for (int j = 0; j < origNeighs[i].size(); j++){
			neighborhood[i][j] = origNeighs[i][j];
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
	for (int i = 0; i < threatSubcriteriaA.size(); i++){
		out.push_back(threatSubcriteriaA[i]);
	}
	return out;
}

bool Mesh::isNull(){
	bool out = true;
	for (int i = 0; i < values.size(); i++){
		if (values[i] > 0){
			out = false;
			break;
		}
	}
	return out;
}

void Mesh::nullMe () {
	for (int i = 0; i < values.size(); i++){
		values[i] = 0;
	}
}

void Mesh::randomize(){
	for (int i = 0; i < values.size(); i++){
		values[i] = (double) (rand() % 2) ;
	}
}


Mesh * Mesh::copy(){
	Mesh * thisCopy = new Mesh(this->values.size());
	//thisCopy->values.resize( values.size() );
	for (int i = 0; i < values.size(); i++){
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