from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp.map cimport map as cppmap
from libcpp cimport bool

cdef extern from "Data4py.cpp":
	pass

cdef extern from "Data4py.hpp":

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

		#void resetNeighborhood()
		void linkNeighs(int indexA, int indexB)
		#vector<vector<int>> getNeighborhood()
		#vector<int> getCellNeighs(int index)
		#void neighsFromList(vector<vector<int>> origNeighs)

		void setThreatStatus(string newStatus)
		string getThreatStatus()
		void newThreatSubcriteriaA(int subcri)
		vector[int] getThreatSubcriteriaA()
		
		void setMean(int indx, double value)
		void setStadev(int indx, double value)
		double getMean(int indx)
		double getStadev(int indx)
		void randomize()


	cdef cppclass SolutionB:

		SolutionB() except +
		SolutionB(int size) except +
		SolutionB(Mesh * mother) except +
		int critA, critB, score, extent
		double ndmScore
		void setValue(int index, double value)
		double getValue(int index)
		int getSize()
		bool isNull()
		void randomize()
		cppmap[int, vector[int]] spp2crit

cdef extern from "search4py.cpp":
	pass

cdef extern from "search4py.hpp":

	void fitness(SolutionB * rsearchSol, vector [Mesh*] &observations, double overPenalty, bool updateAll)
