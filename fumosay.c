// -- fumosay --
// like cowsay, but with funky fumos!
// ᗜ_ᗜ have a nice day ᗜˬᗜ

/* ===== INCLUDES ===== */

// https://github.com/skeeto/optparse
#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Everything fumo
// clang-format off
#include "fumoutil.h"
#include "fumo.fumo"
// clang-format on

// Everything text
#include "fumolang.h"

#ifndef __COSMOCC__
#define VERSION_STRING "fumosay 1.2.3\n"
#else
#define VERSION_STRING "fumosay 1.2.3 cosmos\n"
#endif

/* ===== FUNCTIONS ===== */
void fumo_help(fumo_who fm)
{
    printf(
        "fumosay: funky talking fumofumos\n"
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
        "Fumos feature characters from Touhou Project, say hi to "
    );

    SET_COLOR(FUMO_LIST[fm]->color);
    fputs(FUMO_LIST[fm]->name[0], stdout);
    RESET_FORMAT;
    fputs("!\n", stdout);
}

/* Turns a string to number (basically just strtol that rejects 0) */
int numberize(char *c)
{
    long number = strtol(c, 0, 0);
    if (number == 0) {
        fprintf(stderr, "ᗜ_ᗜ: Invalid parameter %s\n", c);
    }
    return (int)number;
}

