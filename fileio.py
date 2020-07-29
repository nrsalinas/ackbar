import sys
import csv
import re
import pydata
from math import ceil, floor, sin, cos, atan2, pi

import fiona
from fiona.crs import from_epsg
from shapely.geometry import Point, Polygon, MultiPolygon, mapping

import shapes


class InputData(object):
	"""
	Input data processor class. Class constructor requires a csv file (str) with
	three columns: longitude, latitude, and taxon name.
	"""
	def __init__(self, infile):
		self.points = {} # values are population fractions
		self.iucn = {} # taxon to (Category, subcriteriaA)
		self.minLatitude = 91.0
		self.maxLatitude = -91.0
		self.minLongitude = 181.0
		self.maxLongitude = -181.0
		self.originN = None
		self.cellSize = None
		self.rows = None
		self.cols = None
		self.geometry = None
		self.csvfile = infile
		self.presence_grid = []
		self.index_reg = {}
		self.taxonGroups = {}
		self.taxonGroupsInfo = {}
		lineCounter = 0
		latCol = int()
		lonCol = int()

		with open(infile,'r') as fil:
			table = csv.reader(fil)
			for row in table:
				lineCounter += 1
				if lineCounter == 1 and (re.search(r"[^0-9\.\-]",row[1]) or re.search(r"[^0-9\.\-]",row[2])):
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
				
				row[2] = re.sub(r"[\s\'\"]","",row[2])
				row[1] = re.sub(r"[\s\'\"]","",row[1])

				if len(row[latCol]) < 1:
					raise IOError("Line {0} in `{1}` do not contain latitude data.".format(lineCounter, infile))
				if re.search(r"[^0-9\.\-]",row[latCol]) or float(row[latCol]) < -90 or float(row[latCol]) > 90:
					raise IOError("Line {0} in `{1}` contains invalid coordinate value(s): `{2}`.".format(lineCounter, infile, row[latCol]))

				if len(row[lonCol]) < 1:
					raise IOError("Line {0} in `{1}` do not contain longitude data.".format(lineCounter, infile))
				if re.search(r"[^0-9\.\-]",row[lonCol]) or float(row[lonCol]) < -180 or float(row[lonCol]) > 180:
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


	def groupFiles(self, assignments_file, diversity_file):
		
		log = '' # log buffer
		assignments = False
		taxonCol = None
		groupCol = None
		rangeCol = None
		groupBisCol = None
		globsppCol = None
		rangeThresCol = None
		self.taxonGroups = {x: None for x in self.points.keys()}

		with open(assignments_file, 'r') as afile:
			
			table = csv.reader(afile)
			
			for irow , row in enumerate(table):
				
				if irow == 0:

					for ic , cell in enumerate(row):

						if re.search('group', cell, flags=re.I):
							groupCol = ic
							continue

						if re.search('taxon', cell, flags=re.I):
							taxonCol = ic
							continue

						if re.search('range_size', cell, flags=re.I):
							rangeCol = ic
							continue

					if groupCol is None or taxonCol is None:
						raise IOError("Input file `{0}`: column labels do not follow the required format (`Group`, `Taxon`, `Range_size`).".format(assignments_file))

				else:
					rangeS = None

					if not rangeCol is None:
						rangeS = re.sub(r'^\s+', '', row[rangeCol])
						rangeS = re.sub(r'\s+$', '', row[rangeCol])

						if len(rangeS) > 0:
							rangeS = float(rangeS)
								
							if rangeS <= 0:
								raise IOError("Invalid range size provided (`{0}`)".format(rangeS))

						else:
							rangeS = None

					group = re.sub(r'^\s+', '', row[groupCol])
					group = re.sub(r'\s+$', '', row[groupCol])

					if len(group) < 4:
						raise IOError("`{0}` does not seem an actual taxonomic membership".format(group))

					taxon = re.sub(r'^\s+', '', row[taxonCol])
					taxon = re.sub(r'\s+$', '', row[taxonCol])

					if not taxon in self.taxonGroups:
						terr = '`{0}` not included in distribution file\n'.format(taxon)
						print(terr, file = sys.stderr)
						log += terr
				
					if not self.taxonGroups[taxon] is None:
						raise IOError("Taxon duplicated in group file (`{0}`)".format(taxon))

					else:
						self.taxonGroups[taxon] = {'group': group, 'range_size': rangeS}
						self.taxonGroupsInfo[group] = None

		with open(diversity_file, 'r') as dhandle:

			table = csv.reader(dhandle)
			
			for irow , row in enumerate(table):
				
				if irow == 0:

					for ic , cell in enumerate(row):

						if re.search('group', cell, flags=re.I):
							groupBisCol = ic
							continue

						if re.search('global_species', cell, flags=re.I):
							globsppCol = ic
							continue

						if re.search('range_threshold', cell, flags=re.I):
							rangeThresCol = ic
							continue

					if groupBisCol is None or globsppCol is None:
						raise IOError("Input file `{0}`: column labels do not follow the required format (`Group`, `Global_species`, `Range_threshold`).".format(assignments_file))

				else:

					tgroup = row[groupBisCol]
					tsp = row[globsppCol]
					range_thr = None

					if rangeThresCol:
						range_thr = row[rangeThresCol]
						if len(range_thr) > 0:
							range_thr = float(range_thr)
						else:
							range_thr = 10000.0 

					if len(tsp) > 0:
						tsp = int(tsp)

					else:
						tsp = None

					if tgroup in self.taxonGroupsInfo:
						self.taxonGroupsInfo[tgroup] = {'range_threshold': range_thr, 'global_species': tsp}

					else:
						raise IOError("Taxon `{0}` not included in group assignment file.".format(tgroup))

		for sp in self.points:

			if not self.taxonGroups[sp]['range_size']:
			
				point_list = [x for x in self.points[sp].keys()]
				tarea = shapes.area_estimator(point_list)
				self.taxonGroups[sp]['range_size'] = tarea

		
		# Integer dictionaries for search routine functions

		self.groupDict = {}
		self.spp2groupDict = {}

		for igr, gr in enumerate(sorted(self.taxonGroupsInfo.keys())):
		
			mran = self.taxonGroupsInfo[gr]['range_threshold']
			mspp = int(self.taxonGroupsInfo[gr]['global_species'] * 0.0002)
		
			if mspp < 2:
				mspp = 2
		
			self.groupDict[igr] = (mran, mspp)


		for ispp, spp in enumerate(sorted(self.taxonGroups.keys())):
		
			tgr = self.taxonGroups[spp]['group']
		
			for igr, gr in enumerate(sorted(self.taxonGroupsInfo.keys())):
		
				if tgr == gr:
					tgr = igr
					break
		
			self.spp2groupDict[ispp] = tgr



	def iucnFile(self, filename):
		"""
		Process IUCN categories and subcriteria for criterion A from a csv file.
		"""
		lineCounter = 0
		nameCol = None
		categCol = None
		criterCol = None
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

						if re.search("criter",row[ic],flags=re.I):
							criterCol = ic
							continue
					#print(nameCol)
					#print(categCol)
					#print(criterCol)
					if nameCol is None or categCol is None or criterCol is None:
						raise IOError("Input file `{0}`: column labels do not follow the required format (`Taxon`, `Category`, `Criteria`).".format(filename))
				
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
					if type(row[criterCol]) == str:
						
						if not re.search(r'[BCDE]', row[criterCol]) and re.search(r'A', row[criterCol]):

							digits = re.findall(r'\d', row[criterCol])
							#print(digits)

							if len(digits) >= 1:

								for dig in digits:

									if dig == '1':
										subcrA.append(1)

									elif dig == '2':
										subcrA.append(2)

									elif dig == '4':
										subcrA.append(4)

									elif dig != '3':
										raise IOError("{0} has non valid subcriteria A (`{1}`)".format(row[nameCol], row[criterCol]))

					self.iucn[row[nameCol]] = {'category': cat, 'subcritA': subcrA}
		
			for na in [x for x in self.points if not x in self.iucn]:

				self.iucn[na] = {'category': 'NE', 'subcritA': []}


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
		
		self.originN = ((self.minLongitude - offsetLon), (self.maxLatitude + offsetLat))
		span = [max((self.maxLongitude - self.originN[0]), self.cellSize), max((self.originN[1] - self.minLatitude), self.cellSize)]
		totCols = int(ceil(span[0] / self.cellSize))
		totRows = int(ceil(span[1] / self.cellSize))
		span[0] = totCols * self.cellSize
		span[1] = totRows * self.cellSize
		self.rows, self.cols = totRows, totCols
		

		############################################
		# Select cells that are spatially excluded

		#inclusion_grid = [[1 for x in range(totCols)] for x in range(totRows)]

		############################################

		self.presence_grid = [[0 for x in range(totCols)] for x in range(totRows)]
		grid_coll = []

		for taxon in self.points:

			# Join points that are too close to be different populations
			totPops = self.mergePoints(taxon, minDist)
			grid = [[0 for x in range(totCols)] for x in range(totRows)]

			for lon,lat in self.points[taxon]:
				apprindx = ceil( ( (lon - self.originN[0]) / span[0]) * totCols)
				apprindy = ceil( ( (self.originN[1] - lat) / span[1]) * totRows)
				
				x = apprindx - 1
				y = apprindy - 1

				if x < 0:
					x += 1

				if y < 0:
					y += 1


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
			
			##################################################
			##################################################
			##################################################
			# Review if A subcriteria are correctly processed

			for sca in self.iucn[taxon]['subcritA']:
				tile.newThreatSubcriteriaA(sca)

			##################################################
			##################################################
			##################################################

			if len(self.taxonGroups) > 0 and taxon in self.taxonGroups and self.taxonGroups[taxon]['range_size']:

				tile.setRange(self.taxonGroups[taxon]['range_size'])
		
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


	def grid2shape(self, filename):
		"""
		Saves the grid into a shapefile.
		"""
		#polys = []
		#multipol = None
		#irkeys = list(self.index_reg.keys())
		counter = 0
		wrtMode = None

		schema = {
			'geometry': 'Polygon',
			'properties': {'id': 'int', 'x': 'int', 'y': 'int', 'xBase': 'float', 'yBase': 'float'},
			}

		for y, x in self.index_reg:
			#y, x = irkeys[ic]
			xBase = self.originN[0] + self.cellSize * (x + 1)
			yBase = self.originN[1] - self.cellSize * (y + 1)
			ocor = [(xBase, yBase + self.cellSize),
				(xBase - self.cellSize, yBase + self.cellSize),
				(xBase - self.cellSize, yBase),
				(xBase, yBase)]
			pol = Polygon(ocor)
			#polys.append(Polygon(ocor))
			
		#multipol = MultiPolygon(polys)
			
			if counter == 0:
				wrtMode = 'w'
			else:
				wrtMode = 'a'

			with fiona.open(filename, wrtMode, 'ESRI Shapefile', schema, from_epsg(4326)) as fhandle:
				fhandle.write({'geometry': mapping(pol), 'properties': 
					{'id': counter, 'x': x, 'y': y, 'xBase': xBase, 'yBase': yBase}})

			counter += 1

		return None


