// -- fumosay: fumoutil.c --
// like cowsay, but with funky fumos!

// Fumo Utilities: Search, expressions, and other fumo related stuff
#include "fumoutil.h"
#include "fumo.fumo"

#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define MIN4(a,b,c,d) (MIN(MIN((a),(b)),MIN((c),(d))))

/* Picks a fumo */
fumo_who fumo_picker(char *str) {
  fumo_who i = 0, fumo = -1;
  int highscore = 0;
  while (i < FUMO_COUNT) {
    int j = 0, name = 0;
    while (name < 10) {
      int score = bitap(str, FUMO_LIST[i]->name[j], 2);
      if (score > highscore) {
        highscore = score;
        fumo = i;
      }
      j += 1;
      if (0 == FUMO_LIST[i]->name[j][0]) {
        break; /* names are ""-terminated */
      }
      name += 1;
    }
    i += 1;
  }
  if (highscore == 0) {
    printf("ᗜ_ᗜ : We cannot find \"%s\"!\n", str);
  }
  return fumo;
}

/* Set fumo expression, modifies string */
void fumo_expr(fumo_who fm, char ex, char *custom) {
  char *p;
  short count = 0;

  // use custom expression
  if (custom[0] != 0) {
    while (count < 3) {
      p = strchr(FUMO_LIST[fm]->fumo, 'E');
      if (!p) {
        return;
      }
      memmove(p, custom + (8 * count++), 8);
    }
    return;
  }

  // use presets
  #define set_expression(x) {                           \
    while (count < 3) {                                 \
      p = strchr(FUMO_LIST[fm]->fumo, 'E');             \
      if (!p) {                                         \
        return;                                         \
      }                                                 \
      memmove(p, EXPRESSIONS[x] + (8 * count++), 8);    \
    }                                                   \
  }

  switch (ex) {
  case '1':
    set_expression(0); break;
  case '2':
    set_expression(1); break;
  case '3':
    set_expression(2); break;
  case '4':
    set_expression(3); break;
  case 'v':
    set_expression(4); break;
  case 'w':
    set_expression(5); break;
  case 'b':
    set_expression(6); break;
  case 'd':
    set_expression(7); break;
  case 'r':;
    int r = random_uniform(EXPRESSION_COUNT);
    set_expression(r);
    break;
  default:
    while (count < 3) {
      p = strchr(FUMO_LIST[fm]->fumo, 'E');
      if (!p) {
        return;
      }
      *p = ' ';
    }
  }
  #undef set_expression
}

/* Lists all fumos */
void fumo_list() {
  for (int i = 0; i < FUMO_COUNT; i += 1) {
    fputs(FUMO_LIST[i]->name[0], stdout);
    fputs("\n", stdout);
  }
}

/* fumo ᗜˬᗜ fumo */
void fumo_fumo(fumo_who fm, int (*fumo_say)(const char *, FILE *)) {
  fumo_say(FUMO_LIST[fm]->fumo, stdout);
}

/* Says a message then exits */
void fumo_panic(char *message, char expr, char *custom_expr) {
  fputs(message, stdout);
  fumo_who helper_fumo = random_uniform(FUMO_COUNT);
  fumo_expr(helper_fumo, expr, custom_expr);
  fumo_fumo(helper_fumo, &fputs);
  exit(EXIT_FAILURE);
}

/* Bitap fuzzy string search
 *
 * This implementation is translated from github.com/heyimalex/bitap
 * #rewriteItInC
 */
// Give pattern, get bitmasks.
uint64_t *_get_masks(char *str) {
  uint64_t *masks = malloc(ALPHABET * sizeof(uint64_t));
  memset(masks, -1, ALPHABET * sizeof(uint64_t));
  for (int i = 0; i < strlen(str); i += 1) {
    int index = str[i] >= 65 && str[i] <= 90 ? str[i] + 32 : str[i];
    masks[index % ALPHABET] &= ~(1UL << i);
  }
  return masks;
}
// Bitap algorithm based on Levenshtein distance, optimized for max_dist = 1 or 2
int _bitap_levenshtein(char *haystack, uint64_t *masks, int pattern_len, int max_dist) {
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
  uint64_t *m = _get_masks(pattern);
  int score = _bitap_levenshtein(haystack, m, strlen(pattern), max_dist);
  free(m);
  free(pattern);
  return score;
}

// New RNG
int random_uniform(int num) {
  if (num < 1) {
    fprintf(stderr, "ᗜ_ᗜ: invalid num passed to random.\n");
    num = FUMO_COUNT;
  }

  uint32_t r;
  uint32_t limit = UINT32_MAX - (UINT32_MAX % num);

  do {
    getrandom(&r, sizeof(r), 0);
    if (r == -1) {
      fumo_panic("ᗜxᗜ: fumo panic - getrandom failed", 0, "");
    }
  } while (r >= limit);

  return r % num;
}