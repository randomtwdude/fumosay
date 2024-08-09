// -- fumosay: fumolang.h --
// like cowsay, but with funky fumos!

// Fumo Language: Text processing functions

#ifndef FUMO_LANG_H
#define FUMO_LANG_H

#include <stdio.h>
#include <stdbool.h>

#define SET_COLOR(r,g,b) printf("\033[38;2;%hd;%hd;%hdm", r, g, b);

extern int MAX_WIDTH;

// Marisa borrowed the ANSI code from github.com/jaseg/lolcat
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

int longestLineWidth(int argc, char **argv);
void paddedBreak(int padding, int (*fumo_say)(const char *, FILE *));
void wordWrapper(int count, char **words, size_t width, size_t bubble,
                 bool no_wrap, bool cmd, int (*fumo_say)(const char *, FILE *));

static enum esc_st find_escape_sequences(char c, enum esc_st st);
int lolfumo(const char *str, FILE *dest);
int strlen_real(char *str);
char *getInput(FILE *st, size_t size);
char *replaceTab(char *token, short tabstop);
char **splitWords(char **words, int *count);

#endif