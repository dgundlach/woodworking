#include <math.h>
#include "measurement.h"
#include "formulas.h"
#include "BitCount.h"

#ifdef DEBUG_TRIANGLE_C
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#endif

typedef struct triangle {
  double angles[3];
  double sides[3];
  double base;
  double height;
} triangle;

int calculateBaseHeight(triangle *tri) {

  int C, i;

  /* Check if triangle is a right triangle */

  tri->base = 0;
  tri->height = 0;
  for (i=0; i<3; i++) {
    if (tri->angles[i] == M_PI_2) {
      C = i;
      tri->height = tri->sides[(C + 1) % 3];
      break;
    }
  }
  if (tri->height == 0) {

    /* Triangle is not a right triangle.  Create one within the current triangle.
       All angles on the triangle must be right or acute, so if A is obtuse, use
       B as A and A as C. Height will be the side opposite A in the new
       triangle. Use the law of sines to calculate it. */

    C = (tri->angles[0] < M_PI_2) ? 2 : 0;
    // a = lawOfSinesSide(A, C, c)
    tri->height = lawOfSinesSide(tri->angles[(C + 1) % 3], M_PI_2, tri->sides[C]);
  }
  tri->base = tri->sides[(C + 2) % 3];
  return 0;
}

int solveTriangle(triangle *tri, triangle *alt) {

  int i, angleCount=0, sideCount=0;
  int angle1 = -1, angle2, side1 = -1, side2, undef = -1, opposite = -1;
  int powerOfTwo;
  int gmask = 0;

  powerOfTwo = 1;
  for (i=0; i<3; i++) {
    if (tri->angles[i] != 0) {
      gmask = gmask | powerOfTwo;
      angleCount++;
      if (opposite == -1 && tri->sides[i] != 0) {
        opposite = i;
      } else {
        if (angle1 == -1) {
          angle1 = i;
        } else {
          angle2 = i;
        }
      }
    }
    if (tri->sides[i] != 0) {
      gmask = gmask | powerOfTwo;
      sideCount++;
      if (opposite != i) {
        if (side1 == -1) {
          side1 = i;
        } else {
          side2 = i;
        }
      }
    } else {
      if (tri->angles[i] == 0) {
        undef = i;
      }
    }
    powerOfTwo = powerOfTwo << 1;
  }
  if ((angleCount + sideCount) != 3) {
    return -1;
  }
  switch (angleCount) {
    case 0:
      // SSS
      solveSSSTriangle(tri->sides[0], tri->sides[1], tri->sides[2],
                      tri->angles[0], tri->angles[1], tri->angles[2]);
      break;
    case 1:
      if (gmask == 7) {
        // SAS (angle1, side1, side2, side1, side2, angle1)
        solveSASTriangle(tri->angles[angle1], tri->sides[side1],
                         tri->sides[angle2], tri->angles[side1],
                         tri->angles[side2], tri->angles[angle1]);
      } else {
        // SSA (side1, opposite, opposite, undef, undef, side undef+3 undef+3 side+3)
        for (i=0; i<3; i++) {
          alt->angles[i] = tri->angles[i];
          alt->sides[i] = tri->sides[i];
        }
        solveSSATriangle(tri->sides[side1], tri->angles[opposite],
                         tri->sides[opposite], tri->angles[undef],
                         tri->sides[undef], tri->angles[side1],
                         alt->angles[undef], alt->sides[undef],
                         alt->angles[side1]);
        calculateBaseHeight(alt);
      }
      break;
    case 2:
      if (gmask == 7) {
        // ASA (angle1, angle2, side1, side1, angle1, angle2)
        solveASATriangle(tri->angles[angle1], tri->angles[angle2],
                         tri->sides[side1], tri->angles[side1],
                         tri->sides[angle1], tri->sides[angle2]);
      } else {
        // AAS (angle1, opposite, opposite, undef, undef, angle1)
        solveAASTriangle(tri->angles[angle1], tri->angles[opposite],
                         tri->sides[opposite], tri->angles[undef],
                         tri->sides[undef], tri->sides[angle1]);
      }
      break;
    case 3:
      // AAA
      solveAAATriangle(tri->angles[0], tri->angles[1], tri->angles[2],
                       tri->sides[0], tri->sides[1], tri->sides[2]);
      break;
  }
  calculateBaseHeight(tri);
  return 0;
}

