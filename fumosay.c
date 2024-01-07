// -- fumosay v1.0 --
// like cowsay, but with funky fumos!
// ᗜ_ᗜ have a nice day ᗜˬᗜ

/* ===== INCLUDES ===== */
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

// windows support!
#ifdef _WIN32
#include "windows_func.c"
#endif

/* ===== DEFINES ===== */
int MAX_WIDTH = 80;

typedef struct color {
  short R; short G; short B;
} color;

typedef struct fumo_data {
  char  *fumo; // fumo ascii art
  char  *name; // display & search name
  color color;
} fumo_data;

typedef int fumo_who;

// fumo & expression file
#include "fumo.fumo"

#define SET_COLOR(r,g,b) printf("\033[38;2;%hd;%hd;%hdm", r, g, b);
#define RESET_COLOR printf("\033[0m");

/* ===== FUNCTIONS ===== */
#define MIN(a,b) (a > b ? b : a)
#define MAX(a,b) (a > b ? a : b)

void helpInfo(fumo_who fm) {
  printf("=== fumosay ver. 1.0 ===\n"
         "Usage: fumosay [-hlngc] [-W column] [-f name] [-E expression] (message)\n"
         "-l     List all fumos.\n"
         "-n     Disable word-wrapping. Overrides -W.\n"
         "-W     Specify roughly where the message should be wrapped.\n"
         "       The default is 80.\n"
         "-f     Pick fumos by name.\n"
         "-E     Change the expression (mouth) of the fumo.\n"
         "       Use a one-letter preset below or enter your own (like \"          *_*\")!\n"
         "       Presets: '1', '2', '3', '4', 'v', 'w', 'b', 'r'(random)\n"
         "-g     Display the character's name above the text box like a story game.\n"
         "-c     Color the text.\n"
         "-h     Display this message.\n"
         "In spirit of the original cowsay, made by Tony Monroe in 1999.\n"
         "Fumos feature characters from Touhou Project, say hi to ");
  SET_COLOR(FUMO_LIST[fm].color.R, FUMO_LIST[fm].color.G, FUMO_LIST[fm].color.B);
  fputs(FUMO_LIST[fm].name, stdout);
  RESET_COLOR;
  fputc('!', stdout);
}

// Turns a string to number
int numberize(char *c) {
  long number = strtol(c, (char **)NULL, 0);
  if (number == 0) {
    fprintf(stderr, "Invalid parameter: %s", c);
    exit(EXIT_FAILURE);
  }
  return (int)number;
}

