
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "logger/uulog.h"
#define LOG_TAG "string-easy"

char *string_easy_trim(char *str)
{
    char *s = str;
    char *p;
    while (*s == ' ' || *s == '\t') {
        s += 1;
    }
    p = s;

    while (*p != '\0' && *p != '\r' && *p != '\n' && *p != ' ' && *p != '\t') {
        p += 1;
    }
    *p = 0;
    return s;
}

char **string_easy_split(char *str, const char *delim, int *num)
{
    char *buf = NULL;
    int size = 0;
    int offset = 0;

    char **_array = NULL;
    char *_str= NULL;

    char *ptr;
    int i;
    int _num = 0;

    *num = 0;
    if (str == NULL || strlen(str) == 0) {
        return NULL;
    }

    _num = 1;
    ptr = strstr(str, delim);
    while (NULL != ptr) {
        _num += 1;
        ptr += strlen(delim);
        ptr = strstr(ptr, delim);
    }

    size = sizeof(char *) * (_num + 1)
           + strlen(str) + 1;
    buf = malloc(size);
    if (buf == NULL) {
        return NULL;
    }
    memset(buf, 0, size);

    offset = 0;
    _array = (char **)(buf + offset);
    offset += sizeof(char *) * (_num + 1);
    _str = buf + offset;

    memcpy(_str, str, strlen(str));

    i = 0;
    ptr = strtok(_str, delim);
    while (ptr != NULL && i < _num) {
        _array[i] = ptr;
        i += 1;
        ptr = strtok(NULL, delim);
    }
    _num = i;

    *num = _num;
    return _array;
}
