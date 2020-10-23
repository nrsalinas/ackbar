
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

import numpy as np
import rasterio
import re

from os import walk


class SDMs:
	
	def __init__(self, raster_directory):
		self.directory = raster_directory
		self.origin_N = None
		self.origin_W = None
		self.bounds = [None, None, None, None]
		self.index_reg = {}


	def iucnFile(self, categories_file):
		with open(categories_file, "r") as fhandle:
			pass
			

		
	def get_bounds(self):
		northest = -1e9
		westest = 1e9
		southest = 1e9
		eastest = -1e9
		
		for d, s, f in walk(self.directory):

			for file in f:
			
				if re.search(r"\.tif{1,2}$", file):
			
					with rasterio.open("/".join([d , file])) as src:
			
						ras = src.read(1)
						ras = np.where(ras < 0, np.nan, ras)
						
						sumrow = 0
						rowindx = 0
						while sumrow <= 0:
							row = ras[rowindx, :]
							row = row[~np.isnan(row)]
							if row.shape[0] > 0:
								sumrow = row.sum()
							rowindx += 1

						sumcol = 0
						colindx = 0
						while sumcol <= 0:
							col = ras[: , colindx]
							col = col[~np.isnan(col)]
							if col.shape[0] > 0:
								sumcol = col.sum()
							colindx += 1
							
						thwest, thnorth = src.xy(rowindx, colindx)
						
						sumrow = 0
						rowindx = ras.shape[0] - 1
						while sumrow <= 0:
							row = ras[rowindx, :]
							row = row[~np.isnan(row)]
							if row.shape[0] > 0:
								sumrow = row.sum()
							rowindx -= 1

						sumcol = 0
						colindx = ras.shape[1] - 1
						while sumcol <= 0:
							col = ras[: , colindx]
							col = col[~np.isnan(col)]
							if col.shape[0] > 0:
								sumcol = col.sum()
							colindx -= 1
						
						theast, thsouth = src.xy(rowindx, colindx)
						
						if thwest < westest:
							westest = thwest

						if thnorth > northest:
							northest = thnorth
						
						if theast > eastest:
							eastest = theast 
						
						if thsouth < southest:
							southest = thsouth
		
		self.bounds = [northest, southest, westest, eastest]
		return None

						
	def pop_in_cell(self, src, ras, points):
		"""Point is a coordinate list of a cell corners: NW, NE, SE, SW"""
		x0,y0 = src.index(points[0], points[1])
		x1,y1 = src.index(points[2], points[3])
		temp = ras[x0:(x1+1), y0:(y1+1)]
		return temp[temp == 1].sum()
	

	def set_grid_prop(self, cell_size, offset):
		
		self.cellSize = cell_size
		self.origin_N = self.bounds[0] + offset
		self.origin_W = self.bounds[0] - offset
		span = [max((self.bounds[3] - self.origin_N), self.cellSize), max((self.origin_W - self.bounds[1]), self.cellSize)]
		totCols = int(np.ceil(span[0] / self.cellSize))
		totRows = int(np.ceil(span[1] / self.cellSize))
		#span[0] = totCols * self.cellSize
		#span[1] = totRows * self.cellSize
		self.rows, self.cols = totRows, totCols
		


	def getTiles(self):

		taxa = []
		gridColl = []
		presenceGrid = [[0 for x in range(self.cols)] for y in range(self.rows)]

		for d, s, f in walk(self.directory):

			for file in f:
			
				if re.search(r"\.tif{1,2}$", file):
			
					with rasterio.open("/".join([d , file])) as src:
						
						thisgrid = []
						taxon = re.sub(r"\.tif{1,2}$", "", file)
						ras = src.read(1)
						ras = np.where(ras < 0, np.nan, ras)
						popExt = ras[~np.isnan(ras)].sum()
			
						for ir, row in enumerate(range(self.rows)):
							
							thisrow = []

							for ic, col in enumerate(range(self.cols)):
			
								x0 = self.origin_W + row * self.cellSize
								y0 = self.origin_N - col * self.cellSize
								x1 = self.origin_W + (row + 1) * self.cellSize
								y1 = self.origin_N - (col + 1) * self.cellSize
								thcell = self.pop_in_cell(src, ras, (x0, y0, x1, y1))
								thisrow.append(thcell)
								presenceGrid[ir][ic] += thcell

							thisgrid.append(thisrow)

						gridColl.append(thisgrid)
						taxa.append(taxon)
		#---
		self.index_reg = {}
		act_size = 0

		for ir, row in enumerate(presenceGrid):
			
			for ic, cel in enumerate(row):
				
				if cel > 0:
					
					self.index_reg[(ir, ic)] = act_size
					act_size += 1
		#---
		#for it, tax in taxa:
		#################
		### IN PROGRESS ###

