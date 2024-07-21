// -- fumosay --
// like cowsay, but with funky fumos!
// utilities to look for your favorite fumo

// This implementation is translated from github.com/heyimalex/bitap
// #rewriteItInC

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define MIN4(a,b,c,d) (MIN(MIN((a),(b)),MIN((c),(d))))

#define ALPHABET 128
#define WORDLEN 63

// Give pattern, get bitmasks.
uint64_t *get_masks(char *str) {
  uint64_t *masks = malloc(ALPHABET * sizeof(uint64_t));
  memset(masks, -1, ALPHABET * sizeof(uint64_t));
  for (int i = 0; i < strlen(str); i += 1) {
    int index = str[i] >= 65 && str[i] <= 90 ? str[i] + 32 : str[i];
    masks[index % ALPHABET] &= ~(1UL << i);
  }
  return masks;
}

// Bitap algorithm based on Levenshtein distance, optimized for max_dist = 1 or 2
int bitap_levenshtein(char *haystack, uint64_t *masks, int pattern_len, int max_dist) {
  int score = 0;
  int max_distance = MIN(max_dist, pattern_len);
  uint64_t r[3] = {~1UL, ~1UL << 1, ~1UL << 2};
  uint64_t mask;
  for (int i = 0; i < strlen(haystack); i += 1) {
    int index = haystack[i] >= 65 && haystack[i] <= 90 ? haystack[i] + 32 : haystack[i];
    mask = masks[index % ALPHABET];

    uint64_t prev_parent = r[0];
    r[0] |= mask;
    r[0] <<= 1;
    for (int j = 1; j < 1 + max_distance; j += 1) {
      uint64_t prev    = r[j];
      uint64_t current = (prev | mask) << 1;
      uint64_t replace = prev_parent << 1;
      uint64_t delete  = r[j - 1] << 1;
      uint64_t insert  = prev_parent;
      r[j] = current & insert & delete & replace;
      prev_parent = prev;
    }
    for (int j = 0; j < max_distance + 1; j += 1) {
      if (0 == (r[j] & (1UL << pattern_len))) {
        score += pattern_len - MIN(pattern_len, 2 * j);
      }
    }
  }
  return score;
}

// Fuzzy bitap search, max_dist <= 2.
int bitap(char *haystack, char *needle, int max_dist) {
  char *pattern = strndup(needle, WORDLEN);
  uint64_t *m = get_masks(pattern);
  int score = bitap_levenshtein(haystack, m, strlen(pattern), max_dist);
  free(m);
  free(pattern);
  return score;
}