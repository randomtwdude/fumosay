// -- fumosay v0.8 --
// like cowsay, but with funky fumos!
// O' Great Reimu, please let me pass the course.

/* ===== INCLUDES ===== */
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

/* ===== DEFINES ===== */
int MAX_WIDTH = 80;

typedef struct color {
  short R; short G; short B;
} color;

typedef struct fumo_data {
  char  *fumo; // fumo ascii art
  char  *name; // display name
  color color;
} fumo_data;

typedef int fumo_who;

// fumo file
#include "fumo.fumo"

#define SET_COLOR(r,g,b) printf("\033[38;2;%hd;%hd;%hdm", r, g, b);
#define RESET_COLOR printf("\033[0m");

/* ===== FUNCTIONS ===== */
#define MIN(a,b) (a > b ? b : a)
#define MAX(a,b) (a > b ? a : b)

void helpInfo() {
  printf("=== fumosay ver. 0.8 ===\n"
         "Usage: fumosay [-hlngc] [-W column] [-f name] [-E expression] (message)\n"
         "-l     List all fumos.\n"
         "-n     Disable word-wrapping. Overrides -W.\n"
         "-W     Specify roughly where the message should be wrapped.\n"
         "       The default is 80.\n"
         "-f     Pick a fumo by name.\n"
         "-E     Change the expression (mouth) of the fumo.\n"
         "       Use a one-letter preset below or enter your own!\n"
         "       Presets: '1', '2', '3', '4', 'v', 'w'\n"
         "-g     Display the character's name above the text box like a story game.\n"
         "-c     Color the text.\n"
         "-h     Display this message.\n"
         "In spirit of the original cowsay, made by Tony Monroe in 1999.\n"
         "Fumos feature characters from Touhou Project.\n");
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
fumo_who fumo_picker(char *str) {
  // we want to compare the lower case version
  for (int i = 0; i < strlen(str); i++) {
    str[i] = tolower(str[i]);
  }
  fumo_who i = 0;
  while (i < FUMO_COUNT) {
    char *fm = strdup(FUMO_LIST[i].name); // lower case
    fm[0] = tolower(fm[0]); // uncapitalize first character
    if (strstr(str, fm) != NULL) {
      free(fm);
      break;
    }
    free(fm);
    i++;
  }
  // end
  if (i == FUMO_COUNT) {
    // alternate names
    #define check_name(name, id) {          \
      if (strstr(str, #name) != NULL) {     \
        return id;                          \
      }                                     \
    }

    check_name(patchy, 1);
    check_name(remi, 10);
    check_name(snae, 11);
    check_name(gap, 13);

    return -1;
  }
  return i;
}

// set fumo expression
void fumo_expr(fumo_who fm, char ex, char *custom) {
  char *p;
  short count = 0;

  // use custom expression
  if (custom[0] != 0) {
    while (count < 3) {
      p = strchr(FUMO_LIST[fm].fumo, 'E');
      if (!p) {
        return;
      }
      memmove(p, custom + (8 * count++), 8);
    }
    return;
  }

  // use presets
  #define set_expression(x) {                           \
    while (count < 3) {                                 \
      p = strchr(FUMO_LIST[fm].fumo, 'E');              \
      if (!p) {                                         \
        return;                                         \
      }                                                 \
      memmove(p, EXPRESSIONS[x] + (8 * count++), 8);    \
    }                                                   \
  }

  switch (ex) {
  case '1':
    set_expression(0); break;
  case '2':
    set_expression(1); break;
  case '3':
    set_expression(2); break;
  case '4':
    set_expression(3); break;
  case 'v':
    set_expression(4); break;
  case 'w':
    set_expression(5); break;
  default:
    while (count < 3) {
      p = strchr(FUMO_LIST[fm].fumo, 'E');
      if (!p) {
        return;
      }
      *p = ' ';
    }
  }
  #undef set_expression
}

// lists fumos
void fumo_list() {
  printf("There are %d fumos!\n-----\n", FUMO_COUNT);
  for (int i = 0; i < FUMO_COUNT; i++) {
    puts(FUMO_LIST[i].name);
  }
}

