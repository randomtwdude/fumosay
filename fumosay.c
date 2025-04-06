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

// Everything fumo
#include "fumoutil.h"
#include "fumo.fumo"
// Everything text
#include "fumolang.h"

#define SET_COLOR(r,g,b) printf("\033[38;2;%hd;%hd;%hdm", r, g, b);
#define RESET_COLOR printf("\033[0m");

#ifndef __COSMOCC__
#define VERSION_STRING "fumosay 1.2.3\n"
#else
#define VERSION_STRING "fumosay 1.2.3 cosmos\n"
#endif

int MAX_WIDTH = 80;
bool isByakuren = false;

/* ===== FUNCTIONS ===== */
void fumo_help(fumo_who fm) {
  printf("fumosay: funky talking fumofumos\n"
         "--------------------------------\n"
         "Usage: fumosay [-hvl] [ngrRb] [-c] [-W column] [-f name] [-E expression] (message)\n"
         "-l     List all fumos.\n"
         "-h     Display this message.\n"
         "-v     Show version.\n"
         "\n"
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
         "--------------------------------\n"
         "In spirit of the original cowsay, made by Tony Monroe in 1999.\n"
         "Fumos feature characters from Touhou Project, say hi to ");

  SET_COLOR(FUMO_LIST[fm]->color.R, FUMO_LIST[fm]->color.G, FUMO_LIST[fm]->color.B);
  fputs(FUMO_LIST[fm]->name[0], stdout);
  RESET_COLOR;
  fputs("!\n", stdout);
}

/* Turns a string to number (basically just strtol that rejects 0) */
int numberize(char *c) {
  long number = strtol(c, 0, 0);
  if (number == 0) {
    fprintf(stderr, "ᗜ_ᗜ: Invalid parameter %s\n", c);
  }
  return (int)number;
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

  // argument
  char opt;
  while ((opt = getopt(argc, argv, "hvlngrRbc::E:W:f:")) != -1) {
    switch (opt) {
    case 'h':;
      fumo_who helper_fumo = random_uniform(FUMO_COUNT);
      fumo_help(helper_fumo);
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
        fm = choices[random_uniform(choice_count)];
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
        if ((token = strtok(NULL, ",.;/|"))) {
          custom_colour.G = strtol(token, NULL, 0);
        }
        if ((token = strtok(NULL, ",.;/|"))) {
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
  char **word_vector = NULL;
  if (optind == argc) { // read from stdin
    while (1) {
      char *token;
      token = getInput(stdin, 64);
      if (!token) {
        fumo_panic("ᗜxᗜ: fumo panic - memory allocation failed: getInput.\n", expr, custom_expr);
      }
      if (0 == strlen(token)) {
        free(token); /* sneaky 1-byte memory leak exterminated */
        break;
      }
      if (no_wrap) {
        token = replaceTab(token, 8);
        if (!token) {
          fumo_panic("ᗜxᗜ: fumo panic - memory allocation failed: replaceTab.\n", expr, custom_expr);
        }
      }
      word_vector = realloc(word_vector, (word_count + 1) * sizeof(char *));
      if (!word_vector) {
        fumo_panic("ᗜxᗜ: fumo panic - memory allocation failed while reading!\n", expr, custom_expr);
      }
      word_vector[word_count++] = token;
    }

  } else { // read from cmd line arguments
    word_count = argc - optind;

    // I can't realloc argv so this'll have to do
    word_vector = malloc(word_count * sizeof(char *));
    if (!word_vector) {
      fumo_panic("ᗜxᗜ: fumo panic - memory allocation failed while reading!\n", expr, custom_expr);
    }
    memcpy(word_vector, argv + optind, word_count * sizeof(char *));

    char *newline = NULL;
    for (int i = 0; i < word_count; i++) {
      // remove newlines from cmd message
      while ((newline = strchr(word_vector[i], '\n'))) {
        *newline = ' ';
      }
      // replace tabs with spaces
      if (strchr(word_vector[i], '\t') == NULL) {
        continue;
      }
      char *token = strdup(word_vector[i]);
      word_vector[i] = replaceTab(token, 8);
    }
  }

  // cut lines into words
  if (!no_wrap) {
    word_vector = splitWords(word_vector, &word_count);
  }

  // calculate the width of the bubble
  int bubble_width = longestLineWidth(word_count, word_vector) + 1;

  // choose a fumo if not already chosen
  if (fm == -1) {
    fm = random_uniform(FUMO_COUNT);
  }

  // finalize expression
  fumo_expr(fm, expr, custom_expr);

  // colours
  if (fm == 55) {
    rainbow = true;
  } else if (fm == 60) {
    isByakuren = true;
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
    char cyoa_str[64];
    sprintf(cyoa_str, "%s says:\n", FUMO_LIST[fm]->name[0]);
    fumo_say(cyoa_str, stdout);
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
      wordWrapper(
        cur_section,
        process,
        bubble_width - 2,
        bubble_width,
        no_wrap,
        optind != argc,
        fumo_say
      );
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
