// -- fumosay --
// like cowsay, but with funky fumos!
// ᗜ_ᗜ have a nice day ᗜˬᗜ

/* ===== INCLUDES ===== */
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
  char *fumo;    // fumo ascii art
  color color;    // [object Object]
  char  *name[];   // all the names
} fumo_data;

typedef int fumo_who;

// fumo & expression file
#include "fumo.fumo"
// ANSI Rainbow
#include "rainbow.c"
// Fumo picker
#include "fumosearch.c"

#define SET_COLOR(r,g,b) printf("\033[38;2;%hd;%hd;%hdm", r, g, b);
#define RESET_COLOR printf("\033[0m");

#define VERSION_STRING "fumosay 1.1.10\n"
/* ===== FUNCTIONS ===== */

/* utility functions */

#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define MIN4(a,b,c,d) (MIN(MIN((a),(b)),MIN((c),(d))))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

void helpInfo(fumo_who fm) {
  printf("fumosay: funky talking fumofumos\n"
         "--------------------------------\n"
         "Usage: fumosay [-hvlngrRb] [-c] [-W column] [-f name] [-E expression] (message)\n"
         "-l     List all fumos.\n"
         "-n     Disable word-wrapping. Overrides -W.\n"
         "-W     Specify roughly where the message should be wrapped.\n"
         "       The default is 80.\n"
         "-f     Pick fumos by names.\n"
         "-E     Change the expression (mouth string) of the fumo.\n"
         "       Use a one-letter preset below or enter your own!\n"
         "       Presets: '1', '2', '3', '4', 'v', 'w', 'b', 'r'(random)\n"
         "-g     Display the character's name above the text box like a story game.\n"
         "-c     Color message. Optionally, specify RGB like \"-c255,255,255\".\n"
         "-r     Rainbow message courtesy of Tenkyuu Chimata.\n"
         "-R     Rainbow fumo as well.\n"
         "-b     Bold font.\n"
         "-h     Display this message.\n"
         "-v     Show version.\n"
         "--------------------------------\n"
         "In spirit of the original cowsay, made by Tony Monroe in 1999.\n"
         "Fumos feature characters from Touhou Project, say hi to ");

  SET_COLOR(FUMO_LIST[fm]->color.R, FUMO_LIST[fm]->color.G, FUMO_LIST[fm]->color.B);
  fputs(FUMO_LIST[fm]->name[0], stdout);
  RESET_COLOR;
  fputs("!\n", stdout);
}

// Turns a string to number (basically just strtol that rejects 0)
int numberize(char *c) {
  long number = strtol(c, 0, 0);
  if (number == 0) {
    printf("Invalid parameter: %s", c);
  }
  return (int)number;
}

// Returns one word that's at most a certain length
// _line_ turns this into getline(), max length is adjusted to very large in main()
char *getInput(FILE *st, size_t size, bool line) {
  size_t len = 0;
  char *str = malloc(size);
  if (!str) {
    return str;
  }
  char ch;
  while ((ch = fgetc(st)) != EOF) {
    if (!line && (ch == ' ' || ch == '\t')) {
      if (len) {
        break;    // if we have a word, spaces terminate
      } else {
        continue; // ignore consecutive spaces
      }
    }
    str[len++] = ch;
    // extend the buffer if necessary
    if (len == size) {
      str = realloc(str, sizeof(*str) * (size += 16));
      if (!str) {
        return str;
      }
    }
    if (ch == '\n' || len == MAX_WIDTH - 1) {
      break;
    }
  }
  str[len++] = '\0';
  return realloc(str, len);
}

// Replace tabs with spaces, REALLOCs token!
char *replaceTab(char *token, short tabstop) {
  int size_byte = strlen(token) + 1;
  for (int i = 0; token[i]; i += 1) {
    if (token[i] == '\t') {
      short shift = tabstop - (i % tabstop);
      token = realloc(token, (size_byte += shift - 1));
      memmove(&token[i + shift], &token[i + 1], size_byte - shift - i);
      memset(&token[i], 32, shift);
    }
  }
  return token;
}

/* fumo functions */

