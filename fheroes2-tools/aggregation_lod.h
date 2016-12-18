/**
 * \file aggregation_lod.h
 * \brief LOD format handler.
 */

#ifndef AGGREGATOR_LOD_H
#define AGGREGATOR_LOD_H


/**
 * Extract the data from an aggregate stream in the directory indicated.
 * \param fd_data data stream from input file
 * \param output_dir the name of the directory where to create the files
 * \return the number of files extracted
 */
int extract_lod(FILE *fd_data, char *output_dir);


#endif
