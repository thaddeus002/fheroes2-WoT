/**
 * \file aggregation.h
 * \brief AGG format handler.
 *
 * A AGG (or aggregate) file is a uniq file that contains all the data (artwork)
 * for the games HOMM1 (version 1) or HOMM2 (version 2). The second version of
 * AGG can also be used with fheroes2 since fheroes2 uses HOMM2 data.
 */


#ifndef AGGREGATOR_H
#define AGGREGATOR_H



/** filenames' length in aggregate */
#define AGGSIZENAME 15

/**
 * This define a file, with its name and the informations
 * we need to recover its data in the aggregate.
 */
typedef struct {
    char name[AGGSIZENAME+1]; /**< filename */
    uint32_t  offset; /**< offset of the data */
    uint32_t  size; /**< data's length */
} aggfile_t;


/**
 *
 */
typedef struct {
    int count; /**< number of files */
    aggfile_t *files; /**< an allocated array of count files */
} aggtable_t;





void destroy_aggtable(aggtable_t *table);


/**
 * \return a new table or NULL
 */
aggtable_t *read_aggtable(FILE *fd_data, int agg_version) ;


/**
 * Create the files in the given directory.
 * \return the number of extracted files
 */
int extract_files(FILE *fd_data, char *output_dir, aggtable_t *table);




#endif
