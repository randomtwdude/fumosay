// fumosay
// like cowsay, but with funky fumos!
// fairly barebones for now, more updates to come.

/* ===== INCLUDES ===== */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

/* ===== DEFINES ===== */
#define FUMO_COUNT 4
int MAX_WIDTH = 80;
enum fumo_who {Reimu, Patchy, Marisa, Flandre};

/* ===== FUNCTIONS ===== */
#define MIN(a,b) (a > b ? b : a)
#define MAX(a,b) (a > b ? a : b)

void helpInfo() {
  printf("=== fumosay ver. 0.4 ===\n"
         "Usage: fumosay [-hn] [-W column] *message*\n"
         "-n     Disables word-wrapping.\n"
         "-W     Specifies roughly where the message should be wrapped.\n"
         "       The default is 80.\n"
         "In spirit of the original cowsay, made by Tony Monroe in 1999.\n"
         "Fumos are characters from Touhou Project.\n");
}

// string to number
int numberize(char *c) {
  long number = strtol(c, (char **)NULL, 0);
  if (number == 0) {
    fprintf(stderr, "Invalid parameter: %s", c);
    exit(EXIT_FAILURE);
  }
  return (int)number;
}

// get one word
char *getInput(FILE *st, size_t size) {
  char *str;
  int ch;
  size_t len = 0;
  str = realloc(NULL, sizeof(*str) * size);
  if (!str) {
    return str;
  }
  while (1) {
    ch = fgetc(st);
    if (ch == ' ' || ch == 9 /* TAB */) {
      if (len) {
        break;
      } else {
        continue;
      }
    } else if (ch == '\n') {
      str[len++] = ch;
      if (len == size) {
        str = realloc(str, sizeof(*str) * (size += 16));
        if (!str) {
          return str;
        }
      } // extend
      break;
    } else if (ch == EOF) {
      break;
    } // if

    // append new character
    str[len++] = ch;
    // extend the string if necessary
    if (len == size) {
      str = realloc(str, sizeof(*str) * (size += 16));
      if (!str) {
        return str;
      }
    }
  } // while
  str[len++] = '\0';
  return realloc(str, sizeof(*str) * len);
}

#include "fumo.fumo"
// fumo!
void fumofumo(enum fumo_who fm) {
  switch (fm) {
  case Reimu:
    fputs(reimu, stdout);
    break;
  case Patchy:
    fputs(patchouli, stdout);
    break;
  case Marisa:
    fputs(marisa, stdout);
    break;
  case Flandre:
    fputs(flandre, stdout);
    break;
  }
}

// returns length of longest line
size_t longestLineWidth(int argc, char **argv) {
  size_t cur_line = 0;
  size_t max_line = 0;
  bool line_break;

  for (int i = 0; i < argc; i++) {
    line_break = false;
    size_t word_len = strlen(argv[i]) + 1;
    if (argv[i][word_len - 2] == '\n') {
      word_len -= 1;
      line_break = true;
    }

    // if word is longer than max, return max
    if (word_len > MAX_WIDTH) {
      return MAX_WIDTH;
    }
    if (cur_line + word_len < MAX_WIDTH) {
      // add this word
      cur_line += word_len;
      max_line = MAX(cur_line, max_line);
      if (line_break) {
        cur_line = 0;
      }
    } else {
      // count a new line
      max_line = MAX(cur_line, max_line);
      if (line_break) {
        max_line = MAX(word_len, max_line);
        cur_line = 0;
      } else {
        cur_line = word_len;
      }
    }
  }
  return max_line;
}

void paddedBreak(int padding) {
  for (int i = 0; i < padding; i++) {
    printf(" ");
  }
  printf(")\n");
}

