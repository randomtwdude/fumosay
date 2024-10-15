// -- fumosay: fumolang.c --
// like cowsay, but with funky fumos!

// Fumo Language: Text processing functions
#include "fumolang.h"
#include "fumoutil.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistr.h>
#include <uniwidth.h>
#include <unictype.h>
#include <unigbrk.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))

/* How long the longest line should be, accounting for hard newlines */
int longestLineWidth(int argc, char **argv) {
  size_t cur_line = 0, max_line = 0;
  size_t word_len;
  for (int i = 0; i < argc; i += 1) {
    word_len = strlen_real(argv[i]);
    cur_line += word_len + 1;
    if (cur_line > MAX_WIDTH) {
      max_line = MAX(cur_line - word_len - 1, max_line);
      cur_line = word_len + 1;
    }
    if (argv[i][strlen(argv[i]) - 1] == '\n') {
      max_line = MAX(cur_line, max_line);
      cur_line = 0;
    }
  }
  return MAX(cur_line, max_line);
}

/* Print some spaces and the right parenthesis */
void paddedBreak(int padding, int (*fumo_say)(const char *, FILE *)) {
  for (int i = 0; i < padding; i += 1) {
    fumo_say(" ", stdout);
  }
  fumo_say(")\n", stdout);
}

/* Shiny "better" word-wrapping
 * Based on the shortest path algo. (xxyxyz.org/line-breaking)
 */