// fumo!
void fumofumo(fumo_who fm) {
  fputs(FUMO_LIST[fm].fumo, stdout);
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

// experimental minimum raggedness algo
// Based on the shortest path algo. (xxyxyz.org/line-breaking)
char **new_word_wrapper(int count, char **words, size_t width, int *linec) {

  int *offsets = calloc((count + 1), sizeof(int));
  for (int i = 1; i < count + 1; i++) {
    offsets[i] = offsets[i - 1] + strlen(words[i - 1]);
  }

  int *minima = malloc((count + 1) * sizeof(int));
  memset(minima, 0x7f, (count + 1) * sizeof(int)); // init to some large number
  minima[0] = 0;
  int *breaks = calloc((count + 1), sizeof(int));

  for (int i = 0; i < count; i++) {
    int j = i + 1;
    while (j <= count) {
      int w = offsets[j] - offsets[i] + j - i - 1;
      if (w > width) {
        break;
      }
      int cost = minima[i] + ((i > 0 && j == count) ? 0 : (width - w) * (width - w));
      if (cost <= minima[j]) {
        minima[j] = cost;
        breaks[j] = i;
      }
      j++;
    }
  }

  // output buffers
  char **lines = malloc(sizeof(char *));
  int lines_count = 0;
  int j = count;
  while (j > 0) {
    int i = breaks[j];
    char *line;
    size_t len;
    FILE *new_line = open_memstream(&line, &len);
    for (int k = i; k < j; k++) {
      fprintf(new_line, "%s ", words[k]);
    }
    fflush(new_line);
    lines[lines_count++] = line;
    lines = realloc(lines, (lines_count + 1) * sizeof(char *));
    j = i;
  }
  *linec = lines_count;
  return realloc(lines, lines_count * sizeof(char *));
}

// prints the paragraphs produced by new_word_wrapper
void output_buffer_print(char **lines, int linec, size_t bubble_width) {
  int padding;
  for (int i = linec - 1; i >= 0; i--) {
    lines[i][strcspn(lines[i], "\n")] = 0;
    printf("( %s", lines[i]);
    paddedBreak(bubble_width - strlen(lines[i]) - 1);
    free(lines[i]);
  }
}

int main(int argc, char **argv) {
  // init
  srand(time(NULL));
  bool no_wrap = false;
  bool display_name = false;
  bool colour = false;
  char expr = 0;
  char custom_expr[30] = {0};
  fumo_who fm = -1;

  // argument
  char opt;
  while ((opt = getopt(argc, argv, "hlngcE:W:f:")) != -1) {
    switch (opt) {
    case 'h':
      helpInfo();
      exit(EXIT_SUCCESS);
    case 'l':
      fumo_list();
      exit(EXIT_SUCCESS);
    case 'n':
      MAX_WIDTH = 100000000;
      no_wrap = true;
      break;
    case 'W':
      MAX_WIDTH = no_wrap ? MAX_WIDTH : numberize(optarg);
      break;
    case 'f':
      fm = fumo_picker(optarg);
      if (fm == -1) {
        printf("\"%s\"??? Choosing randomly...\n", optarg);
      }
      break;
    case 'g':
      display_name = true;
      break;
    case 'c':
      colour = true;
      break;
    case 'E':
      int optlen = strlen(optarg);
      if (optlen > 1) {
        // use custom expression
        short padding = 24 - strlen(optarg);
        if (padding < 0) {
          padding = optarg[24] = 0; // ensure non-negative padding & truncate string
        }
        sprintf(custom_expr, "%s%.*s", optarg,
                padding, "                              ");
      } else {
        expr = optarg[0];
      }
      break;
    } // switch
  }

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
        if (!token) {
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
  printf("Bubble width %d\n", bubble_width);

  // experiments
  /* int last_section = 0, cur_section = 0, linec;
  char **process = word_vector;
  char *nl;
  for (int i = 0; i < word_count; i++) {
    cur_section++;
    if ((nl = strchr(word_vector[i], '\n')) != NULL || i == word_count - 1) {
      if (nl) {
        *nl = 0; // strip the last newline
      }
      process += last_section;
      char **output = new_word_wrapper(cur_section, process, bubble_width - 2, &linec);
      output_buffer_print(output, linec, bubble_width);
      last_section = cur_section;
      cur_section = 0;
    }
  } */

  // choose a fumo if not already chosen
  if (fm == -1) {
    fm = rand() % FUMO_COUNT;
  }
  // expression
  fumo_expr(fm, expr, custom_expr);
  // color
  if (colour) {
    SET_COLOR(FUMO_LIST[fm].color.R, FUMO_LIST[fm].color.G, FUMO_LIST[fm].color.B);
  }
  // CYOA mode
  if (display_name) {
    printf("%s says:\n", FUMO_LIST[fm].name);
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
    RESET_COLOR;
  }
  // fumo!
  fumofumo(fm);
  return 0;
}
