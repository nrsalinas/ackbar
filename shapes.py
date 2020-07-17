import os
import fileio
import fiona
from shapely.geometry import shape, Point, MultiPolygon, mapping
#from shapely.ops import unary_union


class KBA(object):
	"""
	Reader of shapefiles.

	- path (str): Path to folders containing shapefiles to read.

	- type (str): Shapefile purpose, which could be to set previously delimited
	KBAs (`kba`) or areas to exclude from the analyses for other reasons (`exclusion`). 
	"""

	def __init__(self, path, index_field, encoding = 'utf8', name_field = None):

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

		if type(distroData) == fileio.InputData:
			
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