void printTriangle(triangle tri, int anglePrecision, int lengthPrecision,
                   int fractional) {

  char format[256];
  int i;
  int aDecimals = 0, lDecimals = 0;
  int precision;

  if (!(anglePrecision % 10)) {
    precision = anglePrecision;
    while ((precision % 10)) {
      aDecimals++;
      precision = precision / 10;
    }
  } else if ((anglePrecision != 1) && (BitCount((unsigned)anglePrecision) == 1)) {
    aDecimals = anglePrecision >> 1;
  }
  if (!(lengthPrecision % 10)) {
    precision = lengthPrecision;
    while ((precision % 10)) {
      lDecimals++;
      precision = precision / 10;
    }
  } else if ((lengthPrecision != 1) && (BitCount((unsigned)lengthPrecision) == 1)) {
    lDecimals = lengthPrecision >> 1;
  }
  if (fractional) {
    snprintf(format, 256, "%%c = %%%i.%ilf   %%c = %%s\n", aDecimals + 4, aDecimals);
    for(i=0; i<3; i++) {
      printf(format, i + 'A', roundFraction(degrees(tri.angles[i]), anglePrecision),
                     i + 'a', formatMeasurement(roundFraction(tri.sides[i], lengthPrecision), 3));
    }
  } else {
    snprintf(format, 256, "%%c = %%%i.%ilf   %%c = %%%i.%ilf\n", aDecimals + 4, aDecimals, lDecimals + 4, lDecimals);
    for(i=0; i<3; i++) {
      printf(format, i + 'A', roundFraction(degrees(tri.angles[i]), anglePrecision),
                     i + 'a', roundFraction(tri.sides[i], lengthPrecision));
    }
  }
  printf("\nBase  : %s\n", formatMeasurement(roundFraction(tri.base,lengthPrecision), 3));
  printf("Height: %s\n", formatMeasurement(roundFraction(tri.height,lengthPrecision), 3));
}

#ifdef DEBUG_TRIANGLE_C

int main(int argc, char * const argv[]) {

  int i;
  int opt;
  double val;
  triangle tri;
  triangle alt;
  int inputRadians = 0;
  int anglePrecision = 4;
  int sidePrecision = 32;
  int fractional = 0;

  for (i=0; i<3; i++) {
    tri.angles[i] = 0;
    tri.sides[i] = 0;
    alt.angles[i] = 0;
    alt.sides[i] = 0;
  }
  while ((opt = getopt(argc, argv, "A:B:C:a:b:c:fP:p:r")) != -1) {
    switch(opt) {
      case 'A':
      case 'B':
      case 'C':
         sscanf(optarg, "%lf", &val);
         tri.angles[opt - 'A'] = val;
         break;
      case 'a':
      case 'b':
      case 'c':
        tri.sides[opt - 'a'] = scanMeasurement(optarg);
        break;
      case 'f':
        fractional = 1;
        break;
      case 'P':
        anglePrecision = atoi(optarg);
        break;
      case 'p':
        sidePrecision = atoi(optarg);
        break;
      case 'r':
        inputRadians = 1;
      default:
        break;
    }
  }
  if (!inputRadians) {
    for(i=0; i<3; i++) {
      tri.angles[i] = radians(tri.angles[i]);
    }
  }

  if (!solveTriangle(&tri, &alt)) {
    printTriangle(tri, anglePrecision, sidePrecision, fractional);
    if (alt.angles[0] != 0) {
      printf("\n");
      printTriangle(alt, anglePrecision, sidePrecision, fractional);
    }
  }
  return 0;
}
#endif
