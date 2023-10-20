// fumosay
// like cowsay, but with funky fumos!
// fairly barebones for now, more updates to come.

/* ===== INCLUDES ===== */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

/* ===== DEFINES ===== */
#define FUMO_COUNT 2
int MAX_WIDTH = 80;
enum fumo_who {Reimu, Patchy};

/* ===== FUNCTIONS ===== */
#define MIN(a,b) (a > b ? b : a)
#define MAX(a,b) (a > b ? a : b)

void helpInfo() {
  printf("Usage: fumosay [-h] [-W column] *message*\n"
         "-W     Specifies roughly where the message should be wrapped.\n"
         "       The default is 80.\n"
         "In spirit of the original cowsay, made by Tony Monroe in 1999.\n"
         "Fumos are characters from Touhou Project.\n");
}

// string to number
int numberize(char *c) {
  long number = strtol(c, (char **)NULL, 0);
  if (number == 0) {
    fprintf(stderr, "Invalid parameter: %s", c);
    exit(EXIT_FAILURE);
  }
  return (int)number;
}

// todo: move to own file
void fumofumo(enum fumo_who fm) {
  switch (fm) {
  case Reimu:
    fputs(R"(
       \
        \                                                *******
         \         *@@@@*                           .*@@*       @=@@
          \    #@@@      @@@@@@+.                @@@              @: %=
           \  @  @                  =@@-     @@.                 *@   @.
              @  @*                      @@@                      @@   @
              @   @                  :@@@@@%%%@@@@+               .@   @
              @  @              =@@.                 %@+         .%    @
              @  =@+         @@                          @@        @  .*
              -+   @       @.                              .@     @.  @
               @. @      %-                                  .@.@@  -@
                @. @% %@@             %         #                  %@
                 #@.@:  @    @       #%@        -        .   @   @   .@
                 =@     @ @  #.   _@#   @-     @.@@=_     @  @@  @     .@
               .@       .@ @  @@==        %@#=@      ###@@- @  +@.       -@
              @=        @    @@:==========      ==========+@    @          @
             @.         *    %.  @  *    @      @  *    @  @    @           @
            *-          @     #  @*#     @      @**     @  @    @         ##-@
            @.          %     @  @.     @       *@      @  @    @.  ##@---   @
           #: -@##_   _.@     @   @=__=@          @=__=@   @   = @--      %@@
            *@_   *@#-  @     @                            @    @  @  @@@+
               @@_     @.     @                            @    @   **
                  +@##-  *@@%%*            ####             @:  /
                         @    @@_                        _@.@+@@
                          *@@@#  #@@.                 @@#   @  -*
                          /   @     @.*@@@=    =@@@* @      @   #
                          @   @    #*      @- @.     :@     @   @
                          @,  #      \###@@@   @:@==/       *@*
                            *^             @. *.
                                          =    @.
                                         @      @
                                        #@@-__-@@#
    )", stdout);
    break;
  case Patchy:
    fputs(R"(
       \
        \                                  +--*##=_
         \                       ___         *+    #@
          \                 +@#@@  #*_==-_##  =     #@#=*
           \          __ .@*                   %    .%   @.
            \        @  @:                    @      @    :#
             \      @   #               @=__+*-     .@     @+=#@
                   #    @                +_        _@      #   *#
                 __*                       *.=___=+             @_=**_
              @@-  -*=_*                                    @  @ *    *#
           .@     @* %  #==    ==@#@,    @#@    .@@+.   _*@@ == *#@     @
          @:     @  @      *##*      ---#   #==-     *#*        @  @     #
           =@  .@   #  *       .      .       .      :.     %   .#  @  @=
             @=@    @  #       *      #       #      :-      @  @   *#@
                     # .      =:      @       @       +      @ =
                       *-----*  *----* +-----+ *----* +----**
                      #@    @ ==========     ========== *    *
                      @     @  @ #    %       @ %    %  @    #.
                      @     @  @+     @       #*     @  @    #.
                      *.   .*  @      #       @      @  @    @
                       *   *    #=__=#         #=__=#   *.  @
                  #= %@.@@ _*=*.                    _*==_ @@ ##==
                 *     @  @    @          n        #     @  @    *#
                 *.    _@@.    *                   @.   # @@ +    @
                  #@__*#  #*=_# * #=,           ,=#_ =-+ #  # #--*
                       #  # ___      #==,___,==#     _   #  #
                  *%%=* @@ *   #                  .:# #** @@ **#%#=
                 @     @  @     @ #=_   @ * @   _= @     @  @      @
                 *   ,+ @@ *__-+     **=@ * @=**   @  .,# @@ +_   @
                  +*#  +  +             *   *       **=  +  +  =**
                       @  @             @-_-@            @  @
                        ##                                ##
    )", stdout);
    break;
  }
}

