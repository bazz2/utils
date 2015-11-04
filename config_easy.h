#ifndef _CONFIG_EASY_H_
#define _CONFIG_EASY_H_

int config_easy_get(const char *file, const char *key, char *value, unsigned int len);
int config_easy_get_integer(const char *file, const char *key, int *value);
int config_easy_get_float(const char *file, const char *key, float *value);

#endif
