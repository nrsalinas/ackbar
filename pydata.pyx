# distutils: language = c++

from pydata cimport Mesh

cdef class Meshpy:
	
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

	def nullMe(self):
		self.thisMesh.nullMe()

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


cdef class Solutionpy:
	
	cdef Solution * thissol

	def __cinit__(self, Meshpy mother):

		self.thissol = new Solution(mother.thismesh)

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
	def ndmScore(self):
		return self.thissol.ndmScore

	@property
	def aggrScore(self):
		return self.thissol.aggrScore

	@property
	def extent(self):
		return self.thissol.extent

	@property
	def spp2crit(self):
		return self.thissol.spp2crit

	def setValue(self, int index, float theVal):
		self.thissol.setValue(index, theVal)
		
	def getValue(self, int index):
		return self.thissol.getValue(index)

	def getSize(self):
		return self.thissol.getSize()

	def isNull(self):
		return self.thissol.isNull()

	def nullMe(self):
		self.thissol.nullMe()

	def randomize(self):
		self.thissol.randomize()

	def toBitList(self):
		out = []
		for c in range(self.getSize()):
			if self.getValue(c) > 0:
				out.append(1)
			elif self.getValue(c) == 0:
				out.append(0)
			else:
				raise ValueError("Solution object has negative values.")
		return out


def evalFit(Solutionpy mySol, list myObs, double outerFactor, double absFactor, double ndmWeight, bool updateAll):

	cdef vector[Mesh*] ve

	for ob in myObs:
		obme = <Meshpy> ob
		ve.push_back(obme.thismesh)


	fitness(mySol.thissol, ve, outerFactor, absFactor, ndmWeight, updateAll)


def solExp(Solutionpy mySol, list myObs, dict exMap, int cellIndx, double prescore, double ndmOutFactor, double ndmAbsFactor, double ndmWeight):

	cdef vector[Mesh*] ve
	#cdef cppmap[int,int] emap

	for ob in myObs:
		obme = <Meshpy> ob
		ve.push_back(obme.thismesh)
	
	solExpansion(mySol.thissol, ve, exMap, cellIndx, prescore, ndmOutFactor, ndmAbsFactor, ndmWeight)

	

def metasearch(list obs, double eps, int iters, int maxOutSize, double ndmOutFactor, double ndmAbsFactor, double ndmWeight):

	pout = []
	cdef vector[Mesh*] ve
	cdef vector[Solution*] out

	for ob in obs:
		obme = <Meshpy> ob
		ve.push_back(obme.thismesh)

	out = meta(ve, eps, iters, maxOutSize, ndmOutFactor, ndmAbsFactor, ndmWeight)

	for i in range(out.size()):

		psol = Solutionpy(obs[0])
		del psol.thissol
		psol.thissol = out[i]
		pout.append(psol)

	return pout


def metasearchAlt(list obs, double eps, int iters, int maxOutSize, double ndmWeight):

	pout = []
	cdef vector[Mesh*] ve
	cdef vector[Solution*] out

	for ob in obs:
		obme = <Meshpy> ob
		ve.push_back(obme.thismesh)

	out = metaAlt(ve, eps, iters, maxOutSize, ndmWeight)

	for i in range(out.size()):

		psol = Solutionpy(obs[0])
		del psol.thissol
		psol.thissol = out[i]
		pout.append(psol)

	return pout


def islNum(pysolita):
	solita = <Solutionpy> pysolita
	out = islandNumber(solita.thissol)
	return out


def isCont(pysolita):
	solita = <Solutionpy> pysolita
	out = isContinuous(solita.thissol)
	return out