// returns length of longest line
size_t longestLineWidth(int argc, char **argv) {
  size_t cur_line = 0;
  size_t max_line = 0;
  for (int i = 0; i < argc; i++) {
    size_t word_len = strlen(argv[i]) + 1;
    // if word is longer than max, return max
    if (word_len > MAX_WIDTH) {
      return MAX_WIDTH;
    }
    if (cur_line + word_len < MAX_WIDTH) {
      // add this word
      cur_line += word_len;
      max_line = MAX(cur_line, max_line);
    } else {
      // count a new line
      max_line = MAX(cur_line, max_line);
      cur_line = word_len;
    }
  }
  return max_line;
}

void paddedBreak(int padding) {
  for (int i = 0; i < padding; i++) {
    printf(" ");
  }
  printf(")\n");
}

void printMessage(int argc, char **argv, size_t bubble_size) {
  size_t cur_line = 0;
  for (int i = 0; i < argc; i++) {

    size_t word_len = strlen(argv[i]) + 1;

    // left edge
    if (cur_line == 0) {
      printf("( ");
    }

    // word fits
    if (cur_line + word_len <= bubble_size) {
      printf("%s ", argv[i]);
      // terminate line if at max length
      if (cur_line + word_len == bubble_size) {
        paddedBreak(0);
        cur_line = 0;
        continue;
      }
      cur_line += word_len;
      // last word
      if (i == argc - 1) {
        paddedBreak(bubble_size - cur_line);
      }
      continue;
    }
    // word doesn't fit this line
    // break if line isn't empty
    if (cur_line > 0) {
      paddedBreak(bubble_size - cur_line);
      printf("( ");
    }
    // now we're in a new line
    if (word_len > bubble_size) {
      // word is too long for a single line
      char *word = argv[i];
      size_t out = 0;
      for (int j = 0;; j++) {
        size_t len = MIN(bubble_size - 1 /* leave a blank for looks */, strlen(word));
        printf("%.*s", (int)len, word);
        paddedBreak(bubble_size - len);
        word += len;
        out += len;
        if (out >= word_len - 1) {
          break;
        }
        printf("( ");
      }
      cur_line = 0;
    } else {
      printf("%s ", argv[i]);
      cur_line = word_len;
      if (cur_line == bubble_size || i == argc - 1) {
        paddedBreak(bubble_size - cur_line);
        cur_line = 0;
      }
    }
  }
}

int main(int argc, char **argv) {
  // parse arguments
  if (argc == 1) {
    helpInfo();
    exit(EXIT_SUCCESS);
  }
  // random init
  srand(time(NULL));
  // argument
  char opt;
  while ((opt = getopt(argc, argv, "hW:")) != -1) {
    switch (opt) {
    case 'h':
      helpInfo();
      exit(EXIT_SUCCESS);
    case 'W':
      MAX_WIDTH = numberize(optarg);
      if (MAX_WIDTH < 2) {
        printf("The fumos hath spoken: Width must be at least 2!\n");
        exit(EXIT_FAILURE);
      }
      break;
    }
  }
  size_t bubble_width = longestLineWidth(argc - optind, argv + optind) + 1;
  // top line
  printf(" ");
  for (int i = 0; i < bubble_width; i++) {
    printf("_");
  }
  printf("\n");
  // message
  printMessage(argc - optind, argv + optind, bubble_width - 1);
  // bottom line
  printf(" ");
  for (int i = 0; i < bubble_width; i++) {
    printf("-");
  }
  // fumo
  enum fumo_who fm = rand() % FUMO_COUNT;
  fumofumo(fm);
  // bye
  return 0;
}