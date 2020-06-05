# distutils: language = c++

#from libcpp.string cimport string
#from libcpp.map cimport map as mapcpp
#from libcpp.vector cimport vector
#from cython.operator import dereference, postincrement

from pyData cimport Mesh

cdef class pyMesh:
	
	cdef Mesh * thismesh

	def __cinit__(self, int size, str inname, str threatCat):

		#cdef string n = inname.encode('utf-8')
		#cdef string tc = threatCat.encode('utf-8')
		self.thismesh = new Mesh(size, inname.encode('utf-8'), threatCat.encode('utf-8'))

	def __dealloc__(self):
		del self.thismesh

	def setValue(self, int index, float theVal):
		self.thismesh.setValue(index, theVal)
		
	def getValue(self, int index):
		if index < 0 or index >= self.thismesh.getSize():
			raise IndexError("Index {0} out of range".format(index))
		else:
			return self.thismesh.getValue(index)

	def getSize(self):
		return self.thismesh.getSize()

	def setName(self, str newName):
		return self.thismesh.setName(newName.encode('utf-8'))

	def getName(self):
		nm = self.thismesh.getName()
		return nm.decode('utf-8')
		
	def isNull(self):
		return self.thismesh.isNull()

	def linkNeighs(self, int indexA, int indexB):
		self.thismesh.linkNeighs(indexA, indexB)

	def setThreatStatus(self, str newStatus):
		self.thismesh.setThreatStatus(newStatus.encode('utf-8'))

	def getThreatStatus(self):
		st = self.thismesh.getThreatStatus()
		return st.decode('utf-8')

	def newThreatSubcriteriaA(self, int subcri):
		self.thismesh.newThreatSubcriteriaA(subcri)

	def getThreatSubcriteriaA(self):
		return self.thismesh.getThreatSubcriteriaA()

	def randomize(self):
		self.thismesh.randomize()


cdef class pySolution:
	
	cdef SolutionB * thissol

	def __cinit__(self, pyMesh mother):

		self.thissol = new SolutionB(mother.thismesh)

	def __dealloc__(self):
		del self.thissol

	@property
	def critA(self):
		return self.thissol.critA

	@critA.setter
	def critA(self, newCritA):
		self.thissol.critA = newCritA


	@property
	def critB(self):
		return self.thissol.critB

	@critB.setter
	def critB(self, newCritB):
		self.thissol.critB = newCritB


	@property
	def score(self):
		return self.thissol.score

	@score.setter
	def score(self, newScore):
		self.thissol.score = newScore


	@property
	def spp2crit(self):
		"""
		out = {}
		cdef mapcpp[int,vector[int]] mymap = self.thissol.spp2crit
		cdef mapcpp[int,vector[int]].iterator it = mymap.begin()
		while(it != mymap.end()):
			out[dereference(it).first] = dereference(it).second
			postincrement(it)
		return out
		"""
		return self.thissol.spp2crit
#	map<int, vector<int>> spp2crit;

	def setValue(self, int index, float theVal):
		self.thissol.setValue(index, theVal)
		
	def getValue(self, int index):
		return self.thissol.getValue(index)

	def getSize(self):
		return self.thissol.getSize()

	def isNull(self):
		return self.thissol.isNull()

	def randomize(self):
		self.thissol.randomize()




def evalFit(pySolution mySol, list myObs, double overPenalty, bool updateAll):

	cdef vector[Mesh*] ve

	for ob in myObs:
		obme = <pyMesh> ob
		ve.push_back(obme.thismesh)


	fitness(mySol.thissol, ve, overPenalty, updateAll)