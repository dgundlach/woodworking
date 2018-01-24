/* Arithmetic sum from 1 to n: n(n-1)/2 */
#define sum1ToN(n) (((n) * ((n) + 1)) / 2)

/* Sum of powers from 1 to n: (x^n-1)/(x-1)  The equation is undefined for x=1,
       so just return n (1 + 1^1 + ... + 1^(n-1) = n). */
#define sumOfConsecutivePowers(x, n) (x == 1 ? n : (pow(x, n) - 1) / (x - 1))

/* Round a fractional number to the nearest unit of precision */
#define roundFraction(f,p) (round(f * p) / p)

/* Find the nth number in the Fibonacci sequence */
#define fibonacci(n) ((pow((1 + sqrt(5)) / 2, n) - pow((1 - sqrt(5)) / 2, n) / sqrt(5)))

/* Find the sum of the first n Fibonacci numbers */
#define sumOfFibonacciNumbers(n) (fibonacci(n + 2) - 1)

/* Radius of an arc given width and height */
#define radius(w,h) (h / 2 + (w * w) / (8 * h))

/* Angle given number of sides */
#define angle(s) (M_PI / s)

/* Miter angle given number of sides */
#define miter(s) (M_PI / (s * 2))

/* Convert radians to degrees */
#define degrees(r) (r * (180 / M_PI))

/* Convert degrees to radians */
#define radians(a) (a * (M_PI / 180))

/* Length of chord given diameter and number of sides */
#define chord(d,s) ((2 * d) * sin(miter(s)))

/* Find side b of a right triangle given a and angle A */
#define sideB(A, a) (a * tan(A))

#define lawOfCosinesAngle(a, b, c) acos((a * a + b * b - c * c) / (2 * a * b))
#define lawOfCosinesSide(A, b, c) sqrt(b * b + c * c + (2 * b * c * cos(A)))
#define lawOfSinesAngle(B, b, c) asin((sin(B) / b) * c)
#define lawOfSinesSide(A, B, b) ((sin(A) * b) / sin(B))

/* "AAA" is when we know all three angles of a triangle, but no sides. */
#define solveAAATriangle(A, B, C, a, b, c) \
    if (a) { \
      solveAASTriangle(B, A, a, C, c, b); \
    } else if (b) { \
      solveAASTriangle(A, B, b, C, c, a); \
    } else if (c) { \
      solveAASTriangle(A, C, c, B, b, a); \
    } else { \
      if (A <= B && A <= C) { \
        a = 1; \
        solveAASTriangle(B, A, a, C, c, b); \
      } else if (B <= A && B <= C) { \
        b = 1; \
        solveAASTriangle(A, B, b, C, c, a); \
      } else { \
        c = 1; \
        solveAASTriangle(A, C, c, B, b, a); \
      } \
    }

/* "AAS" is when we know two angles and one side (which is not between the angles). */
#define solveAASTriangle(A, C, c, B, b, a) \
    B = M_PI - A - C; \
    a = lawOfSinesSide(A, C, c); \
    b = lawOfSinesSide(B, C, c);

/* "ASA" is when we know two angles and a side between the angles. */
#define solveASATriangle(A, B, c, C, a, b) \
    C = M_PI - A - B;\
    a = lawOfSinesSide(A, C, c); \
    b = lawOfSinesSide(B, C, c);

/* "SAS" is when we know two sides and the angle between them. */
#define solveSASTriangle(A, b, c, B, C, a) \
    a = lawOfCosinesSide(A, b, c); \
    if (b < c) { \
      B = lawOfSinesAngle(A, a, b); \
      C = M_PI - A - B; \
    } else { \
      C = lawOfSinesAngle(A, a, c); \
      B = M_PI - A - C; \
    }

/* "SSA" is whean we know two sides and an angle that is not the angle between the sides. */
#define solveSSATriangle(c, B, b, A, a, C, A2, a2, C2) \
    C = lawOfSinesAngle(B, b, c); \
    A = M_PI - B - C; \
    a = lawOfSinesSide(A, B, b); \
    C2 = M_PI - C; \
    A2 = M_PI - B - C2; \
    a2 = lawOfSinesSide(A2, B, b);

/* "SSS" is when we know three sides of the triangle, and want to find the missing angles. */
#define solveSSSTriangle(a, b, c, A, B, C) \
    A = lawOfCosinesAngle(b, c, a); \
    B = lawOfCosinesAngle(a, c, b); \
    C = M_PI - A - B;

#define meanProportional(a, b) sqrt(a * b)
