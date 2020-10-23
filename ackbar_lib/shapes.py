###############################################################################
#
#	Copyright 2020 Nelson R. Salinas
#
#
#	This file is part of Ackbar.
#
#   Ackbar is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
# 	(at your option) any later version.
#
#	Ackbar is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with Ackbar. If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################


import os
import pyproj
import fiona
from fiona.crs import from_epsg
from shapely.geometry import shape, Point, Polygon, mapping, MultiPoint
from shapely.ops import unary_union, transform
from functools import partial
from ackbar_lib import fileio as fileio

class KBA(object):
	"""
	Shapefile reader.

	- path (str): Path to folders containing shapefiles to read.
	"""

	def __init__(self, path, index_field, encoding = 'utf8'):

		self.source_directory = None
		self.polys = {}
		self.new_trigger_spp = {}
		self.encoding = encoding
		self.index_field = index_field

		if not os.path.exists(path):
			raise IOError("`{0}` is not a valid directory.".format(path))

		self.source_directory = path

		#if shtype is None or (shtype != 'kba' and shtype != 'exclusion' and shtype != 'reserves'):
		#	raise ValueError("`{0}` is not a valid value for parameter `shtype`. Valid values are `kba`, `exclusion`, and `reserves`.".format(shtype))

		#self.shape_type = shtype

		for directory, subdi, files in os.walk(self.source_directory):
		
			for fi in files:
		
				if fi.endswith('.shp'):
		
					try:

						toop = directory + "/" + fi
						filehandle = fiona.open(toop, crs= 'EPSG:4326', encoding = self.encoding)

						for item in filehandle:
							#self.polys.append(shape(item['geometry']))
							self.polys[item['properties'][self.index_field]] = {
								'shape': shape(item['geometry']),
								'new_spp': {}
								}
					
					except:

						raise IOError("Could not open file `{0}`".format(fi))

		# Maybe it is not necessary to merge all polygons
		#self.upoly = unary_union(self.polys)


	def spp_inclusion(self, distroData):
		"""
		Verify if new species could support previously delimited KBA.
		"""

		if isinstance(distroData, fileio.InputData):
			
			#print("In inclusion")
			#self.new_trigger_spp = {x:{} for x in range(len(self.polys))}

			for k in self.polys:
				
				for spp in distroData.points:
					pointsWithin = []
					popsize = 0
					isTrigger = False
					criteria = []
					
					for p in distroData.points[spp]:
						tp = Point(p)
						
						if self.polys[k]['shape'].contains(tp):
							pointsWithin.append(p)
							popsize += distroData.points[spp][p]

					#print(popsize)
						
					if distroData.iucn[spp]['category'] in ['CR', 'EN']:

						if popsize > 0.95:
							isTrigger = True
							criteria.append(4)
							criteria.append(0)
							
						elif popsize > 0.005:
							isTrigger = True
							criteria.append(0)
							
						if popsize >= 0.01 and len(distroData.iucn[spp]['subcritA']) > 0:
							isTrigger = True
							criteria.append(2)
							
					elif distroData.iucn[spp]['category'] == 'VU':

						if popsize > 0.01:
							isTrigger = True
							criteria.append(1)
							
						elif popsize >= 0.02 and len(distroData.iucn[spp]['subcritA']) > 0:
							isTrigger = True
							criteria.append(3)
							
					if popsize > 0.1:
						isTrigger = True
						criteria.append(5)
					
					if isTrigger:
						#self.new_trigger_spp[ik][spp] = criteria
						self.polys[k]['new_spp'][spp] = criteria

					if len(pointsWithin) > 0:
						for q in pointsWithin:
							distroData.points[spp][q] = 0


	def new_spp_table(self, filename):
		"""
		Writes out a simple csv file indicating new trigger species to previously
		delimited KBA.
		"""
		crmap = {0: 'A1a', 1: 'A1b', 2: 'A1c', 3: 'A1d', 4: 'A1e', 5: 'B1', 6: 'B2'}
		bffr = '{0},Species,Criteria\n'.format(self.index_field)

		for kbaid in self.polys:
			for sp in self.polys[kbaid]['new_spp']:
				cr = '"'
				for c in self.polys[kbaid]['new_spp'][sp]:
					cr += crmap[c] + ', '
				cr = cr.rstrip(', ')
				cr += '"'
				bffr += '{0},{1},{2}\n'.format(kbaid, sp, cr)

		with open(filename, 'w') as fhandle:
			fhandle.write(bffr)