// Returns one word that's at most a certain length
char *getInput(FILE *st, size_t size) {
  char *str;
  int ch;
  size_t len = 0;
  str = malloc(size);
  if (!str) {
    return str;
  }
  while ((ch = fgetc(st)) != EOF) {
    if (ch == ' ' || ch == '\t') {
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
    if (ch == '\n' || len == MAX_WIDTH - 2) {
      break;
    }
  }
  str[len++] = '\0';
  return realloc(str, len);
}

// Pick a fumo
fumo_who fumo_picker(char *str) {
  // we want to compare the lower case version
  for (int i = 0; i < strlen(str); i++) {
    str[i] = tolower(str[i]);
  }
  fumo_who i = 0;
  while (i < FUMO_COUNT) {
    char *fm = strdup(FUMO_LIST[i].name);
    fm[0] = tolower(fm[0]);
    if (strstr(str, fm) != NULL) {
      free(fm);
      break;
    }
    free(fm);
    i++;
  }

  if (i == FUMO_COUNT) {
    // alternate names we may want to check
    #define check_name(name, id) {          \
      if (strstr(str, #name) != NULL) {     \
        return id;                          \
      }                                     \
    }
    check_name(patchy, 1);
    check_name(remi, 10);
    check_name(snae, 11);
    check_name(gap, 13);
    check_name(tenko, 16);
    check_name(baka, 17);
    check_name(maido, 19);
    check_name(inu, 20);

    printf("Can't find fumo \"%s\"!\n", str);
    return -1;
  }

  return i;
}

// Set fumo expression, modifies string
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
  case 'b':
    set_expression(6); break;
  case 'r':
    int r = rand() % EXPRESSION_COUNT;
    set_expression(r);
    break;
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

// Lists fumos
void fumo_list() {
  printf("There are %d fumos!\n-----\n", FUMO_COUNT);
  for (int i = 0; i < FUMO_COUNT; i++) {
    puts(FUMO_LIST[i].name);
  }
}

// Fumo!
void fumofumo(fumo_who fm) {
  fputs(FUMO_LIST[fm].fumo, stdout);
}

// Returns length of longest line, considering hard line breaks
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

// Print some spaces and the right parenthesis
void paddedBreak(int padding) {
  for (int i = 0; i < padding; i++) {
    printf(" ");
  }
  printf(")\n");
}

// Shiny "better" word-wrapping
// Based on the shortest path algo. (xxyxyz.org/line-breaking)
void word_wrapper(int count, char **words, size_t width, size_t bubble, bool no_wrap, bool cmd) {

  int *offsets = calloc((count + 1), sizeof(int));
  for (int i = 1; i < count + 1; i++) {
    offsets[i] = offsets[i - 1] + strlen(words[i - 1]);
  }

  int *minima = malloc((count + 1) * sizeof(int));
  memset(minima, 0x7f, (count + 1) * sizeof(int)); // big number
  minima[0] = 0;
  int *breaks = calloc((count + 1), sizeof(int));

  for (int i = 0; i < count; i++) {
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
      j++;
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
    printf("( ");
    for (; j < intervals[start] - 1; j++) {
      printf("%s ", words[j]);
      cur_len += strlen(words[j]) + 1;
      if (!cmd) {
        free(words[j]);
      }
    }
    // last word has no space following it
    printf("%s", words[j]);
    cur_len += strlen(words[j]);
    if (!cmd) {
      free(words[j]);
    }
    paddedBreak(bubble - cur_len - 1);
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
  srand(getpid());
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
      fumo_who helper_fumo = rand() % FUMO_COUNT;
      helpInfo(helper_fumo);
      fumo_expr(helper_fumo, expr, custom_expr);
      fumofumo(helper_fumo);
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
      char *token = strtok(optarg, ",");
      short choice_count = 0;
      short *choices = NULL;
      while (token) {
        int choice = fumo_picker(token);
        if (choice > -1) {
          choices = realloc(choices, ++choice_count * sizeof(short));
          choices[choice_count - 1] = choice;
        }
        token = strtok(NULL, ",");
      }
      fm = choices[rand() % choice_count];
      free(choices);
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

  if (MAX_WIDTH < 3) {
    MAX_WIDTH = 3;
  }

  // fumo reads
  int word_count = 0;
  char **word_vector = realloc(NULL, sizeof(char *));
  if (optind == argc) { // read from stdin instead
    if (no_wrap) { // read one line at a time
      char *buffer = NULL;
      size_t buf_line, line;
      ssize_t bytes;
      while ((bytes = getline(&buffer, &buf_line, stdin)) > -1) {
      #ifndef _WIN32
        char *token = NULL;
        FILE *st = open_memstream(&token, &line); // POSIX supremacy
        for (int i = 0; i < buf_line; i++) {
          if (buffer[i] == 9) { // TAB
            fprintf(st, "    ");
          } else {
            fprintf(st, "%c", buffer[i]);
          }
        }
        fclose(st);
      #else
        char *token = malloc(buf_line);
        replace_tabs_windows(buffer, &token, bytes, buf_line);
      #endif
        word_vector[word_count++] = token;
        word_vector = realloc(word_vector, (word_count + 1) * sizeof(char *));
      }
      word_vector = realloc(word_vector, word_count * sizeof(char *));
      free(buffer);
    } else { // read one word at a time
      while (1) {
        char *token;
        token = getInput(stdin, 10);
        if (!token) {
          printf("Something has gone terribly wrong! (malloc failed)\n");
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
  } else { // read from cmd line
    word_count = argc - optind;
    // I can't realloc argv so this'll have to do
    word_vector = malloc(word_count * sizeof(char *));
    memcpy(word_vector, argv + optind, word_count * sizeof(char *));
    // cut words that are too long
    if (!no_wrap) {
      for (int i = 0; i < word_count; i++) {
        if (strlen(word_vector[i]) > MAX_WIDTH - 1) {
          word_vector = realloc(word_vector, (++word_count) * sizeof(char *));
          for (int j = word_count - 1; j > i + 1; j--) {
            word_vector[j] = word_vector[j - 1];
          }
          word_vector[i + 1] = malloc(strlen(word_vector[i]) - MAX_WIDTH + 2);
          memmove(word_vector[i + 1], word_vector[i] + MAX_WIDTH - 1, strlen(word_vector[i]) - MAX_WIDTH + 2);
          word_vector[i][MAX_WIDTH - 1] = 0;
        }
      }
    }
  }

  // calculate the width of the bubble
  size_t bubble_width = longestLineWidth(word_count, word_vector) + 1;

  // choose a fumo if not already chosen
  if (fm == -1) {
    fm = rand() % FUMO_COUNT;
  }

  // finalize expression
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
      word_wrapper(cur_section, process, bubble_width - 2, bubble_width, no_wrap, optind != argc);
      last_section = cur_section;
      cur_section = 0;
    }
  }

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

  free(word_vector);
  return 0;
}