int main(int argc, char **argv)
{
    // init
    fumo_settings pref = {
        .fumo         = -1,
        .no_wrap      = false,
        .width        = 80,
        .display_name = false,
        .rainbow      = false,
        .rainbow_fumo = false,
        .bold         = false,
        .color        = COLOR_OFF,
        .custom_color = { .R = 0, .G = 0, .B = 0 },
        .expr         = 0,
        .custom_expr  = { 0 },
    };
    struct optparse options;
    optparse_init(&options, argv);

    // argument
    char opt;
    while ((opt = optparse(&options, "hvlngrRbc::E:W:f:")) != -1) {
        switch (opt) {
            case 'h':;
                fumo_who helper_fumo = random_uniform(FUMO_COUNT);
                fumo_help(helper_fumo);
                fumo_expr(helper_fumo, pref.expr, pref.custom_expr);
                fumo_fumo(helper_fumo, fputs);
                return 0;

            case 'l':
                fumo_list();
                return 0;

            case 'n':
                pref.no_wrap = true;
                break;

            case 'W':
                pref.width = pref.no_wrap ? pref.width : numberize(optarg);
                break;

            case 'f':;
                char *token    = strtok(options.optarg, ",;|");
                short count    = 0;
                short size     = 4;
                short *choices = malloc(size * sizeof(short));
                if (!choices) {
                    FUMO_QUIT("ᗜxᗜ: fumo panic - memory allocation failed: processing '-f'.\n");
                }
                while (token) {
                    int choice = fumo_picker(token);
                    if (choice > -1) {
                        choices[count] = choice;
                        count += 1;
                        if (count == size) {
                            size *= 2;
                            choices = realloc(choices, size);
                        }
                    }
                    token = strtok(NULL, ",;/");
                }
                pref.fumo = choices[random_uniform(count)];
                free(choices);
                break;

            case 'g':
                pref.display_name = true;
                break;

            case 'c':
                if (options.optarg) {
                    pref.color          = COLOR_CUSTOM;
                    char *token         = strtok(optarg, ",.;/|");
                    pref.custom_color.R = strtol(token, NULL, 0);
                    if ((token = strtok(NULL, ",.;/|"))) {
                        pref.custom_color.G = strtol(token, NULL, 0);
                    }
                    if ((token = strtok(NULL, ",.;/|"))) {
                        pref.custom_color.B = strtol(token, NULL, 0);
                    }
                } else {
                    pref.color = COLOR_ON;
                }
                break;

            case 'r':;
                pref.rainbow = true;
                break;

            case 'R':;
                pref.rainbow      = true;
                pref.rainbow_fumo = true;
                break;

            case 'b':;
                pref.bold = true;
                break;

            case 'E':;
                int optlen = strlen(options.optarg);
                if (optlen > 1) {
                    // use custom expression
                    short padding = 24 - strlen(options.optarg);
                    if (padding < 0) {
                        // ensure non-negative padding & truncate string
                        padding = options.optarg[24] = 0;
                    }
                    sprintf(
                        pref.custom_expr,
                        "%s%.*s",
                        options.optarg,
                        padding,
                        "                             "
                    );
                } else {
                    pref.expr = options.optarg[0];
                }
                break;

            case 'v':;
                puts(VERSION_STRING);
                return 0;

            case '?':
                fprintf(stderr, "ᗜ_ᗜ: %s\n", options.errmsg);
                return 1;
        } // switch
    }

    if (pref.width < 3) {
        pref.width = 3;
    }

    // fumo reads
    int word_count     = 0;
    char **word_vector = NULL;
    if (options.optind == argc) {
        // read from stdin
        while (1) {
            char *token;
            token = getInput(stdin, 64);
            if (!token) {
                FUMO_QUIT("ᗜxᗜ: fumo panic - memory allocation failed: getInput.\n");
            }
            if (0 == strlen(token)) {
                free(token);
                break;
            }
            if (pref.no_wrap) {
                token = replaceTab(token, 8);
                if (!token) {
                    FUMO_QUIT("ᗜxᗜ: fumo panic - memory allocation failed: replaceTab.\n");
                }
            }
            word_vector = realloc(word_vector, (word_count + 1) * sizeof(char *));
            if (!word_vector) {
                FUMO_QUIT("ᗜxᗜ: fumo panic - memory allocation failed: realloc.\n");
            }
            word_vector[word_count] = token;
            word_count += 1;
        }

    } else {
        // read from cmd line arguments
        word_count = argc - options.optind;

        // I can't realloc argv so this'll have to do
        word_vector = malloc(word_count * sizeof(char *));
        if (!word_vector) {
            FUMO_QUIT("ᗜxᗜ: fumo panic - memory allocation failed: malloc.\n");
        }
        memcpy(word_vector, argv + options.optind, word_count * sizeof(char *));

        char *newline = NULL;
        for (int i = 0; i < word_count; i += 1) {
            // remove newlines from cmd message
            while ((newline = strchr(word_vector[i], '\n'))) {
                *newline = ' ';
            }
            // replace tabs with spaces
            if (strchr(word_vector[i], '\t') == NULL) {
                continue;
            }
            char *token    = strdup(word_vector[i]);
            word_vector[i] = replaceTab(token, 8);
        }
    }

    // cut lines into words
    if (!pref.no_wrap) {
        word_vector = splitWords(&word_count, word_vector, pref.width);
    }

    // calculate the width of the bubble
    int bubble_width = longestLineWidth(word_count, word_vector, pref.width) + 1;

    // choose a fumo if not already chosen
    pref.fumo = pref.fumo == -1 ? random_uniform(FUMO_COUNT) : pref.fumo;

    // finalize expression
    fumo_expr(pref.fumo, pref.expr, pref.custom_expr);

    // check for special fumos
    bool is_byakuren = false;
    switch (fumo_check_special(pref.fumo)) {
        case SPECIAL_CHIMATA:
            pref.rainbow = true;
            break;

        case SPECIAL_BYAKUREN:
            is_byakuren = true;
            /* falls through */
        case SPECIAL_NONE:
            break;
    }

    int (*fumo_say)(const char *, FILE *) =
        pref.rainbow ? (is_byakuren ? &lolbyakuren : &lolfumo) : &fputs;
    int (*fumo_draw)(const char *, FILE *) =
        pref.rainbow_fumo ? (is_byakuren ? &lolbyakuren : &lolfumo) : &fputs;

    if (pref.bold) {
        fputs("\033[1m", stdout);
    }

    if (pref.color == COLOR_ON) {
        SET_COLOR(FUMO_LIST[pref.fumo]->color);
    } else if (pref.color == COLOR_CUSTOM) {
        SET_COLOR(pref.custom_color);
    }

    // CYOA mode
    if (pref.display_name) {
        fumo_say(FUMO_LIST[pref.fumo]->name[0], stdout);
        fumo_say(" says:\n", stdout);
    }

    // top line
    fumo_say(" ", stdout);
    for (int i = 0; i < bubble_width; i += 1) {
        fumo_say("_", stdout);
    }
    fumo_say("\n", stdout);

    // message
    int last_section = 0, cur_section = 0;
    char **process = word_vector;
    char *nl;
    for (int i = 0; i < word_count; i += 1) {
        cur_section += 1;
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
                pref.no_wrap,
                options.optind != argc,
                fumo_say
            );
            last_section = cur_section;
            cur_section  = 0;
        }
    }

    // bottom line
    fumo_say(" ", stdout);
    for (int i = 0; i < bubble_width; i += 1) {
        fumo_say("-", stdout);
    }

    // reset color
    if (pref.color || (pref.rainbow && !pref.rainbow_fumo)) {
        RESET_FORMAT;
        if (pref.bold) {
            fputs("\033[1m", stdout);
        }
    }

    fumo_fumo(pref.fumo, fumo_draw);

    if (pref.rainbow_fumo || pref.bold) {
        RESET_FORMAT;
    }

    free(word_vector);
    return 0;
}
