/***************************************************************************
 * \file aggregation_lod.c
 *
 * extract files from Heroes3 archive (*.lod)
 *
 *
 * Format Specifications :
 *
 * char {4}     - Header ("LOD\0")
 * byte {4}     - "file use flag", i.e. used as 'base' or 'extension' resource file. 200 if base resource file, 500 if extension, little endian values.
 * uint32 {4}   - Number of files
 * byte {80}    - Unknown
 *
 * // for each file
 *
 *     char {16}    - Filename (null)
 *     uint32 {4}   - File Offset
 *     uint32 {4}   - Uncompressed file size
 *     uint32 {4}   - File type(?)
 *     uint32 {4}   - File Length
 *
 *
 * byte {X}     - File Data
 *
 ***************************************************************************/


#include <sys/stat.h>
#include <sys/types.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // tolower()
#include <string.h>
#include "aggregation_lod.h"
#include "aggregation.h"

#define LODSIZENAME 16
#ifndef WITH_ZLIB
    #define WITH_ZLIB
    #include "zlib.h"
#endif

/*typedef struct {
    char name[LODSIZENAME]; // null-terminated, sometimes null-padded too, sometimes padded with, well, something after the null
    uint32_t offset; // includes the header size, no preprocessing required
    uint32_t size_original; // before compression, that is
    uint32_t type; // what's in the file - probably not used by the game directly, more on that below
    uint32_t size_compressed; // how many bytes to read, starting from offset - can be zero for stored files, use size_original in such case
} h3lod_file;


typedef struct {
    uint32_t magic; // always 0x00444f4c, that is, a null-terminated "LOD" string
    uint32_t type; // 200 for base archives, 500 for expansion pack archives, probably completely arbitrary numbers
    uint32_t files_count; // obvious
    uint8_t unknown[80]; // 80 bytes of hell knows what - in most cases that's all zeros, but in H3sprite.lod there's a bunch of seemingly irrelevant numbers here, any information is welcome
    h3lod_file files[10000]; // file list
} h3lod;*/


typedef struct
{
    char name[LODSIZENAME];
    uint32_t  offset; // includes the header size
    uint32_t  size; // before compression
    uint32_t  length; // how many bytes to read, starting from offset - can be zero for stored files, use size in such case
} lodfile_t;


/**
 *
 */
typedef struct {
    int count; /**< number of files */
    lodfile_t *files; /**< an allocated array of count files */
} lodtable_t;






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






lodtable_t *read_lodtable(FILE *fd_data) {

    int count = 0;
    lodtable_t *table;
    char archiveType[4]; /* First four bytes of file, must be LOD\0 */
    int i;

    fread(archiveType, 4, 1, fd_data);

    if(strncmp(archiveType, "LOD", 4)) {
        fprintf(stderr, "Bad file type\n");
        return NULL;
    }

    table = malloc(sizeof(lodtable_t));
    if(table==NULL) {
        fprintf(stderr, "Failed allocating memory\n");
        return NULL;
    }

    // we ignore the 4 next bytes
    fseek(fd_data, 4, SEEK_CUR);

    /* read 32 bits */
    fread(&count, sizeof(uint32_t), 1, fd_data);
    count = le32toh(count);

    table->files = malloc(sizeof(lodfile_t)*count);

    if(table->files==NULL) {
        fprintf(stderr, "Failed allocating memory\n");
        // TODO destroy table
        return NULL;
    }

    // we ignore the 80 next bytes
    fseek(fd_data, 80, SEEK_CUR);

    /* reading of $count elements in data file */
    fprintf(stdout, "Reading %d elements\n", count);


    for(i = 0; i < count; i++){

        lodfile_t *current = table->files + i;
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






int extract_lod(FILE *fd_data, char *output_dir){

    uint32_t total = 0; /* number of elements extracted */
    int i; /* files counter */
    lodtable_t *table = NULL;

    fprintf(stdout, "Extracting on %s\n", output_dir);

    table = read_lodtable(fd_data);
    if (table == NULL) return 0;


    /* reading data */
    for(i = 0; i < table->count; i++)
    {
        lodfile_t *file = table->files + i;
        int longueur; /* number of bytes to extract */
        int compress; /* are data compressed ? */
        int extractOK; /* true if the file is extract */
        char *full_name; /* full name of created file */
        FILE *fd_output; /* file descriptor for created file */
        unsigned char *body; /* file content */

        extractOK=0;

        if(file->length) {
            longueur=file->length;
            compress=1;
        } else {
            longueur=file->size;
            compress=0;
        }

        fseek(fd_data, file->offset, SEEK_SET);

        body = malloc(sizeof(unsigned char) * longueur);
        if(body == NULL) {
            fprintf(stderr, "Failed allocate memory. File %s not created.\n", file->name);
            continue;
        }

        fread(body, longueur, 1, fd_data);
        fprintf(stdout, "Lecture de %s sur %d octets -> %d octets décompressés\n", file->name, longueur, file->size);


        // creating extracted file
        full_name = malloc(sizeof(char) * (strlen(output_dir) + strlen(file->name) + 2));
        // TODO if(full_name==NULL)
        sprintf(full_name, "%s/%s", output_dir, file->name);

        fd_output = fopen(full_name, "w");
        if(fd_output != NULL) {

            if(!compress){
                fwrite(body, longueur, 1, fd_output);
                extractOK=1;
            } else {

                unsigned char *infBody;
                int err; // error code

                infBody = malloc(sizeof(unsigned char)*file->size);
                // TODO if(infBody == NULL) { ...
                err = uncompress(infBody, (uLongf *) &file->size, body, longueur);

                if(err) zerr(err);
                else {
                    fwrite(infBody, file->size, 1, fd_output);
                    extractOK=1;
                }
                free(infBody);
            }

            fclose(fd_output);

            if(extractOK) {
                fprintf(stdout, "extracted : %s\n", full_name);
                total++; /* increasing number of extracted files */
            }


        } else {
            fprintf(stderr, "Fail creating file %s\n", full_name);
        }

        free(body);
        free(full_name);
    }

    //TODO destroy table
    return total;
}