// Pick a fumo
fumo_who fumo_picker(char *str) {
  fumo_who i = 0, fumo = -1;
  int highscore = 0;
  while (i < FUMO_COUNT) {
    int j = 0, name = 0;
    while (name < 10) {
      int score = bitap(str, FUMO_LIST[i]->name[j], 2);
      if (score > highscore) {
        highscore = score;
        fumo = i;
      }
      j += 1;
      if (0 == FUMO_LIST[i]->name[j][0]) {
        break; /* names are ""-terminated */
      }
      name += 1;
    }
    i += 1;
  }
  if (highscore == 0) {
    printf("ᗜ_ᗜ : We cannot find \"%s\"!\n", str);
  }
  return fumo;
}

// Set fumo expression, modifies string
void fumo_expr(fumo_who fm, char ex, char *custom) {
  char *p;
  short count = 0;

  // use custom expression
  if (custom[0] != 0) {
    while (count < 3) {
      p = strchr(FUMO_LIST[fm]->fumo, 'E');
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
      p = strchr(FUMO_LIST[fm]->fumo, 'E');             \
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
  case 'b':
    set_expression(6); break;
  case 'r':;
    int r = arc4random_uniform(EXPRESSION_COUNT);
    set_expression(r);
    break;
  default:
    while (count < 3) {
      p = strchr(FUMO_LIST[fm]->fumo, 'E');
      if (!p) {
        return;
      }
      *p = ' ';
    }
  }
  #undef set_expression
}

// Lists fumos
void fumo_list() {
  printf("There are %d fumos!\n-------------------\n", FUMO_COUNT);
  for (int i = 0; i < FUMO_COUNT; i += 1) {
    fputs(FUMO_LIST[i]->name[0], stdout);
    fputs("\n", stdout);
  }
}

// Fumo!
void fumo_fumo(fumo_who fm, int (*fumo_say)(const char *, FILE *)) {
  fumo_say(FUMO_LIST[fm]->fumo, stdout);
}

// panik
void fumo_panic(char *message, char expr, char *custom_expr) {
  fputs(message, stdout);
  fumo_who helper_fumo = arc4random_uniform(FUMO_COUNT);
  fumo_expr(helper_fumo, expr, custom_expr);
  fumo_fumo(helper_fumo, &fputs);
  exit(EXIT_FAILURE);
}

/* word processing functions */

// should probably integrate this into word wrapper somehow
int longestLineWidth(int argc, char **argv) {
  size_t cur_line = 0, max_line = 0;
  size_t word_len;
  for (int i = 0; i < argc; i += 1) {
    word_len = strlen(argv[i]);
    cur_line += word_len + 1;
    if (cur_line > MAX_WIDTH) {
      max_line = MAX(cur_line - word_len - 1, max_line);
      cur_line = word_len + 1;
    }
    if (argv[i][word_len - 1] == '\n') {
      max_line = MAX(cur_line - 1, max_line);
      cur_line = 0;
    }
  }
  return MAX(cur_line, max_line);
}

// Print some spaces and the right parenthesis
void paddedBreak(int padding, int (*fumo_say)(const char *, FILE *)) {
  for (int i = 0; i < padding; i += 1) {
    fumo_say(" ", stdout);
  }
  fumo_say(")\n", stdout);
}

// Shiny "better" word-wrapping
// Based on the shortest path algo. (xxyxyz.org/line-breaking)
void wordWrapper(int count, char **words, size_t width, size_t bubble,
                 bool no_wrap, bool cmd, int (*fumo_say)(const char *, FILE *)) {

  int *offsets = calloc((count + 1), sizeof(int));
  for (int i = 1; i < count + 1; i += 1) {
    offsets[i] = offsets[i - 1] + strlen(words[i - 1]);
  }

  int *minima = malloc((count + 1) * sizeof(int));
  memset(minima, 0x7f, (count + 1) * sizeof(int)); // big number
  minima[0] = 0;
  int *breaks = calloc((count + 1), sizeof(int));

  for (int i = 0; i < count; i += 1) {
    int j = i + 1;
    while (j <= count) {
      int w = offsets[j] - offsets[i] + j - i - 1;
      if (w > width) {
        break;
      }
      // do not add cost if it's the last line of the paragraph
      int cost = minima[i] + ((i > 0 && j == count) ? 0 : (width - w) * (width - w));
      if (cost <= minima[j]) {
        minima[j] = cost;
        breaks[j] = i;
      }
      j += 1;
    }
  }

  // reverse the breaks so they're in the right order
  int *intervals = malloc((count + 1) * sizeof(int));
  int j = count;
  while (j > 0) {
    int i = breaks[j];
    intervals[i] = j;
    j = i;
  }

  // print message
  int cur_len, start;
  while (j < count) {
    cur_len = 0;
    start = j;

    fumo_say("( ", stdout);
    // print words
    for (; j < intervals[start] - 1; j++) {
      fumo_say(words[j], stdout);
      fumo_say(" ", stdout);
      cur_len += strlen(words[j]) + 1;
      if (!cmd) {
        free(words[j]);
      }
    }
    fumo_say(words[j], stdout); // last word has no space following it
    cur_len += strlen(words[j]);
    if (!cmd) { // only clean if the word is not from the command line
      free(words[j]);
    }
    paddedBreak(bubble - cur_len - 1, fumo_say);
    j = intervals[start];
  }

  // clean
  free(intervals);
  free(offsets);
  free(minima);
  free(breaks);
}

int main(int argc, char **argv) {
  // init
  bool no_wrap = false;
  bool display_name = false;
  char colour = 0;
  color custom_colour = {0, 0, 0};
  bool rainbow = false;
  bool rainbow_fumo = false;
  bool bold = false;
  char expr = 0;
  char custom_expr[30] = {0};
  fumo_who fm = -1;
  // for cosmopolitan build, which uses random()
  // srandom(getpid());

  // argument
  char opt;
  while ((opt = getopt(argc, argv, "hvlngrRbc::E:W:f:")) != -1) {
    switch (opt) {
    case 'h':;
      fumo_who helper_fumo = arc4random_uniform(FUMO_COUNT);
      helpInfo(helper_fumo);
      fumo_expr(helper_fumo, expr, custom_expr);
      fumo_fumo(helper_fumo, fputs);
      return 0;
    case 'l':
      fumo_list();
      return 0;
    case 'n':
      MAX_WIDTH = 100000000;
      no_wrap = true;
      break;
    case 'W':
      MAX_WIDTH = no_wrap ? MAX_WIDTH : numberize(optarg);
      break;
    case 'f':;
      char *token = strtok(optarg, ",.;/|");
      short choice_count = 0;
      short *choices = NULL;
      while (token) {
        int choice = fumo_picker(token);
        if (choice > -1) {
          choices = realloc(choices, ++choice_count * sizeof(short));
          choices[choice_count - 1] = choice;
        }
        token = strtok(NULL, ",.;/|");
      }
      if (choices) {
        fm = choices[arc4random_uniform(choice_count)];
        free(choices);
      }
      break;
    case 'g':
      display_name = true;
      break;
    case 'c':
      colour = 1;
      if (optarg) {
        colour = 2;
        // custom color values
        char *token = strtok(optarg, ",.;/|");
        custom_colour.R = strtol(token, NULL, 0);
        if (token = strtok(NULL, ",.;/|")) {
          custom_colour.G = strtol(token, NULL, 0);
        }
        if (token = strtok(NULL, ",.;/|")) {
          custom_colour.B = strtol(token, NULL, 0);
        }
      }
      break;
    case 'r':;
      rainbow = true;
      break;
    case 'R':;
      rainbow = true;
      rainbow_fumo = true;
      break;
    case 'b':;
      bold = true;
      break;
    case 'E':;
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
    case 'v':;
      printf(VERSION_STRING);
      return 0;
    } // switch
  }

  if (MAX_WIDTH < 3) {
    MAX_WIDTH = 3;
  }

  if (bold) {
    fputs("\033[1m", stdout);
  }

  // fumo reads
  int word_count = 0;
  char **word_vector = realloc(NULL, sizeof(char *));
  if (optind == argc) { // read from stdin
    while (1) {
      char *token;
      token = getInput(stdin, 64, no_wrap);
      if (!token) {
        fumo_panic("fumo panic! - memory allocation failed: getInput.\n", expr, custom_expr);
      }
      if (0 == strlen(token)) {
        free(token); /* sneaky 1-byte memory leak exterminated */
        break;
      }
      if (no_wrap) {
        token = replaceTab(token, 8);
        if (!token) {
          fumo_panic("fumo panic! - memory allocation failed: replaceTab.\n", expr, custom_expr);
        }
      }
      word_vector = realloc(word_vector, (word_count + 1) * sizeof(char *));
      word_vector[word_count++] = token;
    }

  } else { // read from cmd line arguments
    word_count = argc - optind;

    // I can't realloc argv so this'll have to do
    word_vector = malloc(word_count * sizeof(char *));
    memcpy(word_vector, argv + optind, word_count * sizeof(char *));

    // tabs
    for (int i = 0; i < word_count; i++) {
      if (strchr(word_vector[i], '\t') == NULL) {
        continue;
      }
      // this is a memory leak, but better than
      // having to have another copy of every word
      char *token = strdup(word_vector[i]);
      word_vector[i] = replaceTab(token, 8);
    }

    // cut words that are too long
    for (int i = 0; !no_wrap && i < word_count; i++) {
      if (strlen(word_vector[i]) > MAX_WIDTH - 1) {
        // move things around
        word_vector = realloc(word_vector, (word_count += 1) * sizeof(char *));
        memmove(&word_vector[i + 2],
                &word_vector[i + 1],
                (word_count - i - 2) * sizeof(char *));
        word_vector[i + 1] = malloc(strlen(word_vector[i]) - MAX_WIDTH + 2);
        memmove(word_vector[i + 1],
                word_vector[i] + MAX_WIDTH - 1,
                strlen(word_vector[i]) - MAX_WIDTH + 2);
        word_vector[i][MAX_WIDTH - 1] = 0;
      }
    }
  }

  // calculate the width of the bubble
  int bubble_width = longestLineWidth(word_count, word_vector) + 1;

  // choose a fumo if not already chosen
  if (fm == -1) {
    fm = arc4random_uniform(FUMO_COUNT);
  }

  // finalize expression
  fumo_expr(fm, expr, custom_expr);

  // colours
  if (strcmp(FUMO_LIST[fm]->name[0], "Chimata") == 0) {
    rainbow = true;
  }

  int (*fumo_say)(const char *, FILE *);
  if (rainbow) {
    fumo_say = &lolfumo;
  } else {
    fumo_say = &fputs;
  }

  if (colour == 1) {
    SET_COLOR(FUMO_LIST[fm]->color.R, FUMO_LIST[fm]->color.G, FUMO_LIST[fm]->color.B);
  } else if (colour == 2) {
    SET_COLOR(custom_colour.R, custom_colour.G, custom_colour.B);
  }

  // CYOA mode
  if (display_name) {
    printf("%s says:\n", FUMO_LIST[fm]->name[0]);
  }

  // top line
  fumo_say(" ", stdout);
  for (int i = 0; i < bubble_width; i++) {
    fumo_say("_", stdout);
  }
  fumo_say("\n", stdout);

  // message
  int last_section = 0, cur_section = 0;
  char **process = word_vector;
  char *nl;
  for (int i = 0; i < word_count; i++) {
    cur_section++;
    if ((nl = strchr(word_vector[i], '\n')) != NULL || i == word_count - 1) {
      if (nl) {
        *nl = 0; // strip the last newline of the paragraph
      }
      process += last_section;
      wordWrapper(cur_section,
                   process,
                   bubble_width - 2,
                   bubble_width,
                   no_wrap,
                   optind != argc,
                   fumo_say);
      last_section = cur_section;
      cur_section = 0;
    }
  }

  // bottom line
  fumo_say(" ", stdout);
  for (int i = 0; i < bubble_width; i++) {
    fumo_say("-", stdout);
  }

  // reset color
  if (colour || (rainbow && !rainbow_fumo)) {
    RESET_COLOR;
    if (bold) {
      fputs("\033[1m", stdout);
    }
  }

  // fumo!
  if (rainbow_fumo) {
    fumo_fumo(fm, &lolfumo);
  } else {
    fumo_fumo(fm, &fputs);
  }

  if (rainbow_fumo || bold) {
    RESET_COLOR;
  }

  free(word_vector);
  return 0;
}
