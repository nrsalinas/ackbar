## Categories file

A .csv file that contains the IUCN evaluation information required of the species to be analyzed.
All the info of a given species should be presented in a single row.
Species not included in this file will be assumed to be Not Evalutated (NE).

The file should have three columns:

- **Taxon**: Name of the taxon.
  
- **Category**: Standarized IUCN code of the category: `CR`, `EN`, `VU`, `NT`, `LC`, `DD`, `NE`.
  
- **Criteria**: This field should contain the standarized code of the assessment criteria (v.g., `A2c`, `B2(i,ii,iii)`, `A2ae;B1+2ab(iii,v);C2a(i)`). Be aware that the only letters that should be capitalized are criteria; subcriteria, thresholds, and parameter types should all be lowercase, if applicable.


## KBA shpefile directory

Path to a directory. Within this directory the user should have saved all the ESRI shapefiles with the polygons of previously delimited KBAs.
All polygons in this file should share a unique, indexed field. The name of that field should be annotated in the configuration file, option "kba_index". 

## KBA trigger species table

A .csv file with the list of species soporting each KBA previously delimited.
Each row should bear the information of a single pair species-KBA.
There are two mandatory fields: a column with species names (`scientificName`) and another with a key that associates the species to a KBA.
The name of the latter should be the same token employed as indexing field in the the KBA shapefile(s), and included into the configuration file throught the option "kba_index".


----

```python
parser = argparse.ArgumentParser(description = 'Ackbar: a Python software to delimit and update Key Biodiversity Areas.')

parser.add_argument('-d', '--distribution_file', required = True, dest = 'distrofile', metavar = '<infile>', action = 'store', help = 'Input distribution file in csv format. See manual for a detailed guideline.')

parser.add_argument('-e', '--evaluation_file', required = True, dest = 'evalfile', metavar = '<infile>', action = 'store', help = 'Input file of IUCN evaluations in csv format. See manual for a detailed guideline.')

parser.add_argument('-k', '--kba_directory', dest = 'kbaDir', metavar = '<directory>', action = 'store', help = 'Path to shapefiles of previously delimited KBA. See manual for a detailed guideline.')

parser.add_argument('-l', '--excluded_areas', dest = 'exclDir', metavar = '<directory>', action = 'store', help = 'Path to shapefiles of areas to exclude from analysis. See manual for a detailed guideline.')

parser.add_argument('-o', '--outfile_root', required = True, dest = 'outfileRoot', metavar = '<outfile_root_name>', action = 'store', help = 'Outfiles name root.')

parser.add_argument('-s', '--cell_size', required = True, dest = 'cellSize', metavar = '<#>', action = 'store', type = float, help = 'Grid cell size in geographic degrees.')

parser.add_argument('-x', '--long_offset', dest = 'lonOffset', metavar = '<#>', action = 'store', default = 0, type = float, help = 'Longitudinal offset of the W border of the grid in relation to the westernmost point in the infile. Should be a positive value in geographic degrees. Default = 0.')

parser.add_argument('-y', '--lat_offset', dest = 'latOffset', metavar = '<#>', action = 'store', default = 0, type = float, help = 'Latitudinal offset of the N border of the grid in relation to the northernmost point in the infile. Should be a positive value in geographic degrees. Default = 0.')

parser.add_argument('-c', '--cohesion', dest = 'cohesion', metavar = '<#>', default = 0.3, action = 'store', type = float, help = 'Clustering cohesion parameter (0.0--1.0). Default = 0.3.')

parser.add_argument('-v', '--version', action = 'version', version = "Ackbar v. {0}".format(version))
```