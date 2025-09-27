// -- fumosay: fumoutil.h --
// like cowsay, but with funky fumos!

// Fumo Utilities: Search, expressions, and other fumo related stuff

#ifndef FUMO_UTIL_H
#define FUMO_UTIL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>

#define ALPHABET 128
#define WORDLEN 63

#define SET_COLOR(c)                  \
    ({                                \
        color _c = (c);               \
        SET_COLOR3(_c.R, _c.G, _c.B); \
    })
#define SET_COLOR3(r, g, b) printf("\033[38;2;%hd;%hd;%hdm", r, g, b)
#define RESET_FORMAT printf("\033[0m")

#define COLOR_OFF 0
#define COLOR_ON 1
#define COLOR_CUSTOM 2

#define FUMO_QUIT(msg) fumo_panic(msg, pref.expr, pref.custom_expr)

typedef int fumo_who;

// fumos with special handlings
typedef enum fumo_special {
    SPECIAL_NONE, // ZUN better not name someone のね
    SPECIAL_CHIMATA,
    SPECIAL_BYAKUREN,
} fumo_special;

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

typedef struct fumo_settings {
    fumo_who fumo;
    // word wrapping
    bool no_wrap;
    int width;
    // formatting
    bool display_name;
    bool rainbow;
    bool rainbow_fumo;
    bool bold;
    char color;
    struct color custom_color;
    // expressions
    char expr;
    char custom_expr[30];
} fumo_settings;

// Check if we're a fumo that needs special handling
fumo_special fumo_check_special(fumo_who fm);

// Set expression of the fumo
void fumo_expr(fumo_who fm, char ex, char *custom);

// ᗜˬᗜ
void fumo_fumo(fumo_who fm, int (*fumo_say)(const char *, FILE *));
void fumo_list();
void fumo_panic(char *message, char expr, char *custom_expr);
fumo_who fumo_picker(char *str);

int bitap(char *haystack, char *needle, int max_dist);
int random_uniform(int num);

#endif