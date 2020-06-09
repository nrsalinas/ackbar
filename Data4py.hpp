#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;

#ifndef DATA4PY_HPP
#define DATA4PY_HPP

class Cell {

	public:

		Cell(double invalue = -1, int ingeometry = 0, double inxCentroid = 0.0, double inyCentroid = 0.0);
	    ~Cell();
		void setValue(double theVal);
		double getValue();

		void newNeigh(Cell * theNei);
		Cell * getNeigh(int index);
		int neighSize();

		double storedCorners();
		void addCorner(double lon, double lat);
		double getCorner(int index);


	private:

	    double value;
	    unsigned neighs; // number of neighbors
	    int geometry; // 0 = square, 1 = hexagonal, 2 = triangular
	    double xCentroid;
	    double yCentroid;
	    Cell * neiPoints[6]; // array of neighbors' pointers
	    vector<double> corners; // corner coordinates. For each corner, first figure given is longitude, then latitude.

  	};


class Mesh {

	public:

		Mesh();
		Mesh(int size, string inname = "NN", string threatCat = "LC");
		~Mesh();

		void newCell(Cell * theCell);
		void setName(string newName);
		int getSize();
		string getName();
		bool isNull();

		void setValue(int index, double value);
		double getValue(int index);

		void resetNeighborhood();
		void linkNeighs(int indexA, int indexB);
		vector<vector<int>> getNeighborhood();
		vector<int> getCellNeighs(int index);
		void neighsFromList(vector<vector<int>> origNeighs);

		void setThreatStatus(string newStatus);
		string getThreatStatus();
		void newThreatSubcriteriaA(int subcri);
		vector<int> getThreatSubcriteriaA();
		
	
		Mesh * copy();
		void randomize();

	private:

	    int geometry; // 0 = square, 1 = hexagonal, 2 = triangular
	    string name;
	    vector<vector<int>> neighborhood;
	    string threatStatus; // 'CR', 'EN', or 'VU'
	    vector<int> threatSubcriteriaA; // 1, 2, 3, or 4
		vector<Cell*> cellColl;
		double means[2];
		double stadevs[2];

	};


class SolutionB : public Mesh {

	public:

	SolutionB();
	SolutionB(int size);
	SolutionB(Mesh * mother);
	~SolutionB();
	int critA;
	int critB;
	int score;
	double ndmScore;
	int extent;
	map<int, vector<int>> spp2crit;
	/* keys are indexes in observations vector. Values are vectors integers that 
	associates to KBA criteria. Code options are:
		0 = A1a
		1 = A1b
		2 = A1c
		3 = A1d
		4 = A1e
		5 = B1
		6 = B2*/ 

	};

#endif