#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gcd.h"
#include "measurement.h"

measurement splitMeasurement(double num) {

  measurement measure;
  int i;
  unsigned long divisor;

  measure.numerator = 0;
  measure.denominator = 1;

  /* we don't want the sign */

  num = fabs(num);

  measure.inches = (long)num;
  measure.numerator = 0;
  measure.denominator = 1;

  /* strip the whole number from the fracion */

  num = num - (double)measure.inches;

  /* strip each digit off and add it to the numerator shifted by 1 significant digit.
         increase the denominator by 1 significant digit. */

  for (i=0; (num>0)&&(i<SIGNIFICANT_DIGITS); i++) {
    num = num * 10;
    measure.numerator = measure.numerator * 10 + (long)num;
    measure.denominator = measure.denominator * 10;
    num = num - (long)num;
  }

  /* factor the fraction down to something useful. */

  divisor = gcd(measure.numerator, measure.denominator);
  measure.numerator = measure.numerator / divisor;
  measure.denominator = measure.denominator / divisor;
  return measure;
}

char *formatMeasurement(double size, int spacing) {

  static char str[256];
  char *p;
  measurement measure;
  char format[256];
  int space;

  if (size == 0.0) {
    sprintf(str, "0");
    return str;
  }
  measure = splitMeasurement(size);
  p = str;
  if (measure.inches) {
    if (spacing) {
      snprintf(format, 256, "%%%ili", spacing);
    } else {
      snprintf(format, 256, "%%li");
    }
    p += snprintf(p, 256, format, measure.inches);
  } else {
    for (space=spacing; space!=0; space--) {
      *p++ = ' ';
    }
  }
  if (measure.numerator) {
    if (measure.inches) {
      *p++ = '-';
    } else if (spacing) {
      *p++ = ' ';
    }
    p += snprintf(p, 256, "%li/%li", measure.numerator, measure.denominator);
  }
  return str;
}

double scanMeasurement(char *measure) {

  double measurement = 0;
  int inches, numerator, denominator;

  /* Scan the string measure for a valid measurement.  A valid measurement can be any
         of the following:

             * A whole number, a '-', then a fraction
             * A fraction
             * A decimal number
             * A whole number

         -1 is returned if there is an error.
  */

  if ((sscanf(measure, "%i-%i/%i", &inches, &numerator, &denominator)) == 3) {
    if (denominator) {
      measurement = (double)inches + (double)numerator / (double)denominator;
    } else {
      measurement = -1;
    }
  } else if ((sscanf(measure, "%i/%i", &numerator, &denominator)) == 2) {
    if (denominator) {
      measurement = (double)numerator / (double)denominator;
    } else {
      measurement = -1;
    }
  } else if ((sscanf(measure, "%lf", &measurement)) != 1) {
    measurement = -1;
  }
  return measurement;
}
