/**
 * \file aggregation.c
 * \brief AGG format handler.
 *
 * A AGG (or aggregate) file is a uniq file that contains all the data (artwork)
 * for the games HOMM1 (version 1) or HOMM2 (version 2). The second version of
 * AGG can also be used with fheroes2 since fheroes2 uses HOMM2 data.
 * This file format is :
 *   - 2 bytes for number of files (n) in little endien.
 *   - n * 12 bytes (for version 2) :
 *       - id : 4 bytes
 *       - offset : 4 bytes
 *       - size : 4 bytes
 *   - or n * 14 bytes (for version 1):
 *       - id : 4 bytes
 *       - unknown : 2 bytes
 *       - size : 4 bytes
 *       - repeated size : 4 same bytes
 *   - files' data
 *   - 15 * n bytes for filenames
 */


#include <endian.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // tolower()
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "aggregation.h"


/**
 * Try to create a directory if it doesn't exist.
 * If a file with same name exists, or the directory doesn't exist and creating
 * it has failed, the function returns an error code.
 * \returns a non null code if an error occured
 */
static int create_dir(const char *directory) {
    struct stat buf;
    int err = 0;

    if(stat(directory, &buf)) {
        // file doesn't exist or is not accessible => try create it
        if(mkdir(directory, 0755)){
            fprintf(stderr, "ERROR : Fail to create directory %s\n", directory);
            err = 1;
        }
    } else {
        // file already exists
        if(!S_ISDIR(buf.st_mode)) {
            err=2;
        }
    }

    return err;
}




/* report a zlib or i/o error */
static void zerr(int ret)
{
    fputs("zpipe: ", stderr);
    switch (ret) {
    case Z_ERRNO:
        if (ferror(stdin))
            fputs("error reading stdin\n", stderr);
        if (ferror(stdout))
            fputs("error writing stdout\n", stderr);
        break;
    case Z_STREAM_ERROR:
        fputs("invalid compression level\n", stderr);
        break;
    case Z_DATA_ERROR:
        fputs("invalid or incomplete deflate data\n", stderr);
        break;
    case Z_MEM_ERROR:
        fputs("out of memory\n", stderr);
        break;
    case Z_VERSION_ERROR:
        fputs("zlib version mismatch!\n", stderr);
    }
}






/**
 * Write a file in the given output directory.
 * \return a non null value if an error occured
 */
static int write_file(FILE *fd_agg, aggfile_t *file, const char *output_dir) {

    unsigned char *body; /* file's data */
    char *full_name; /* new file's name */
    FILE *fd_output;
    int err = 0;
    int compress; /* are data compressed ? */
    int longueur; /* number of byte to read */

    if(file->length) {
        longueur=file->length;
        compress=1;
    } else {
        longueur=file->size;
        compress=0;
    }

    // reading data
    fseek(fd_agg, file->offset, SEEK_SET);


    body = malloc(sizeof(unsigned char) * longueur);
    if(body == NULL) {
        fprintf(stderr, "Failed allocate memory. File %s not created.\n", file->name);
        return 1;
    }

    fread(body, longueur, 1, fd_agg);
    fprintf(stdout, "Lecture de %s sur %d octets -> %d octets décompressés\n", file->name, longueur, file->size);

    // uncompress body
    if(compress) {
        unsigned char *infBody;
        int err; // error code

        infBody = malloc(sizeof(unsigned char)*file->size);
        // TODO if(infBody == NULL) { ...
        err = uncompress(infBody, (uLongf *) &file->size, body, longueur);

        if(err) {
            zerr(err);
            // extraction failed
            return 2;
        } else {
            free(body);
            body = infBody;
        }
    }

    // creating extracted file
    full_name = malloc(sizeof(char) * (strlen(output_dir) + strlen(file->name) + 2));
    if(full_name==NULL) {
        fprintf(stderr, "Failed allocate memory. File %s not created.\n", file->name);
        free(body);
        return 1;
    }

    sprintf(full_name, "%s/%s", output_dir, file->name);

    fd_output = fopen(full_name, "w");
    if(fd_output != NULL) {
        fwrite(body, file->size, 1, fd_output);
        fclose(fd_output);
        fprintf(stdout, "extracted : %s\n", full_name);

    } else {
        fprintf(stderr, "Fail creating file %s\n", full_name);
        err = 2;
    }

    free(body);
    free(full_name);

    return err;
}




void destroy_aggtable(aggtable_t *table) {

    if(table == NULL) return;

    if(table->files != NULL) free(table->files);

    free(table);
}


/**
 * \return a new table or NULL
 */
