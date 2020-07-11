from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp.map cimport map as cppmap
from libcpp cimport bool


cdef extern from "data.cpp":
	pass

cdef extern from "data.hpp":

	cdef cppclass Mesh:

		#public

		Mesh() except +
		Mesh(int size, string inname, string threatCat) except +

		void setValue(int index, double value)
		double getValue(int index)
		void setName(string newName)
		int getSize()
		string getName()
		bool isNull()
		void nullMe()

		#void resetNeighborhood()
		void linkNeighs(int indexA, int indexB)
		#vector<vector<int>> getNeighborhood()
		#vector<int> getCellNeighs(int index)
		#void neighsFromList(vector<vector<int>> origNeighs)

		void setThreatStatus(string newStatus)
		string getThreatStatus()
		void newThreatSubcriteriaA(int subcri)
		vector[int] getThreatSubcriteriaA()
		
		void randomize()


	cdef cppclass Solution:

		Solution() except +
		Solution(int size) except +
		Solution(Mesh * mother) except +
		int critA, critB, score, extent
		double ndmScore, aggrScore
		void setValue(int index, double value)
		double getValue(int index)
		int getSize()
		bool isNull()
		void nullMe()
		void randomize()
		cppmap[int, vector[int]] spp2crit


ctypedef vector[Solution*] vesol


cdef extern from "search.cpp":
	pass


cdef extern from "search.hpp":

	void fitness(Solution * rsearchSol, vector [Mesh*] &observations, double outerFactor, double absFactor, double ndmWeight, bool updateAll)

	void solExpansion(Solution* solita, vector [Mesh*] &observations, cppmap[int,int] &exclMap, int cellIndx, double prescore, double ndmOutFactor, double ndmAbsFactor, double ndmWeight)

	vector[Solution*] meta(vector[Mesh*] &observations, double clusterEps, int iters, int outSize, double ndmOutFactor, double ndmAbsFactor, double ndmWeight)

	vector[vesol] metaAlt(vector[Mesh*] &observations, double clusterEps, int iters, int outSize, double ndmWeight)

	int islandNumber(Solution * solita)

	bool isContinuous (Solution* solita)

