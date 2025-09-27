// -- fumosay: fumolang.h --
// like cowsay, but with funky fumos!

// Fumo Language: Text processing functions

#ifndef FUMO_LANG_H
#define FUMO_LANG_H

#include "fumoutil.h"
#include <stdbool.h>
#include <stdio.h>

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

// How long the longest line should be, accounting for hard newlines
int longestLineWidth(int count, char **words, int width_limit);

// Splits words if needed, first by white spaces, then by length
// modifies both _words_ and _count_
char **splitWords(int *count, char **words, int width_limit);

// prints some spaces and the right parenthesis
void paddedBreak(int padding, int (*fumo_say)(const char *, FILE *));

// wraps and prints words
void wordWrapper(
    int count,
    char **words,
    size_t width,
    size_t bubble,
    bool no_wrap,
    bool cmd,
    int (*fumo_say)(const char *, FILE *)
);

/* lolcat but fumo (rainbow fputs + utf8) */
int lolfumo(const char *str, FILE *dest);
int lolbyakuren(const char *str, FILE *dest);

// Takes NULL-terminated UTF-8 strings!
// Returns the display width of the string, ANSI escape codes are handled.
int strlen_real(char *str);

// reads a line, ignoring carriage returns '\r' for Windows
char *getInput(FILE *st, size_t size);

// Replace tabs with spaces in one word, reallocates _token_
char *replaceTab(char *token, short tabstop);

#endif