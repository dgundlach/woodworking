#include <stdio.h>
#include <string.h>
#include <math.h>
#include <pcre.h>
#include <stdlib.h>

// aaa

inline size_t pcre_vector_size(const char *re) {

  size_t rv = 3;

  if (re) {
    while (*re) {
      if (*re++ == '(') {
        rv += 3;
      }
    }
  }
  return rv;
}

int scanMeasurement(char *str, double *val) {

  char *re = "^\\s*(\\d+)(/(\\d+)|\\.(\\d+)|-(\\d+)/(\\d+))?\\s*$";
  static pcre *r = NULL;
  const char *error;
  int erroffset;
  static int *ovector;
  static size_t ovectorSize;
  int rc;
  const char **components;
  int result = -1;
  double rv;
  double denominator;

  if (!r) {
    r = pcre_compile(re, PCRE_DOTALL, &error, &erroffset, NULL);
    ovectorSize = pcre_vector_size(re);
    ovector = malloc(sizeof(int) * ovectorSize);
  }
  rc = pcre_exec(r, NULL, str, strlen(str), 0, 0, ovector, ovectorSize);
  if (rc > 0) {
    pcre_get_substring_list(str, ovector, rc, &components);
    rv = strtod(components[1], NULL);
    switch (rc) {
      case 2:
        result = 0;
        break;
      case 4:
        denominator = strtod(components[3], NULL);
        if (denominator != 0) {
          rv = rv / strtod(components[3], NULL);
          result = 0;
        }
        break;
      case 5:
        rv = rv + strtod(components[4], NULL) / pow(10, strlen(components[4]));
        result = 0;
        break;
      case 7:
        denominator = strtod(components[6], NULL);
        if (denominator != 0) {
          rv = rv + strtod(components[5], NULL) / denominator;
          result = 0;
        }
    }
    pcre_free_substring_list(components);
  }
  if (!result) {
    *val = rv;
  }
  return result;
}

int main(int argc, char **argv) {

  double val;
  char *values[] = {"10", "10.4", "5/16", "10-5/16", NULL};
  int i;

  for (i=0; values[i]; i++) {
    scanMeasurement(values[i], &val);
    printf("%10s %f\n", values[i], val);
  }
  return 0;
}