aggtable_t *read_aggtable(FILE *fd_data, int agg_version) {

    uint16_t count; /* 16 bits read in data file : number of elements in aggregate */
    char buf[AGGSIZENAME]; /* name of a element */
    uint32_t offset; /* for version 1 only */
    int i;

    aggtable_t *table;

    table = malloc(sizeof(aggtable_t));

    if(table==NULL) {
        fprintf(stderr, "Failed allocating memory\n");
        return NULL;
    }

    /* read 16 bits */
    fread(&count, sizeof(uint16_t), 1, fd_data);
    count = le16toh(count);

    /* reading of $count elements in data file */
    fprintf(stdout, "Extracting %d elements\n", count);
    table->files = malloc(sizeof(aggfile_t) * count);
    if(table->files == NULL) {
        fprintf(stderr, "Failed allocating memory\n");
        destroy_aggtable(table);
        return NULL;
    }

    offset = 2 + (count*14); // first file 's offset

    for(i = 0; i < count; i++)
    {
        uint32_t id;
        uint8_t j; /* counter */

        aggfile_t *fat  = table->files + i;
        fseek(fd_data, -AGGSIZENAME * (count - i), SEEK_END);
        fread(buf, AGGSIZENAME, 1, fd_data);

        for(j = 0; j < AGGSIZENAME; j++) {
            buf[j] = tolower(buf[j]);
        }

        strncpy(fat->name, buf, AGGSIZENAME);
        fat->name[AGGSIZENAME]='\0';

        // finding offset and size
        if(agg_version==2)
            fseek(fd_data, sizeof(uint16_t) + i * (3 * sizeof(uint32_t)), SEEK_SET);
        else {
            fseek(fd_data, sizeof(uint16_t) + i * ((3 * sizeof(uint32_t)) + sizeof(uint16_t)), SEEK_SET);
            fat->offset=offset;
        }

        fread(&id, sizeof(uint32_t), 1, fd_data);
        id = le32toh(id);

        if(agg_version==1) {
            fseek(fd_data, sizeof(uint16_t), SEEK_CUR);
        } else if(agg_version==2){
            fread(&(fat->offset), sizeof(uint32_t), 1, fd_data);
            fat->offset = le32toh(fat->offset);
        }

        fread(&(fat->size), sizeof(uint32_t), 1, fd_data);
        fat->size = le32toh(fat->size);

        // On ajoute la taille du fichier courant pour l'offset du fichier suivant
        if(agg_version==1) {
            offset+=fat->size;
            fat->offset=offset;
        }
   }

   return table;
}



aggtable_t *read_lodtable(FILE *fd_data) {

    int count = 0;
    aggtable_t *table;
    char archiveType[4]; /* First four bytes of file, must be LOD\0 */
    int i;

    fread(archiveType, 4, 1, fd_data);

    if(strncmp(archiveType, "LOD", 4)) {
        fprintf(stderr, "Bad file type\n");
        return NULL;
    }

    table = malloc(sizeof(aggtable_t));
    if(table==NULL) {
        fprintf(stderr, "Failed allocating memory\n");
        return NULL;
    }

    // we ignore the 4 next bytes
    fseek(fd_data, 4, SEEK_CUR);

    /* read 32 bits */
    fread(&count, sizeof(uint32_t), 1, fd_data);
    count = le32toh(count);

    table->files = malloc(sizeof(aggfile_t)*count);

    if(table->files==NULL) {
        fprintf(stderr, "Failed allocating memory\n");
        destroy_aggtable(table);
        return NULL;
    }

    // we ignore the 80 next bytes
    fseek(fd_data, 80, SEEK_CUR);

    /* reading of $count elements in data file */
    fprintf(stdout, "Reading %d elements\n", count);


    for(i = 0; i < count; i++){

        aggfile_t *current = table->files + i;
        int j;
        char buf[LODSIZENAME];

        /* finding element name */
        fread(buf, LODSIZENAME, 1, fd_data);

        buf[LODSIZENAME-1]='\0';

        for(j = 0; j < LODSIZENAME; j++) {
            buf[j] = tolower(buf[j]);
        }

        strcpy(current->name, buf);

        /* finding other elements of the header */

        fread(&current->offset, sizeof(uint32_t), 1, fd_data);
        current->offset = le32toh(current->offset);

        fread(&current->size, sizeof(uint32_t), 1, fd_data);
        current->size = le32toh(current->size);

        // we don't need next integer (4 bytes)
        fseek(fd_data, 4, SEEK_CUR);

        fread(&current->length, sizeof(uint32_t), 1, fd_data);
        current->length = le32toh(current->length);
    }

    return table;
}






/**
 * Create the files in the given directory.
 * \return the number of extracted files
 */
int extract_files(FILE *fd_data, char *output_dir, aggtable_t *table){

    int total = 0; /* number of elements extracted */
    int i; /* files counter */

    if(create_dir(output_dir)) {
        fprintf(stderr, "Directory %s was not created - Aborting extraction\n", output_dir);
        return 0;
    }

    /* reading and creating files for $count elements in data file */
    fprintf(stdout, "Extracting %d elements\n", table->count);

    for(i = 0; i < table->count; i++)
    {
        aggfile_t *file  = table->files + i;

        if(!write_file(fd_data, file, output_dir)) {
            total++;
        }
   }

    return total;
}