void printMessage(int argc, char **argv, size_t bubble_size) {
  size_t cur_line = 0;
  bool line_break = false;

  for (int i = 0; i < argc; i++) {


    line_break = false;
    size_t word_len = strlen(argv[i]) + 1;
    if (argv[i][word_len - 2] == '\n') {
      argv[i][word_len - 2] = 0; // strip newline
      word_len -= 1;
      line_break = true;
    }

    // left edge
    if (cur_line == 0) {
      printf("( ");
    }

    // word fits
    if (cur_line + word_len <= bubble_size) {
      printf("%s ", argv[i]);
      if (cur_line + word_len == bubble_size) {
        // terminate this line if max length
        paddedBreak(0);
        cur_line = 0;
        continue;
      } else if (i == argc - 1) {
        // terminate all if last word
        paddedBreak(bubble_size - cur_line - word_len);
        break;
      } else if (line_break) {
        // terminate by newline
        paddedBreak(bubble_size - cur_line - word_len);
        cur_line = 0;
        continue;
      }
      cur_line += word_len;
      continue;
    }
    // word doesn't fit this line
    // break if line isn't empty
    if (cur_line > 0) {
      paddedBreak(bubble_size - cur_line);
      printf("( ");
    }
    // now we're in a new line
    if (word_len > bubble_size) {
      // word is too long for a single line
      char *word = argv[i];
      size_t out = 0;
      for (int j = 0;; j++) {
        size_t len = MIN(bubble_size - 1 /* leave a blank for looks */, strlen(word));
        printf("%.*s", (int)len, word);
        word += len;
        out += len;
        cur_line = len;
        if (out >= word_len - 1) {
          if (line_break) {
            paddedBreak(bubble_size - cur_line);
            cur_line = 0;
          } else {
            printf(" ");
            cur_line += 1;
          }
          break;
        } else {
          paddedBreak(1);
        }
        printf("( ");
      } // for
      if (line_break) {
        cur_line = 0;
      }
    } else {
      // word fits in a single line
      printf("%s ", argv[i]);
      cur_line = word_len;
      if (i == argc - 1) {
        paddedBreak(bubble_size - cur_line);
        break;
      }
      if (line_break) {
        paddedBreak(bubble_size - cur_line);
        printf("( ");
        paddedBreak(bubble_size);
        cur_line = 0;
      }
    } // word doesn't fit this line
  } // for each word
}

int main(int argc, char **argv) {
  // random init
  srand(time(NULL));
  bool no_wrap = false;
  // argument
  char opt;
  while ((opt = getopt(argc, argv, "hnW:")) != -1) {
    switch (opt) {
    case 'h':
      helpInfo();
      exit(EXIT_SUCCESS);
    case 'n':
      MAX_WIDTH = 100000000;
      no_wrap = true;
      break;
    case 'W':
      MAX_WIDTH = numberize(optarg);
      if (MAX_WIDTH < 2) {
        printf("The fumos hath spoken: Width must be at least 2!\n");
        exit(EXIT_FAILURE);
      }
      break;
    }
  }
  int word_count = 0;
  char **word_vector = realloc(NULL, sizeof(char *));
  if (optind == argc) {
    // read from stdin instead
    if (no_wrap) {
      char *token = NULL;
      size_t line;
      while (getline(&token, &line, stdin) > -1) {
        word_vector[word_count++] = token;
        word_vector = realloc(word_vector, (word_count + 1) * sizeof(char *));
        token = malloc(120 /* some reasonable numbers */);
      }
      word_vector = realloc(word_vector, word_count * sizeof(char *));
    } else {
      while (1) {
        char *token;
        token = getInput(stdin, 10);
        if (strlen(token) == 0) {
          break;
        }
        word_vector[word_count++] = token;
        word_vector = realloc(word_vector, (word_count + 1) * sizeof(char *));
      }
      word_vector = realloc(word_vector, word_count * sizeof(char *));
    }
  } else {
    word_count = argc - optind;
    word_vector = argv + optind;
  }
  size_t bubble_width = longestLineWidth(word_count, word_vector) + 1;
  // top line
  printf(" ");
  for (int i = 0; i < bubble_width; i++) {
    printf("_");
  }
  printf("\n");
  // message
  printMessage(word_count, word_vector, bubble_width - 1);
  // bottom line
  printf(" ");
  for (int i = 0; i < bubble_width; i++) {
    printf("-");
  }
  // fumo
  enum fumo_who fm = rand() % FUMO_COUNT;
  fumofumo(fm);
  // bye
  return 0;
}