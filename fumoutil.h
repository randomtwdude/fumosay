// -- fumosay: fumoutil.h --
// like cowsay, but with funky fumos!

// Fumo Utilities: Search, expressions, and other fumo related stuff

#ifndef FUMO_UTIL_H
#define FUMO_UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>

#define ALPHABET 128
#define WORDLEN 63

typedef struct color {
    short R;
    short G;
    short B;
} color;

typedef struct fumo_data {
    char *fumo;   // fumo ascii art
    color color;  // [object Object]
    char *name[]; // all the names
} fumo_data;

typedef int fumo_who;

fumo_who fumo_picker(char *str);
void fumo_expr(fumo_who fm, char ex, char *custom);
void fumo_list();
void fumo_fumo(fumo_who fm, int (*fumo_say)(const char *, FILE *));
void fumo_panic(char *, char, char *);

int bitap(char *haystack, char *needle, int max_dist);
int random_uniform(int num);

#endif