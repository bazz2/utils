#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "logger/uulog.h"
#include "utils/string_easy.h"

#define LOG_TAG "get-config"
#define MAX_STRING_LEN 10240

static int _ignore_comment(char *buffer)
{
    char *s;

    if (!buffer) {
        return -1;
    }

    s = buffer;
    while (*s) {
        if (*s == '#') {
            *s = 0;
            break;
        }
        s++;
    }

    return 0;
}

static int _ignore_blank(char *buffer, int size)
{
    char *s;
    int in_quote = 0;
    char fore[MAX_STRING_LEN] = {0};
    int i = 0;

    if (!buffer) {
        return -1;
    }

    s = buffer;
    i = 0;
    while (*s) {
        if (*s == '\n' || *s == '\r') {
            *s = 0;
            break;
        }

        if (*s == '\"') {
            if (in_quote == 0) {
                in_quote = 1;
                s++;
                continue;
            } else {
                *s = 0;
                break;
            }
        }

        if (*s == ' ') {
            if (in_quote == 1) {
                fore[i++] = *s;
            }
        } else {
            fore[i++] = *s;
        }
        s++;
    }
    snprintf(buffer, size, "%s", fore);

    return 0;
}

#if 0
static char *_string_easy_trim(char *str)
{
    char *s = str;
    char *p;
    while (*s == ' ' || *s == '\t') {
        s += 1;
    }
    p = s;

    while (*p != '\0' && *p != '\r' && *p != '\n' && *p != '\t') {
        p += 1;
    }
    *p = 0;
    return s;
}
#endif

/**
 * @param[in] file: full path of the profile
 * @param[in] key: the name of config
 * @param[out] value: the value of config
 * @param[in] len: max length of value's memory
 * @return: 0 - success; -1 - failed.
 */
int config_easy_get(const char *file, const char *key, char *value, unsigned int len)
{
    FILE *fp;
    char buffer[MAX_STRING_LEN + 1], key_tmp[MAX_STRING_LEN + 1];
    char *s;
    int get = -1;

    if (!file || !key || !value) {
        LOGE(LOG_TAG, "Invalid parameter: file(%p), key(%p), value(%p)",
             file, key, value);
        return -1;
    }
    if (len > MAX_STRING_LEN) {
        LOGE(LOG_TAG, "Invalid parameter: 'len(%d)' is too long", len);
        return -1;
    }

    fp = fopen(file, "r");
    if (!fp) {
        LOGE(LOG_TAG, "Failed to open file: %s", file);
        return -1;
    }

    while (fgets(buffer, MAX_STRING_LEN + 1, fp)) {
        _ignore_comment(buffer);
        _ignore_blank(buffer, sizeof (buffer));
        snprintf(key_tmp, sizeof(key_tmp), "%s=", key);
        s = strstr(buffer, key_tmp);
        if (!s) {
            continue;
        }

        s += strlen(key_tmp);
        if (!*s) {
            continue;
        }

        //s = _string_easy_trim(s);
        get = 0; /* success */
        break;
    }
    if (get == 0) {
        snprintf(value, len, "%s", s);
    }

    fclose(fp);
    return get;
}

int config_easy_get_integer(const char *file, const char *key, int *value)
{
    int ret = 0;
    char tmp[MAX_STRING_LEN];

    *value = 0;
    ret = config_easy_get(file, key, tmp, sizeof(tmp));
    if (ret != 0) {
        return -1;
    }

    *value = atoi(tmp);
    return 0;
}

int config_easy_get_float(const char *file, const char *key, float *value)
{
    int ret = 0;
    char tmp[MAX_STRING_LEN];

    *value = 0;
    ret = config_easy_get(file, key, tmp, sizeof(tmp));
    if (ret != 0) {
        return -1;
    }

    *value = atof(tmp);
    return 0;
}
