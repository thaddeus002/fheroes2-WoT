/***************************************************************************
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

/************************************************************************//**
 * \file extractor.c                                                        *
 * \brief Program to extract files from a aggregate (.agg or .lod file).    *
 ****************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include "aggregation.h"




/**
 * Print the usage message.
 * \param progName The name of program
 */
void usage(char *progName) {
    fprintf(stdout, "Usage : %s extracted_dir\n", progName);
    exit(EXIT_FAILURE);
}



/** The main program */
int main(int argc, char **argv)
{
    /* Name of the agg directory */
    char *directory = NULL;
    int err;

    if(argc < 2) {
        usage(argv[0]);
    }

    directory = argv[1];

    err = create_aggregate(directory);

    /* end of program */
    if(!err) {
        fprintf(stdout, "Aggregate created\n");
    } else {
        fprintf(stderr, "An error occured : %d\n", err);
        return err;
    }

    return EXIT_SUCCESS;
}

