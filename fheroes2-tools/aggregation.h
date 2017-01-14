/**
 * \file aggregation.h
 * \brief AGG/LOD format handler.
 *
 * A AGG (or aggregate) file is a uniq file that contains all the data (artwork)
 * for the games HOMM1 (version 1) or HOMM2 (version 2). The second version of
 * AGG can also be used with fheroes2 since fheroes2 uses HOMM2 data.
 *
 * LOD files are for HOMM3. In these files data can be compressed.
 */


#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#ifndef WITH_ZLIB
    #define WITH_ZLIB
    #include "zlib.h"
#endif



/** filenames' length in aggregate */
#define AGGSIZENAME 15
#define LODSIZENAME 16


/** error codes */
#define AGG_ALLOC_ERROR -1
#define AGG_OPENDIR_ERROR -2
#define AGG_CREATE_FILE_ERROR -3


/**
 * This define a file, with its name and the informations
 * we need to recover its data in the aggregate.
 */
typedef struct {
    char name[LODSIZENAME+1]; /**< filename */
    uint32_t  offset; /**< offset of the data */
    uint32_t  size; /**< size of file before compression */
    uint32_t  length; /**< compressed size if file is compressed. 0 otherwise => use "size" */
} aggfile_t;


/**
 *
 */
typedef struct {
    int count; /**< number of files */
    aggfile_t *files; /**< an allocated array of count files */
} aggtable_t;



/**
 * \brief Free memory used by a aggtable_t struct.
 */
void destroy_aggtable(aggtable_t *table);


/**
 * \param fd_data file descriptor of the aggregate
 * \param agg_version must be 1 or 2
 * \return a new table or NULL
 */
aggtable_t *read_aggtable(FILE *fd_data, int agg_version) ;

/**
 * read a LOD file.
 */
aggtable_t *read_lodtable(FILE *fd_data);


/**
 * Create the files in the given directory.
 * \return the number of extracted files
 */
int extract_files(FILE *fd_data, char *output_dir, aggtable_t *table);


/**
 * Make an aggragate with the content of a directory.
 * Filenames will be shorten to 8 characters plus 3 for the extension.
 * Subdirectories will be ignored.
 * Aggregate name will be dirname plus extension .agg
 * Agg version will be 2.
 * \return a negative error code or zero in case of success
 */
int create_aggregate(char *directory);





#endif
