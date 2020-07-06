import csv
import re
import pydata
from math import ceil, sin, cos, atan2, pi

class InputData(object):
	"""
	Input data processor class. Class constructor requires a csv file (str) with
	three columns: longitude, latitude, and taxon name.
	"""
	def __init__(self,infile):
		self.points = {} # values are population fractions
		self.iucn = {} # taxon to (Category, subcriteriaA)
		self.minLatitude = 91.0
		self.maxLatitude = -91.0
		self.minLongitude = 181.0
		self.maxLongitude = -181.0
		#self.cases = ((0,0,0), (0,-1,0), (0,-1,-1), (-1,-1,-1), (-1,0,-1), (-1,0,0)) # for 3- to 2-dimensional index conversion
		self.originN = None
		self.cellSize = None
		self.rows = None
		self.cols = None
		self.geometry = None
		self.csvfile = infile
		self.presence_grid = []
		self.index_reg = {}
		lineCounter = 0
		latCol = int()
		lonCol = int()

		with open(infile,'r') as fil:
			table = csv.reader(fil)
			for row in table:
				lineCounter += 1
				if lineCounter == 1 and (re.search("[^0-9\.\-]",row[1]) or re.search("[^0-9\.\-]",row[2])):
					if re.search("lon(gitude)*",row[1],flags=re.I) and re.search("lat(itude)*",row[2],flags=re.I):
						latCol, lonCol = 2, 1
						continue
					elif re.search("lon(gitude)*",row[2],flags=re.I) and re.search("lat(itude)*",row[1],flags=re.I):
						latCol, lonCol = 1, 2
						continue
					else:
						raise IOError("Input file `{0}`: column labels do not follow the required format (`Longitude`, `Latitude`).".format(infile))

				if len(row) > 3:
					raise IOError("Line {0} in `{1}` contains more than three columns.".format(lineCounter, infile))
				row[2] = re.sub("[\s\'\"]","",row[2])
				row[1] = re.sub("[\s\'\"]","",row[1])

				if len(row[latCol]) < 1:
					raise IOError("Line {0} in `{1}` do not contain latitude data.".format(lineCounter, infile))
				if re.search("[^0-9\.\-]",row[latCol]) or float(row[latCol]) < -90 or float(row[latCol]) > 90:
					raise IOError("Line {0} in `{1}` contains invalid coordinate value(s): `{2}`.".format(lineCounter, infile, row[latCol]))

				if len(row[lonCol]) < 1:
					raise IOError("Line {0} in `{1}` do not contain longitude data.".format(lineCounter, infile))
				if re.search("[^0-9\.\-]",row[lonCol]) or float(row[lonCol]) < -180 or float(row[lonCol]) > 180:
					raise IOError("Line {0} in `{1}` contains invalid coordinate value(s): `{2}`.".format(lineCounter, infile, row[lonCol]))

				lat = float(row[latCol])
				lon = float(row[lonCol])

				if len(row[0]) > 105:
					raise IOError("`{0}` exceeds the maximum taxon name size, 105 chars (line {0} in file `{1}`)".format(row[0],lineCounter, infile))

				if row[0] in self.points:
					self.points[row[0]][(lon,lat)] = 1
				else:
					self.points[row[0]] = { (lon,lat) : 1 }

				if self.minLatitude > lat:
					self.minLatitude = lat
				if self.maxLatitude < lat:
					self.maxLatitude = lat
				if self.minLongitude > lon:
					self.minLongitude = lon
				if self.maxLongitude < lon:
					self.maxLongitude = lon

		if len(self.points) < 3:
			raise ValueError("Input file only contain distribution data from {0} species (at least three are required).".format(len(self.points)))

		return None


	def iucnFile(self, filename):
		"""
		Process IUCN categories and subcriteria for criterion A from a csv file.
		"""
		lineCounter = 0
		nameCol = None
		categCol = None
		subcritACol = None
		validCats = ['CR', 'EN', 'VU', 'NT', 'LC', 'DD', 'NE']

		with open(filename,'r') as fil:
			table = csv.reader(fil)
			for row in table:
				lineCounter += 1
				if lineCounter == 1:
					
					for ic, cell in enumerate(row):
						if re.search("taxon",row[ic],flags=re.I):
							nameCol = ic
							continue
		
						if re.search("categor",row[ic],flags=re.I):
							categCol = ic
							continue

						if re.search("subcriter",row[ic],flags=re.I):
							subcritACol = ic
							continue
					#print(nameCol)
					#print(categCol)
					#print(subcritACol)
					if nameCol is None or categCol is None or subcritACol is None:
						raise IOError("Input file `{0}`: column labels do not follow the required format (`Taxon`, `Category`, `SubcriteriaA`).".format(filename))
				
				else:
					
					cat = row[categCol]

					isvalid = False
					if type(cat) == str:
						
						for vc in validCats:
							cat = re.sub(r'^\s+','',cat)
							cat = re.sub(r'\s+$','',cat)
							if vc == cat.upper():
								isvalid = True

					if not isvalid:
						raise IOError("{0} has a non valid IUCN category code (`{1}`)".format(row[nameCol], cat))

					if cat == '' or re.search(r'^\s+$', cat): 
						cat = 'NE'


					subcrA = []
					if type(row[subcritACol]) == str:
						for dig in row[subcritACol]:
							if dig == '1':
								subcrA.append(1)
							elif dig == '2':
								subcrA.append(2)
							elif dig == '4':
								subcrA.append(4)
							elif dig != '3':
								raise IOError("{0} has non valid subcriteria A (`{1}`)".format(row[nameCol], row[subcritACol]))

					self.iucn[row[nameCol]] = {'category': cat, 'subcritA': subcrA}



	def mergePoints(self, taxonName, minDist):
		"""
		Merge points using DBSCAN. Cluster scheme is store in values of points
		dictionary.
		"""
		clusters = self.dbscan(taxonName, minDist)

		totPops = len(clusters)
		for cl in clusters:
			factor = 1 / len(clusters[cl])
			for loc in clusters[cl]:
				self.points[taxonName][loc] = factor

		return totPops


	def dbscan(self, taxon, eps):
		"""
		DBSCAN-like algorithm to cluster points. There is not minimum cluster 
		size and, therefore, no noise list.

		- eps: maximum distance among cluster members.  
		"""
		minpts = 0
		clusters = {}
		visited = {x:0 for x in self.points[taxon]}
		for pivot in self.points[taxon]:
			if visited[pivot] == 0:
				visited[pivot] = 1
				clusters[pivot] = [pivot]
				self.expand(taxon, clusters, pivot, pivot, visited, eps)

		# I don think the following loop will be necessary
		for q in self.points[taxon]:
			qIsAlone = 1
			for key in clusters:
				if q in clusters[key]:
					qIsAlone = 0
			if qIsAlone:
				clusters[q] = [q]
				
		return clusters


	def expand(self, taxon, clusters, pivot, border, visited, eps):
		for newborder in self.points[taxon]:
			if visited[newborder] == 0:
				if border != newborder:
					#td = ((border[0] - newborder[0]) ** 2 + (border[1] - newborder[1]) ** 2) ** 0.5
					td = self.haversine(border, newborder)
					if td < eps:
						clusters[pivot].append(newborder)
						visited[newborder] = 1
						self.expand(taxon, clusters, pivot, newborder, visited, eps)


	def haversine(self, pointA, pointB, radius = 6371):
		phi1 = pointA[1] * pi / 180 # in radians
		phi2 = pointB[1] * pi / 180
		phiDelta = (pointB[1] - pointA[1]) * pi / 180
		lambdaDelta = (pointB[0] - pointA[0]) * pi / 180
		a = sin(phiDelta / 2) ** 2 + cos(phi1) * cos(phi2) * sin(lambdaDelta / 2) ** 2
		c = 2 * atan2(a ** 0.5, (1-a) ** 0.5)
		d = radius * c
		return d


	def getTiles(self, cellSize, offsetLat = 0.0, offsetLon = 0.0, minDist = 0.0):
		"""
		Create basic data structures required for the analysis from a collection
		of distributional points. Returns a list of data.Tile objects.

		Arguments:

		- cellSize (int or float): Size of the cells making up the lattice. If
		the grid is made up of squares, `cellSize` will be the side of the square.

		- minDist (float): Maximum distance (km) to consider two localities 
		the same population.

		"""
		if cellSize > (self.maxLatitude - self.minLatitude) or cellSize > (self.maxLongitude - self.minLongitude):
			raise ValueError("Grid cell size (`cellSize`) is larger than the extent of the input distributions.")

		self.cellSize = float(cellSize)
		tileStack = []
		self.rows, self.cols = 0, 0
		offsetLat = float(offsetLat)
		offsetLon = float(offsetLon)
		correctionFactor = self.cellSize / 100

		self.originN = ((self.minLongitude - offsetLon), (self.maxLatitude + offsetLat))
		span = (max((self.maxLongitude - self.originN[0]), self.cellSize), max((self.originN[1] - self.minLatitude), self.cellSize))
		totCols = int(ceil(span[0] / self.cellSize))
		totRows = int(ceil(span[1] / self.cellSize))
		self.rows, self.cols = totRows, totCols

		inclusion_grid = [[1 for x in range(totCols)] for x in range(totRows)]
		############################################
		# Select cells that are spatially excluded
		############################################

		self.presence_grid = [[0 for x in range(totCols)] for x in range(totRows)]
		grid_coll = []

		for taxon in self.points:

			# Join points that are too close to be different populations
			totPops = self.mergePoints(taxon, minDist)
			grid = [[0 for x in range(totCols)] for x in range(totRows)]

			for lon,lat in self.points[taxon]:
				apprindx = ceil(((lon - self.originN[0]) / span[0]) * totCols)
				apprindy = ceil(((self.originN[1] - lat) / span[1]) * totRows)
				
				if apprindx == 0:
					x = 0
				else:
					x = int(apprindx - 1)
				
				if apprindy == 0:
					y = 0
				else:
					y = int(apprindy - 1)

				th = self.points[taxon][lon,lat] / totPops								
				grid[y][x] += th
				self.presence_grid[y][x] += th
				
			#for row in grid:
			#	print(row)
			grid_coll.append(grid)


		self.index_reg = {}

		act_size = 0
		for ir, row in enumerate(self.presence_grid):
			for ic, cel in enumerate(row):
				if cel > 0:
					self.index_reg[(ir, ic)] = act_size
					act_size += 1

		for it, taxon in enumerate(self.points):
			#print(taxon)
			# instanciate pyMesh
			cat = self.iucn[taxon]['category']
			#tile = pydata.Meshpy(self.rows * self.cols, taxon, cat)
			tile = pydata.Meshpy(act_size, taxon, cat)
			for sca in self.iucn[taxon]['subcritA']:
				tile.newThreatSubcriteriaA(sca)
			#print('Rows: {0}, Cols: {1}'.format(self.rows, self.cols))
			for r in range(self.rows):
				for c in range(self.cols):
					if self.presence_grid[r][c] > 0:
						#print('{0}, {1}: {2}'.format(r, c, grid_coll[it][r][c]))
						rowNeighs = [r]
						colNeighs = [c]
						
						if grid_coll[it][r][c] > 0:
							#print("Index: {0}, row: {1}, col: {2}".format(self.index_reg[(r, c)], r, c))
							tile.setValue(self.index_reg[(r, c)], grid_coll[it][r][c])

						if r > 0:
							rowNeighs.append(r-1)

						if r < (self.rows - 1):
							rowNeighs.append(r+1)

						if c > 0:
							colNeighs.append(c-1)

						if c < (self.cols - 1):
							colNeighs.append(c+1)

						for nr in rowNeighs:
							if r != nr and self.presence_grid[nr][c] > 0:
								#print('\t{0}, {1}'.format(nr, c))
								tile.linkNeighs(self.index_reg[(r, c)], self.index_reg[(nr, c)])

						for nc in colNeighs:
							if c != nc and self.presence_grid[r][nc] > 0:
								#print('\t{0}, {1}'.format(r, nc))
								tile.linkNeighs(self.index_reg[(r, c)], self.index_reg[(r, nc)])

						#print("{0},{1}. Neighs: {2}".format(r, c, neighs))

			#tileStack.append(grid)
			tileStack.append(tile)

		return tileStack


	def tile2str(self, tile):
		"""
		Get string of Tile given a encodification scheme. Only for testing.
		"""
		if tile.getSize() == len(self.index_reg):		
			ms = ''

			for r in range(self.rows):

				for c in range(self.cols):

					if self.presence_grid[r][c] > 0:
						val = tile.getValue(self.index_reg[(r, c)])

						if val > 0:
							ms += '1 '

						else:
							ms += '0 '

					else:
						ms += '- '

				ms += '\n'

			return ms

