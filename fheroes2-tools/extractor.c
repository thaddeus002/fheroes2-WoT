/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
 *                                                                         *
 *   Copyrigth (C) 2015 by Yannick Garcia                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// for mkdir()
#include <sys/stat.h>
#include <sys/types.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "aggregation.h"
#include "aggregation_lod.h"

// Default Version of the AGG file (1 for HOMM1, 2 for HOMM2, 3 for HOMM3)
#ifndef AGGVERSION
#define AGGVERSION 2
#endif




/**
 * Print the usage message.
 * \param progName The name of program
 */
void usage(char *progName) {
    fprintf(stdout, "Usage : %s [-v agg_version ] path_heroes.[agg|lod] extract_to_dir\n", progName);
    fprintf(stdout, "        -v agg_version : indicate the version of source file. Can be 1, 2, or 3 (default = %d)\n", AGGVERSION);
    exit(EXIT_FAILURE);
}









/**
 * Extract the data from an aggregate stream in the directory indicated.
 * \param fd_data data stream from input file
 * \param output_dir the name of the directory where to create the files
 * \param agg_version the version of HOMM from whom the data file is (may be 1, 2 or 3)
 * \return the number of files extracted
 */
int extract(FILE *fd_data, char *output_dir, int agg_version){

    aggtable_t *table = NULL;
    int total = 0;

    switch(agg_version) {
    case 1:
    case 2:
        table = read_aggtable(fd_data, agg_version);
        break;
    case 3:
        table = read_lodtable(fd_data);
        break;
    default:
        fprintf(stderr, "Bad agg version : %d\n", agg_version);
    }

    if(table != NULL) {
        total = extract_files(fd_data, output_dir, table);
        destroy_aggtable(table);
    }

    return total;
}




/** The main program */
int main(int argc, char **argv)
{
    /* Name of the agg file */
    char *aggregate = NULL;

    /* The directory where extract to */
    char *output_dir = NULL;

    /* Version of the AGG file (1 for HOMM1, 2 for HOMM2, 3 for HOMM3) */
    int agg_version = AGGVERSION;

    /* Read commandline args */
    int opt;

    FILE *fd_data;

    int total;

    while ((opt = getopt(argc, argv, "v:")) != -1) {
        switch (opt) {
        case 'v':
            agg_version = atoi(optarg);
            break;
        default:
            usage(argv[0]);
        }
    }

    if (optind >= argc-1) {
         usage(argv[0]);
    }

    aggregate = argv[optind];
    output_dir = argv[optind+1];


    /* open data file */
    fd_data = fopen(aggregate, "r");

    if(fd_data == NULL)
    {
        fprintf(stderr, "ERROR : Can not open file %s\n", aggregate);
        return EXIT_FAILURE;
    }

    fprintf(stdout, "Reading file %s as Homm v%d datafile\n", aggregate, agg_version);

    total = extract(fd_data, output_dir, agg_version);

    /* close data file */
    fclose(fd_data);

    /* end of program */
    fprintf(stdout, "Number of extracted files : %d\n", total);

    return EXIT_SUCCESS;
}


