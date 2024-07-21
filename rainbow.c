// -- fumosay --
// like cowsay, but with funky fumos!
// Lunar rainbow of Tenkyuu Chimata

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define SET_COLOR(r,g,b) printf("\033[38;2;%hd;%hd;%hdm", r, g, b);

// Marisa borrowed the code from github.com/jaseg/lolcat
enum esc_st {
  ST_NONE = 0,
  ST_ESC_BEGIN,
  ST_ESC_STRING,
  ST_ESC_CSI,
  ST_ESC_STRING_TERM,
  ST_ESC_CSI_TERM,
  ST_ESC_TERM,
  NUM_ST
};

// We'll actually use this in the next version, promise
static enum esc_st find_escape_sequences(char c, enum esc_st st) {
  if (st == ST_NONE || st == ST_ESC_CSI_TERM) {
    if (c == '\033') { /* Escape sequence YAY */
      return ST_ESC_BEGIN;
    } else {
      return ST_NONE;
    }

  } else if (st == ST_ESC_BEGIN) {
    if (c == '[') {
      return ST_ESC_CSI;
    } else if (c == 'P' || c == ']' || c == 'X' || c == '^' || c == '_') {
      return ST_ESC_STRING;
    } else {
      return ST_ESC_TERM;
    }

  } else if (st == ST_ESC_CSI) {
    if (0x40 <= c && c <= 0x7e) {
      return ST_ESC_CSI_TERM;
    } else {
      return st;
    }

  } else if (st == ST_ESC_STRING) {
    if (c == '\007') {
      return ST_NONE;
    } else if (c == '\033') {
      return ST_ESC_STRING_TERM;
    } else {
      return st;
    }

  } else if (st == ST_ESC_STRING_TERM) {
    if (c == '\\') {
      return ST_NONE;
    } else {
      return ST_ESC_STRING;
    }

  } else {
    return ST_NONE;
  }
}

// lolcat but fumo (rainbow fputs)
int lolfumo(const char *str, FILE *dest) {
  static int col = 0, line = 0;
  int i = 0;
  double hFreq = 0.25,  /* how wide the rainbow is */
         vFreq = 0.1,   /* how tall the rainbow is */
         offset = 0.45; /* higher value = more pastel */

  // every rainbow is different
  static double rainbow_start = -1.0;
  if (rainbow_start == -1.0) {
    rainbow_start = 2.0 * M_PI * arc4random() / RAND_MAX;
  }

  while (str[i]) {
    // get rainbow color
    double theta = col * hFreq / 5.0 + line * vFreq + rainbow_start;
    int red   = lrintf(255.0 * (offset + (1.0 - offset) * (0.5 + 0.5 * sin(theta + 0))));
    int green = lrintf(255.0 * (offset + (1.0 - offset) * (0.5 + 0.5 * sin(theta + 2 * M_PI / 3))));
    int blue  = lrintf(255.0 * (offset + (1.0 - offset) * (0.5 + 0.5 * sin(theta + 4 * M_PI / 3))));
    SET_COLOR(red, green, blue);

    fputc(str[i], dest);
    if (str[i] == '\n') {
      col = 0;
      line += 1;
    } else {
      col += 1;
    }
    i += 1;
  }
  return i;
}