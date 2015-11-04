#ifndef _GZIP_EASY_H
#define _GZIP_EASY_H

/**
 * @param[in] file: full path of a file
 * @param[in] mode: such as "wc6"
 */
int file_compress(char *file, char *mode);
/**
 * @param[in] file: full path of a file
 */
int file_uncompress(char *file);

#endif
