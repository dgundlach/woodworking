
// [sign]<integer>
// [sign]<integer>.<integer>
// [sign]<integer>-<integer>/<integer>
// [sign]<integer>/<integer>

enum token(word, hyphen, number, dot, slash, err);

typedef union Value {
  char *str;
  long val;
}

typedef struct Token {
  enum token tok;
  Value data;
} Token;

Token gettok(char **str) {

  Token t;
  char *s;

  if (isalpha(**str)) {
    t.tok = word;
    t.data.str = s = malloc(strlen(*str) + 1);
    while (**str && isalpha(**str)) {
      *s++ = **str++;
    }
  }
}

int scanFraction(char *str, double *val) {

  enum token {hyphen, number, dot, slash, err};
  enum state {};
  enum state s;
  double mul = 1;
  long mantissa;
  long numerator;
  long denominator;

  val = 0;
  while (*str && (*str == ' ')) {
    str++;
  }
  if (!*str) {
    return 0;
  }
  if (*str == '-') {
    mul = -1;
    str++;
  }
s = start;
switch (*s) {
  case '-':
    s = sign;

}
