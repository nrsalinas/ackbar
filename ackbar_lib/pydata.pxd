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
		void setRange(double newRange)
		double getRange()

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
		int critA, critB, score, extent, origin
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

	vector[vesol] metaAlt(vector[Mesh*] &observations, cppmap[int, vector[int]] &taxGroups, cppmap[int, int] &spp2groups, double clusterEps, int iters, int outSize, double ndmWeight)

