/**
 * This is a 256 colors palette tool, for coloring HOMM images files.
 */


#ifndef PALETTE_H
#define PALETTE_H


/**
 * Read palette data from a file.
 * \param palette A 256 bytes pre-allocated table
 * \param filename The name of a pal file.
 * \return 0 in case of success. An error code otherwise
 */
int homm_init_palette(const char *filename);

/**
 * Read palette data from a file.
 * \param filename The name of a pal file.
 * \param version If 1, take in account a translation function for the palette when getting a color
 * \return 0 in case of success. An error code otherwise
 */
int homm_init_palette(const char *filename, int homm_version);


/**
 * Find a color in a palette. Default palette is used if none had been setted by homeinit_palette() function.
 * \param index The number of the color in palette. Must be >= 0 and <256
 * \return the corresponding color (transparent if number is out of range)
 */
yColor getColor(int index);


/**
 * Allocate memory to transform a table of pixels' number in a bitmap of RGB colors.
 * \param data A table of uint8_t which represent an image (palette indexes)
 * \param the size of data table
 * \return a table of RGB data, that need to be freed
 */
unsigned char *create_bitmap(unsigned char *data, int size);


/**
 * Create a picture representing the palette.
 * \param filename The name of file to create
 * \return 0 in case of success
 */
int draw_palette(const char *filename);


#endif
