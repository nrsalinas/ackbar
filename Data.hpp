#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;
using std::cout;
using std::endl;


class Cell {

	public:

		Cell(float invalue = -1, int ingeometry = 0, float inxCentroid = 0.0, float inyCentroid = 0.0);
	    ~Cell();
		void setValue(float theVal);
		float getValue();

		void newNeigh(Cell * theNei);
		Cell * getNeigh(int index);
		int neighSize();

		float storedCorners();
		void addCorner(float lon, float lat);
		float getCorner(int index);


	private:

	    float value;
	    unsigned neighs; // number of neighbors
	    int geometry; // 0 = square, 1 = hexagonal, 2 = triangular
	    float xCentroid;
	    float yCentroid;
	    Cell * neiPoints[6]; // array of neighbors' pointers
	    vector<float> corners; // corner coordinates. For each corner, first figure given is longitude, then latitude.

  	};


class Mesh {

	public:

		Mesh(int size, string inname = "NN", float mean0 = 0.0, float mean1 = 1.0, float stadev0 = 0.13, float stadev1 = 0.13, string threatCat = "LC");
		~Mesh();

		void newCell(Cell * theCell);
		void setName(string newName);
		int getSize();
		string getName();
		bool isNull();
		void prettyPrint();

		void setValue(int index, float value);
		float getValue(int index);

		void resetNeighborhood();
		void linkNeighs(int indexA, int indexB);
		vector<vector<int>> getNeighborhood();
		vector<int> getCellNeighs(int index);
		void neighsFromList(vector<vector<int>> origNeighs);

		void setThreatStatus(string newStatus);
		string getThreatStatus();
		void newThreatSubcriteriaA(int subcri);
		vector<int> getThreatSubcriteriaA();
		
		void setMean(int indx, float value);
		void setStadev(int indx, float value);
		float getMean(int indx);
		float getStadev(int indx);
		Mesh * copy();
		void randomize();

	private:

	    int geometry; // 0 = square, 1 = hexagonal, 2 = triangular
	    string name;
	    vector<vector<int>> neighborhood;
	    string threatStatus; // 'CR', 'EN', or 'VU'
	    vector<int> threatSubcriteriaA; // 1, 2, 3, or 4
		vector<Cell*> cellColl;
		float means[2];
		float stadevs[2];

	};


class SolutionB : public Mesh {

	public:

	SolutionB(int size);
	SolutionB(Mesh * mother);
	~SolutionB();
	int critA;
	int critB;
	int score;
	map<int, vector<int>> spp2crit;
	/* keys are indexes in observations vector. Values are vectors integers that 
	associates to KBA criteria. Code options are:
		0 = A1a
		1 = A1b
		2 = Aic
		3 = A1d
		4 = A1e
		5 = B1
		6 = B2*/ 

	};


struct Solution {
	Mesh * solMesh;
	double overlayScore;
	int critA = 0;
	int critB = 0;
	int score = 0;
	map<int, vector<int>> spp2crit;
	/* keys are indexes in observations vector. Values are vectors integers that 
	associates to KBA criteria. Code options are:
		0 = A1a
		1 = A1b
		2 = Aic
		3 = A1d
		4 = A1e
		5 = B1
		6 = B2*/ 
	};

