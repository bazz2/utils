/* minigzip.c -- simulate gzip using the zlib compression library
 * Copyright (C) 1995-2006, 2010, 2011 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/*
 * minigzip is a minimal implementation of the gzip utility. This is
 * only an example of using zlib and isn't meant to replace the
 * full-featured gzip. No attempt is made to deal with file systems
 * limiting names to 14 or 8+3 characters, etc... Error checking is
 * very limited. So use minigzip only for testing; use gzip for the
 * real thing. On MSDOS, use only on file names without extension
 * or in pipe mode.
 */

/* @(#) $Id$ */

#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "logger/uulog.h"

#define TAG "gzip"
#define GZ_SUFFIX ".gz"
#define SUFFIX_LEN (sizeof(GZ_SUFFIX)-1)

#define BUFLEN      16384
#define MAX_NAME_LEN 1024

/**
 * Compress input to output then close both files.
 */

void gz_compress(FILE *in, gzFile out)
{
    char buf[BUFLEN];
    int len;

    for (;;) {
        len = (int)fread(buf, 1, sizeof(buf), in);
        if (ferror(in)) {
            LOGE(TAG, "Failed to fread");
            break;
        }
        if (len == 0)
            break;

        if (gzwrite(out, buf, (unsigned)len) != len){
            LOGE(TAG, "Failed to gzwrite");
            break;
        }
    }
}


/**
 * Uncompress input to output then close both files.
 */
void gz_uncompress(gzFile in, FILE *out)
{
    char buf[BUFLEN];
    int len;

    for (;;) {
        len = gzread(in, buf, sizeof(buf));
        if (len < 0) {
            LOGE(TAG, "Failed to gzread");
            break;
        }
        if (len == 0)
            break;

        if ((int)fwrite(buf, 1, (unsigned)len, out) != len) {
            LOGE(TAG, "Failed to fwrite");
            break;
        }
    }
}


/**
 * Compress the given file: create a corresponding .gz file and remove the
 * original.
 */
int file_compress(char *file, char *mode)
{
    char outfile[MAX_NAME_LEN] = {0};
    FILE  *in;
    gzFile out;

    if (file == NULL || mode == NULL) {
        LOGE(TAG, "Invalid input: %p, %p", file, mode);
        return -1;
    }

    snprintf(outfile, sizeof(outfile), "%s%s", file, GZ_SUFFIX);

    in = fopen(file, "rb");
    if (in == NULL) {
        LOGE(TAG, "Failed to open %s: %s", file, strerror(errno));
        return -1;
    }
    out = gzopen(outfile, mode);
    if (out == NULL) {
        LOGE(TAG, "Failed to gzopen %s", outfile);
        fclose(in);
        return -1;
    }
    gz_compress(in, out);

    fclose(in);
    gzclose(out);

    unlink(file);
    return 0;
}


/**
 * Uncompress the given file and remove the original.
 */
int file_uncompress(char *file)
{
    char buf[MAX_NAME_LEN];
    char *infile, *outfile;
    FILE  *out;
    gzFile in;
    size_t len = strlen(file);

    if (file == NULL) {
        LOGE(TAG, "Invalide input: %p", file);
        return -1;
    }

    snprintf(buf, sizeof(buf), "%s", file);

    if (len > SUFFIX_LEN && strcmp(file+len-SUFFIX_LEN, GZ_SUFFIX) == 0) {
        infile = file;
        outfile = buf;
        outfile[len-3] = '\0';
    } else {
        outfile = file;
        infile = buf;
        snprintf(buf + len, sizeof(buf) - len, "%s", GZ_SUFFIX);
    }
    in = gzopen(infile, "rb");
    if (in == NULL) {
        LOGE(TAG, "Failed to gzopen %s", infile);
        return -1;
    }
    out = fopen(outfile, "wb");
    if (out == NULL) {
        LOGE(TAG, "Failed to fopen %s", outfile);
        gzclose(in);
        return -1;
    }

    gz_uncompress(in, out);

    gzclose(in);
    fclose(out);
    unlink(infile);

    return 0;
}
