// This file contains alternate functions for use when compiled for Windows

/* The original code is public domain -- Will Hartung 4/9/09 */
/* Modifications, public domain as well, by Antti Haapala, 11/10/17
   - Switched to getc on 5/23/19 */

#ifndef WINDOWS_H
#define WINDOWS_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

// if typedef doesn't exist (msvc, blah)
typedef intptr_t ssize_t;

ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    size_t pos;
    int c;

    if (lineptr == NULL || stream == NULL || n == NULL) {
        errno = EINVAL;
        return -1;
    }

    c = getc(stream);
    if (c == EOF) {
        return -1;
    }

    if (*lineptr == NULL) {
        *lineptr = malloc(128);
        if (*lineptr == NULL) {
            return -1;
        }
        *n = 128;
    }

    pos = 0;
    while(c != EOF) {
        if (pos + 1 >= *n) {
            size_t new_size = *n + (*n >> 2);
            if (new_size < 128) {
                new_size = 128;
            }
            char *new_ptr = realloc(*lineptr, new_size);
            if (new_ptr == NULL) {
                return -1;
            }
            *n = new_size;
            *lineptr = new_ptr;
        }

        ((unsigned char *)(*lineptr))[pos ++] = c;
        if (c == '\n') {
            break;
        }
        c = getc(stream);
    }

    (*lineptr)[pos] = '\0';
    return pos;
}

// not having open_memstreams proves Windows is objectively worse
void replace_tabs_windows(char *buffer, char **token, ssize_t bytes, size_t buf_line) {
    size_t pos = 0;
    for (int i = 0; i < bytes + 1; i++) {
        if (buffer[i] == 9) {
            if (pos + 5 > buf_line) {
                *token = realloc(*token, (pos + 10));
            }
            snprintf(*token + pos, 5, "    ");
            pos += 4;
        } else {
            snprintf(*token + pos, 2, "%c", buffer[i]);
            pos += 1;
        }
    }
    *token = realloc(*token, pos);
}

#endif