void wordWrapper(int count, char **words, size_t width, size_t bubble,
                 bool no_wrap, bool cmd, int (*fumo_say)(const char *, FILE *)
) {

  int *offsets = calloc((count + 1), sizeof(int));
  for (int i = 1; i < count + 1; i += 1) {
    offsets[i] = offsets[i - 1] + strlen_real(words[i - 1]);
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
    for (; j < intervals[start] - 1; j += 1) {
      int word_len = strlen_real(words[j]);
      fumo_say(words[j], stdout);
      fumo_say(" ", stdout);
      cur_len += word_len + 1;
      if (!cmd) { // only clean if the word is not from the command line
        free(words[j]);
      }
    }
    fumo_say(words[j], stdout); // last word has no space following it
    cur_len += strlen_real(words[j]);
    if (!cmd) {
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

/* getline() but this one is mine, also ignores CR */
char *getInput(FILE *st, size_t size) {
  size_t len = 0;
  char *str = malloc(size);
  if (!str) {
    return str;
  }
  char ch;
  while ((ch = fgetc(st)) != EOF) {
    if (ch == '\r') {
      continue;
    }
    str[len++] = ch;
    if (len == size) {
      str = realloc(str, sizeof(*str) * (size += 16));
      if (!str) {
        return str;
      }
    }
    if (ch == '\n') {
      break;
    }
  }
  str[len++] = '\0';
  return realloc(str, len);
}

/* Replace tabs with spaces in one word, modifies token */
char *replaceTab(char *token, short tabstop) {
  int size_byte = strlen(token) + 1;
  for (int i = 0; token[i]; i += 1) {
    if (token[i] == '\t') {
      token[i] = 0;
      short shift = tabstop - (strlen_real(token) % tabstop);
      token = realloc(token, (size_byte += shift - 1));
      memmove(&token[i + shift], &token[i + 1], size_byte - shift - i);
      memset(&token[i], 32, shift);
    }
  }
  return token;
}

/* Splits words if needed, first by white spaces, then by length
 * modifies both _words_ and _count_
 */
char **splitWords(char **words, int *count) {
  // first pass: split on whitespaces
  for (int i = 0; i < *count; i += 1) {
    int j = 0;
    short unit_size = 0;
    ucs4_t ch;
    while (words[i][j]) {
      j += unit_size;
      unit_size = u8_mbtouc(&ch, &words[i][j], 6);
      if (unit_size < 1) {
        break;
      }
      if (!uc_is_property_space(ch) && ch != 0x0009) {
        continue;
      }

      size_t head_size = j + 1; // before the whitespace
      int ws_start = j; // start of the first space (of potentially many)

      // seek to next non-space code point
      while (uc_is_property_space(ch) || ch == 0x0009) {
        unit_size = u8_mbtouc(&ch, &words[i][j], 6);
        if (unit_size < 1) {
          break;
        }
        j += unit_size;
        // we won't continue the while from here
      }
      size_t tail_size = strlen(words[i]) - j + unit_size + 1;

      // a head/tail_size of 1 means that word is empty (extra spaces in input)
      if (head_size > 1) {
        if (tail_size > 1) {
          words = realloc(words, (*count += 1) * sizeof(char *));
          memmove(&words[i + 2],
                  &words[i + 1],
                  (*count - i - 2) * sizeof(char *)
          );
          words[i + 1] = malloc(tail_size);
          memmove(words[i + 1],
                  words[i] + j - unit_size,
                  tail_size
          );
        }
        words[i][ws_start] = 0; // cut the original
      } else {
        memmove(words[i], words[i] + j - unit_size, tail_size);
        i -= 1; // redo this word (go back one)
      }
      break; // go to next word (for-loop)
    }
  }

  // second pass: split on MAX_WIDTH
  for (int i = 0; i < *count; i += 1) {
    if (strlen_real(words[i]) < MAX_WIDTH) {
      continue;
    }
    size_t size_byte = strlen(words[i]);
    int chosen_break = 0;
    char *good_breaks = malloc(size_byte);
    u8_grapheme_breaks(words[i], size_byte, good_breaks);

    // start looking just before where we could reach max_width
    for (int j = MAX_WIDTH - 2; j < size_byte; j += 1) {
      if (good_breaks[j]) {
        // we need to insert a 0 for strlen
        good_breaks[j] = words[i][j];
        words[i][j] = 0;
        if (strlen_real(words[i]) < MAX_WIDTH) {
          chosen_break = j;
        }
        words[i][j] = good_breaks[j]; // restore
      }
    }
    free(good_breaks);

    // split
    if (chosen_break == 0) {
      fumo_panic("The word just won't fit no matter how hard I tried :(\n", 0, "");
    }
    size_t tail_size = size_byte - chosen_break + 1;

    // perform memory witchery
    words = realloc(words, (*count += 1) * sizeof(char *));
    memmove(&words[i + 2],
            &words[i + 1],
            (*count - i - 2) * sizeof(char *)
    );
    words[i + 1] = malloc(tail_size);
    memmove(words[i + 1],
            words[i] + chosen_break,
            tail_size
    );
    words[i][chosen_break] = 0;
    // go to next word
  }
  return words;
}

// this and lolfumo from jaseg/lolcat
// we only really need ST_NONE
static enum esc_st _find_escape_sequences(char c, enum esc_st st) {
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

// we're doing gradients now too
void rgb_interpolate(color *start, color *end, int *r, int *g, int *b, double f) {
  *r = start->R + (end->R - start->R) * f;
  *g = start->G + (end->G - start->G) * f;
  *b = start->B + (end->B - start->B) * f;
}

/* lolcat but fumo (rainbow fputs + utf8) */
int lolfumo(const char *str, FILE *dest) {
  static int col = 0, line = 0;
  double hFreq = 0.25,  /* how wide the rainbow is */
         vFreq = 0.1,   /* how tall the rainbow is */
         offset = 0.45; /* higher value = more pastel */

  // every rainbow is different
  static double rainbow_start = -1.0;
  if (rainbow_start == -1.0) {
    rainbow_start = 2.0 * M_PI * random_clamped(RAND_MAX) / RAND_MAX;
  }

  int i = 0;
  enum esc_st state = ST_NONE;
  short unit_size = 0;
  while (str[i]) {
    i += unit_size;
    // skip ANSI controls
    unit_size = u8_mblen(&str[i], 6);
    if (unit_size < 1) {
      break;
    }
    if (unit_size == 1) {
      state = _find_escape_sequences(str[i], state);
      if (state != ST_NONE) {
        continue;
      }
    } else {
      state = ST_NONE;
    }

    // get rainbow color
    int red, green, blue;
    double theta = col * hFreq / 5.0 + line * vFreq + rainbow_start;
    if (isByakuren) {
      // special gradient just for her
      color purple = {185, 120, 235};
      color yellow = {240, 195, 130};
      theta = fmodf(theta / 2.0 / M_PI, 2.0f);
      theta = theta > 1.0 ? 2.0 - theta : theta;
      rgb_interpolate(&purple, &yellow, &red, &green, &blue, theta);
    } else {
      red   = lrintf(255.0 * (offset + (1.0 - offset) * (0.5 + 0.5 * sin(theta + 0))));
      green = lrintf(255.0 * (offset + (1.0 - offset) * (0.5 + 0.5 * sin(theta + 2 * M_PI / 3))));
      blue  = lrintf(255.0 * (offset + (1.0 - offset) * (0.5 + 0.5 * sin(theta + 4 * M_PI / 3))));
    }
    SET_COLOR(red, green, blue);
    // print this code point
    for (int j = 0; j < unit_size; j += 1) {
      fputc(str[i + j], dest);
    }

    if (str[i] == '\n') {
      col = 0;
      line += 1;
    } else {
      col += u8_width(&str[i], unit_size, "UTF-8");
    }
  }
  return i;
}

/* Takes NULL-terminated UTF-8 strings!
 * Returns the *display width* of the string, ANSI escape codes are taken into account as well
 */
int strlen_real(char *str) {
  int length = 0;
  int byte_count = strlen(str);
  enum esc_st state = ST_NONE;
  short unit_size = 0;

  for (int i = 0; i < byte_count; i += unit_size) {
    unit_size = u8_mblen(&str[i], 6);
    if (unit_size < 1) {
      break;
    }
    // ANSI escape
    if (unit_size == 1) {
      state = _find_escape_sequences(str[i], state);
      if (state != ST_NONE) {
        continue;
      }
    } else {
      state = ST_NONE; // should be fine?
    }

    length += u8_width(&str[i], unit_size, "UTF-8");
  }
  return length;
}