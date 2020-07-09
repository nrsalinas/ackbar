# Files and directories required

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

A .csv file with the list of species suporting each KBA previously delimited.
Each row should bear the information of a single pair species-KBA.
There are two mandatory fields: a column with species names (`scientificName`) and another with a key that associates the species to a KBA.
The name of the latter should be the same token employed as indexing field in the the KBA shapefile(s), and included into the configuration file throught the option "kba_index".


# Configuration file

A text file that contains all the parameters required for the analysis.
Each line should start with a parameter keyword, followed with a colon and its designated value.
Comments can be included, provided they are preceded by a sharp (`#`).
Possible keywords are the following:  

1. **distribution_file**: Name of the .csv file containing the distribution data. 

2. **iucn_file**:

3. **kba_species_file**:

4. **kba_directory**:

5. **kba_index**:

6. **exclusion_directory**: Path to a folder with shapefiles of areas to be excluded from the analysis. Optional.

7. **outfile_root**:

8. **cell_size**:

9. **offset_lat**:

10. **offset_lon**:

11. **eps**:

12. **iters**:

13. **max_kba**:
    
14. **ndn_weight**:
