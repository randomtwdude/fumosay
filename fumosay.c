// -- fumosay v0.6 --
// like cowsay, but with funky fumos!
// Tagline of the version: Mystia's Izakaya is a great game.

/* ===== INCLUDES ===== */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

/* ===== DEFINES ===== */
#define FUMO_COUNT 8
int MAX_WIDTH = 80;
// for some enum to string actions
#define FUMOS(FUMO)                                                     \
        FUMO(Reimu) FUMO(Patchy) FUMO(Marisa) FUMO(Flandre)             \
        FUMO(Joon)  FUMO(Koishi) FUMO(Kaguya) FUMO(Mokou)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum fumo_who { FUMOS(GENERATE_ENUM) };
static const char *FUMO_STRING[] = { FUMOS(GENERATE_STRING) };

#define COLOR(r,g,b) "\033[38;2;" #r ";" #g ";" #b "m"
#define RESET_COLOR "\033[0m"

/* ===== FUNCTIONS ===== */
#define MIN(a,b) (a > b ? b : a)
#define MAX(a,b) (a > b ? a : b)

void helpInfo() {
  printf("=== fumosay ver. 0.6 ===\n"
         "Usage: fumosay [-hngc] [-W column] [-f character] (message)\n"
         "-n     Disables word-wrapping.\n"
         "-W     Specifies roughly where the message should be wrapped.\n"
         "       The default is 80.\n"
         "       This option is ignored with -n.\n"
         "-f     Pick a fumo by name.\n"
         "-g     Displays the character's name above the text box like a story game.\n"
         "-c     Adds colours to the text.\n"
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
  // allocate buffer
  str = realloc(NULL, sizeof(*str) * size);
  if (!str) {
    return str;
  }
  while ((ch = fgetc(st)) != EOF) {
    if (ch == ' ' || ch == 9 /* TAB */) {
      if (len) {
        break; // if we have a word, spaces terminate
      } else {
        continue; // ignore consecutive spaces
      }
    }
    // append new character
    str[len++] = ch;
    // extend the buffer if necessary
    if (len == size) {
      str = realloc(str, sizeof(*str) * (size += 16));
      if (!str) {
        return str;
      }
    }
    if (ch == '\n') {
      break;
    }
  } // while
  str[len++] = '\0';
  return realloc(str, sizeof(*str) * len);
}

// pick a fumo
int fumo_picker(char *str) {
  char f = str[0];
  if (f == 'r' || f == 'R') {
    return Reimu;
  }
  if (f == 'p' || f == 'P') {
    return Patchy;
  }
  if (f == 'm' || f == 'M') {
    if (strlen(str) < 2) {
      return -1;
    }
    if (str[1] == 'a' || str[1] == 'A') {
      return Marisa;
    } else if (str[1] == 'o' || str[1] == 'O') {
      return Mokou;
    }
  }
  if (f == 'f' || f == 'F') {
    return Flandre;
  }
  if (f == 'j' || f == 'J') {
    return Joon;
  }
  if (f == 'k' || f == 'K') {
    if (strlen(str) < 2) {
      return -1;
    }
    if (str[1] == 'o' || str[1] == 'O') {
      return Koishi;
    } else if (str[1] == 'a' || str[1] == 'A') {
      return Kaguya;
    }
  }
  return -1;
}

// set a terminal color for each fumo
void fumo_colour(enum fumo_who fm) {
  switch (fm) {
  case Reimu:
    printf(COLOR(245,110,120));
    break;
  case Patchy:
    printf(COLOR(175,115,240));
    break;
  case Marisa:
    printf(COLOR(245,210,120));
    break;
  case Flandre:
    printf(COLOR(245,120,145));
    break;
  case Joon:
    printf(COLOR(240,120,240));
    break;
  case Koishi:
    printf(COLOR(200,245,125));
    break;
  case Kaguya:
    printf(COLOR(240,150,130));
    break;
  case Mokou:
    printf(COLOR(245,100,100));
    break;
  }
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
  case Joon:
    fputs(joon, stdout);
    break;
  case Koishi:
    fputs(koishi, stdout);
    break;
  case Kaguya:
    fputs(kaguya, stdout);
    break;
  case Mokou:
    fputs(mokou, stdout);
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

// print some spaces and the right parenthesis
void paddedBreak(int padding) {
  for (int i = 0; i < padding; i++) {
    printf(" ");
  }
  printf(")\n");
}

// greedy algorithm
// TODO: use something better
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

// parse command line arguments
void parseArgument(
  int argc, char **argv, bool *no_wrap,
  bool *display_name, bool *colour, enum fumo_who *fm
  ) {
  char opt;
  while ((opt = getopt(argc, argv, "hngcW:f:")) != -1) {
    switch (opt) {
    case 'h':
      helpInfo();
      exit(EXIT_SUCCESS);
    case 'n':
      MAX_WIDTH = 100000000;
      *no_wrap = true;
      break;
    case 'W':
      MAX_WIDTH = *no_wrap ? MAX_WIDTH : numberize(optarg);
      break;
    case 'f':
      *fm = fumo_picker(optarg);
      if (*fm == -1) {
        printf("\"%s\"??? Choosing randomly...\n", optarg);
      }
      break;
    case 'g':
      *display_name = true;
      break;
    case 'c':
      *colour = true;
      break;
    } // switch
  }
}

int main(int argc, char **argv) {
  // init
  srand(time(NULL));
  bool no_wrap = false;
  bool display_name = false;
  bool colour = false;
  enum fumo_who fm = -1;

  // argument
  parseArgument(argc, argv, &no_wrap, &display_name, &colour, &fm);

  if (MAX_WIDTH < 2) {
    printf("The fumos say: Width must be at least 2!\n");
    exit(EXIT_FAILURE);
  }

  int word_count = 0;
  char **word_vector = realloc(NULL, sizeof(char *));
  if (optind == argc) {
    // read from stdin instead
    if (no_wrap) {
      char *buffer = NULL;
      size_t buf_line, line;
      while (getline(&buffer, &buf_line, stdin) > -1) {
        char *token;
        FILE *st = open_memstream(&token, &line);
        for (int i = 0; i < buf_line; i++) {
          if (buffer[i] == 9) {
            fprintf(st, "    "); // replace tab with 4 spaces
          } else {
            fprintf(st, "%c", buffer[i]);
          }
        }
        fflush(st);
        word_vector[word_count++] = token;
        word_vector = realloc(word_vector, (word_count + 1) * sizeof(char *));
      }
      word_vector = realloc(word_vector, word_count * sizeof(char *));
    } else {
      while (1) {
        char *token;
        token = getInput(stdin, 10);
        if (token == NULL) {
          printf("Something has gone terribly wrong!\n");
          exit(EXIT_FAILURE);
        }
        if (strlen(token) == 0) {
          break;
        }
        word_vector[word_count++] = token;
        word_vector = realloc(word_vector, (word_count + 1) * sizeof(char *));
      }
      word_vector = realloc(word_vector, word_count * sizeof(char *));
    }
  } else {
    // read from cmd line
    word_count = argc - optind;
    word_vector = argv + optind;
  }

  size_t bubble_width = longestLineWidth(word_count, word_vector) + 1;

  // choose a fumo if not already chosen
  if (fm == -1) {
    fm = rand() % FUMO_COUNT;
  }
  // color
  if (colour) {
    fumo_colour(fm);
  }
  // CYOA mode
  if (display_name) {
    printf("%s says:\n", FUMO_STRING[fm]);
  }
  // top line
  fputc(' ', stdout);
  for (int i = 0; i < bubble_width; i++) {
    fputc('_', stdout);
  }
  fputc('\n', stdout);
  // message
  printMessage(word_count, word_vector, bubble_width - 1);
  // bottom line
  fputc(' ', stdout);
  for (int i = 0; i < bubble_width; i++) {
    fputc('-', stdout);
  }
  // reset color
  if (colour) {
    printf(RESET_COLOR);
  }
  // fumo!
  fumofumo(fm);
  return 0;
}
