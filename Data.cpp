#include "Data.hpp"

Cell::Cell(float invalue, int ingeometry, float inxCentroid, float inyCentroid){
	value = invalue;
	neighs = 0;
	geometry = ingeometry;
	xCentroid = inxCentroid;
	yCentroid = inyCentroid;
	}

Cell::~Cell(){
	}

void Cell::setValue(float theVal){
	value = theVal;
	}

float Cell::getValue(){
	return value;
	}

void Cell::newNeigh(Cell * theNei){
	int nextIndex = neighs;
	neighs += 1;
	neiPoints[nextIndex] = theNei;
	}

int Cell::neighSize(){
	return neighs;
	}

Cell * Cell::getNeigh(int index){
	return neiPoints[index];
	}

float Cell::storedCorners(){
	float out = (float) corners.size() / 2.0;
	return out;
	}

void Cell::addCorner(float lon, float lat){
	corners.push_back(lon);
	corners.push_back(lat);
	}

float Cell::getCorner(int index){
	return corners[index];
	}

/*****************************************************/

Mesh::Mesh(int size, string inname, float mean0, float mean1, float stadev0, 
	float stadev1, string threatCat){

	cellColl.resize(size);
	for (int i = 0; i < size; i++){
		cellColl[i] = new Cell(0.0);
		}

	neighborhood.resize(size);
	
	name = inname;
	
	means[0] = mean0;
	means[1] = mean1;
	stadevs[0] = stadev0;
	stadevs[1] = stadev1;
	
	threatStatus = threatCat;
	}

Mesh::~Mesh(){
	for (int i = 0; i < cellColl.size(); i++){
		delete cellColl[i];
		}
	}

void Mesh::newCell(Cell * theCell){
	cellColl.push_back(theCell);
	}

void Mesh::setValue(int index, float value){
	cellColl[index]->setValue(value);
	}

float Mesh::getValue(int index){
	return cellColl[index]->getValue();
	}

void Mesh::prettyPrint(){
	for (int i = 0; i < cellColl.size(); i++){
		if (cellColl[i]->getValue() > 0){
			cout << 1;
			} else {
				cout << 0;
				}
		}
	cout << endl;
	}

void Mesh::resetNeighborhood(){
	neighborhood.resize(cellColl.size());
	for (int i = 0; i < cellColl.size(); i++){
		neighborhood[i].resize(0);
		}
	}

void Mesh::linkNeighs(int indexA, int indexB){ // indexes in vector cellColl
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

	}

vector<vector<int>> Mesh::getNeighborhood(){
	vector<vector<int>> out;
	out = neighborhood;
	return out;
	}

vector<int> Mesh::getCellNeighs(int index){
	vector <int> out;
	out = neighborhood[index];
	return out;
	}

void Mesh::neighsFromList(vector<vector<int>> origNeighs){
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
	return cellColl.size();
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
	for (int i = 0; i < cellColl.size(); i++){
		if (cellColl[i]->getValue() > 0){
			out = false;
			break;
			}
		}
	return out;
	}

void Mesh::randomize(){
	for (int i = 0; i < cellColl.size(); i++){
		cellColl[i]->setValue( (float) (rand() % 2) );
		}
	}

void Mesh::setMean(int indx, float value){
	means[indx] = value;
	}
	
void Mesh::setStadev(int indx, float value){
	stadevs[indx] = value;
	}

float Mesh::getMean(int indx){
	return means[indx];
	}
	
float Mesh::getStadev(int indx){
	return stadevs[indx];
	}

Mesh * Mesh::copy(){
	Mesh * thisCopy = new Mesh(this->cellColl.size());
	thisCopy->cellColl.resize( cellColl.size() );
	for (int i = 0; i < cellColl.size(); i++){
		thisCopy->cellColl[i] = new Cell;
		thisCopy->cellColl[i]->setValue(cellColl[i]->getValue());
		}
	thisCopy->neighsFromList( this->getNeighborhood() );

	return thisCopy;
	}


/*****************************************************/

SolutionB::SolutionB(int size): Mesh(size){
	critA = 0;
	critB = 0;
	score = 0;
	}

SolutionB::SolutionB(Mesh * mother): Mesh(mother->getSize()) {
	critA = 0;
	critB = 0;
	score = 0;
	this->neighsFromList(mother->getNeighborhood());

	for (int i = 0; i < mother->getSize(); i++){
		this->setValue(i, mother->getValue(i));
		}
	
	}

SolutionB::~SolutionB(){
	}