def solution2shape(mysols, indata, dic_name = 'solutions'):


	if os.path.exists(dic_name):
		raise IOError("There is already a directory called `{0}`. Please chose another name for the solution folder.".format(dic_name))
	else:
		os.mkdir(dic_name)

	irkeys = list(indata.index_reg.keys())
	wrtMode = None

	schema = {
		'geometry': 'Polygon',
		'properties': {'id': 'int',
			'IUCNscore': 'int',
			'aggrScore': 'float',
			'NDMscore': 'float'},
		}

	for igr, gr in enumerate(mysols):
		filename = '{0}/group_{1}.shp'.format(dic_name, igr)

		for its,  tsol in enumerate(gr):
			polys = []
			solpoly = None
			
			for ic in range(tsol.getSize()):
			
				if tsol.getValue(ic) > 0:
			
					y, x = irkeys[ic]
					xBase = indata.originN[0] + indata.cellSize * x
					yBase = indata.originN[1] - indata.cellSize * y
					ocor = [(xBase + indata.cellSize, yBase),
						(xBase, yBase),
						(xBase, yBase - indata.cellSize),
						(xBase + indata.cellSize, yBase - indata.cellSize)]
					polys.append(Polygon(ocor))
			
			solpoly = unary_union(polys)
			
			if its == 0:
				wrtMode = 'w'
			else:
				wrtMode = 'a'
			
			with fiona.open(filename, wrtMode, 'ESRI Shapefile', schema, from_epsg(4326)) as c:
				
				c.write({
					'geometry': mapping(solpoly),
					'properties': {
						'id': its,
						'IUCNscore': tsol.score,
						'aggrScore':  tsol.aggrScore,
						'NDMscore': tsol.ndmScore
						}})

	return None


def area_estimator(point_list, lat0 = 0, lon0 = -73, factor = 0.9992):
	"""
	Estimates the area (km^2) of the convex hull of a set of points. Points 
	should be longitude-latitude points, projected in the WGS84 datum. Area will 
	be estimated using the Transverse Mercator projection. Origin coordinates of 
	the Transverse Mercator projection should be provided; if not set, the 
	Colombian offical origin will be used. Scale factor can be also parsed 
	(default = 0.9992).
	""" 
	out = None

	if len(point_list) > 2:

		convex_hull = MultiPoint(point_list).convex_hull
		wgs84 = pyproj.Proj(init='epsg:4326')
		tm = pyproj.Proj(proj='tmerc', lat_0 = lat0, lon_0 = lon0, k_0=factor, units='m')
		project = partial(pyproj.transform, wgs84, tm)
		tm_ch = transform(project, convex_hull)
		out = tm_ch.area / 1000 ** 2
		
	return out


def filter_points(points, shapefile):
	"""
	Filter points of a fileio.Indata object given a polygon.
	"""
	feats = []
	filtered = {tax:{} for tax in points}

	with fiona.open(shapefile, encoding="utf8") as src:
		feats = [shape(x['geometry']) for x in src]

	for taxon in points:

		for lon, lat in points[taxon]:

			keep = False

			for polyg in feats:

				if polyg.contains(Point(lon, lat)):
					keep = True
					break
			
			if keep:
				filtered[taxon][(lon, lat)] = points[taxon][(lon, lat)]

	filtered = {tax: filtered[tax] for tax in filtered if len(filtered[tax]) > 0}

	return filtered
