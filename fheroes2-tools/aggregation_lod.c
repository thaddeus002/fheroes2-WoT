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

// TODO Transform this in doc


typedef struct {
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
} h3lod;






