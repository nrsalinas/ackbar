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