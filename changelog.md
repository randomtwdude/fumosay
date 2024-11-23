### v1.2.2 - 2024/11/23
My Youtube Music Recap is 46% Touhou.
- Added 4 new fumos:<br>
  Yuuma, Doremy, Seiga, and Kosuzu!
- Fixed `fsgq2.py` after I forgot to revert the debugging changes again

### v1.2.1 - 2024/10/15
MORE FUMOS
- Added 4 new fumos:<br>
  Byakuren, Medicine, Shinmyoumaru, and Ellen!<br>
  Byakuren even has her own special rainbow colors.
- The 'Reimu says:' text (with `-g`) will now be colored when used with rainbows
- Fixed wrong fumo when the message contains the word `to` in `fsgq2.py`

### v1.2 - 2024/08/09
In remembrance of Hieda no Akyuu.<br>
This is *the* big update until I figure out how to make animated fumos!
- Added Unicode support:<br>
  - Provided by GNU `libunistring`
  - UTF-8 is assumed
  - Everything should display properly now.
- Rainbows & colors work with unicode too!<br>
  - ANSI escape sequences no longer breaks offset
  - You can finally pipe colored fumosay output into fumosay
![Fumoception](https://github.com/randomtwdude/fumosay/blob/main/images/fumoception.png?raw=true)
- Added 4 new fumos:<br>
  - Mystia's Izakaya at the Human Village: Mystia, Kyouko, Akyuu, and Keine!
- Fixed broken output when reading from the shell in Windows (CR strikes again)
- Messages from the command line will now be properly split by space
- No more (unintentional) memory leaks
- Reorganized code files:<br>
  - `fumosay.c` can now compile with the Cosmopolitan toolchain
  - no longer including C files in other C files
- `fsgq2.py` replaces `fsgq.sh`

### v1.1.10 - 2024/07/21
Finally, fumo completeness.
- Added 4 new fumos:<br>
  Orin, Parsee, Nue, and Chimata!
- The lunar rainbow is now available with `-r`:<br>
  - Always active for Chimata
  - `-R` to also make the fumo rainbow
  - Now requires the math library when compiling
- Bold font is now available with `-b`
- The `-f` option is now a real fuzzy search. (Bitap wow!)
  - fumo data now supports multiple names
- Various fumo touch-ups:<br>
  all fumo now have odd number gap in the center (Reimu, Flan, and Tewi were the exceptions)
  Sanae and others gets more hair
- Number of memory leaks reduced by 1 (one), not that it matters anyways.

### v1.1.9 - 2024/07/13
- Added 4 new fumos:<br>
  Seija, Sagume, Nazrin, and Okina!
- Hina has a more proper bow now.
- Added all fumos to the repo.

### v1.1.8 - 2024/06/15
Almost there!
- Added 4 new fumos:<br>
  Kogasa, Okuu, Rinnosuke, and Hecatia!
- Tweaks for Youmu and Keiki
- Technical stuff:<br>
  code for the Cosmopolitan build is now included in the tar package.<br>
  Prebuilt binary now works on Android.

### v1.1.7 - 2024/05/20
#### 05/24 - fixed fumo_picker() not accepting extra words
Ghost of Tsushima may affect the release schedule.
- Added 3 new fumos!<br>
  Shiki Eiki, Keiki, and Hina joins the roster!
- Added `-v`: display version.

### v1.1.6 - 2024/05/14
Help me, Eirin!
- Added 3 new fumos!<br>
  Welcome Renko, Maribel, and Junko to the roster!
- Touched up some other fumos.<br>
  Kaguya, Shion, Eirin, and Miko looking nicer.

fsgq:
- Fixed fumosay installation (hopefully)
- Fixed Gensoquote error message showing

### v1.1.5 - 2024/04/29
History.
- Added 3 new fumos!<br>
  Welcome, Kasen, Miko, and Futo!
- No longer uses Autotools.

fsgq:
- Added to the source tree.
- Handle characters without quotes.

### v1.1.4 - 2024/04/11
It's the Youkai Mountain special.
- Welcome 3 new fumos!<br>
  We have Momiji, Hatate, and Nitori.
- Tabs now behave properly (they align)<br>
  This works in messages from command line arguments, too. (unlike newlines, which continue to be broken)

### v1.1.3 - 2024/03/23
This update is much delayed because it kept segfaulting. Unsigned integers and arithmetics DO NOT mix.
- Brought 4 new fumos. (32/51)<br>
  Friends from the Moon - Reisen (the Gensokyo one) and Eirin.
  Accesories included - Yuuka with a flower and Kokoro with her mask.
- A little bit cleaner code.
- Windows support is now provided by [Cosmopolitan libc](https://github.com/jart/cosmopolitan)<br>
  This one file should run on Linux, Windows, Mac OS, and (Free/Open/Net)BSD, really cool.<br>
  ~~Bonus: won't be misidentified as a virus.~~

### v1.1.2 - 2024/03/13
- Added 3 new fumos. (28/51)<br>
  Welcome Chen, Ran, and Rumia!
- Switched the random source to `arc4random`.<br>
  No longer produces the same result when run twice in one second.
- Minor tweaks for Reimu.
- FIXED: specifying `-f` with unrecognised names causing a crash due to division by zero.
- FIXED: specifying `-c` with less than 3 numbers causing out-of-bound memory access.
- FIXED: accidentally pressed TAB on Flandre while editing...
- Flandre Scarlet now goes by her full name.
- Changed default color of Yukari to a lighter purple.
- Introducing `fsgq`, the fumosay-gensoquote wrapper script.

### v1.1.1 - 2024/03/06
With this update we begin the quest to add every Touhou fumo... with Suika and Shion!

### v1.1 - 2024/02/26
Just a minor update because no classes.
- The `-c` option now accepts optional custom colors. For example, `-c200,150,230` gives a nice purple.
- New fumofumos!<br>
  Welcome Suwako and Meiling!
- Mini tweaks.

### v1.0 - 2024/01/07
The fumos thank you for having them with you! The fumos have elected to bump the version number to 1.
> More fumos to come still!
- New word-wrapping algorithm should produce more "rectangular" and better-looking paragraphs.<br>
  Also fixes a bug where the text box does not render correctly if the last word in the message is longer than line width.
- The `-f` option now accepts names of multiple fumos, separated by `,`. One of them will be chosen at random.
- Added two more expression presets.<br>
  | preset | description |
  | ------ | ----------- |
  |  `b`   | baka        |
  |  `r`   | random      |

- New fumofumos!<br>
  Fumos from heavens and earth: Tenshi, Cirno, Aya, Sakuya, and Inu Sakuya make this version *STRONGER* than ever.
- Art tweaks<br>
  Patchy got more tweaks.
- The help text is now provided by a fumo.
- Technical changes:<br>
  Package built.
- (a day later) now on the AUR!

### v0.8 - 2023/12/24
Merry Christmas! In festive times like this, every fumo deserves a smile...<br>
> Try options `-f patchy -E w` for an incredibly derpy Patchouli :p
- Added `-E`<br>
  Finally, a unique feature of fumosay! Now, even the most grumpy fumos can smile like a carefree baka.<br>
  You may choose a one-letter preset from the following:
  | preset | description |
  | ------ | ----------- |
  |  `1`   | frowny :(   |
  |  `2`   | meh :\|      |
  |  `3`   | smile :)    |
  |  `4`   | big smile   |
  |  `v`   | vampire!    |
  |  `w`   | bnuuy www   |

  Or specify your own by giving a string longer than 1 character, the first 24 characters will be split into 3 lines in the fumo.<br>
  Each line is 8 characters, with the center generally in the 5th position. Shorter inputs are automatically padded to length with whitespaces.
- Added a manual page with examples, you can view it with `man ./fumosay.1`.
- New fumofumos!<br>
  Bunny Tewi, Yukari, Satori, and Alice!
- Technical changes:<br>
  The first three occurences of `E` in fumo arts are now reserved for the new expression feature.

### v0.7 - 2023/12/19
- Added `-l`<br>
  Lists all available fumos.
- New fumofumos!<br>
  Introducing the Underworld -- Saigyouji Yuyuko & Konpaku Youmu,<br>
  the senior vampire -- Remilia Scarlet,<br>
  and the other shrine maiden -- Kochiya Sanae!
- Art tweaks<br>
  Kaguya and Patchouli received slight tweaks.
- Technical changes:<br>
  All fumo related data is now in `fumo.fumo` for easier access.<br>
  Cleaner code.<br>
  Added a license.

### v0.6 - 2023/12/13
- Added `-g`<br>
  Displays the name of the fumo at the top like it's a story game.
- Added `-c`<br>
  Colors the text the fumo says for some flair. (The fumo isn't colored.)<br>
  Should work on most terminal emulators.
- New fumofumos!<br>
  Kaguya and Mokou have arrived, they won't kill each other here.

### v0.5 - 2023/12/08
- Added `-f`<br>
  Pick a fumo you want. This isn't a real search, it just matches letters.
- New fumofumos!<br>
  Joon Yorigami and Koishi Komeiji are here! Bringing the total fumo count to 6.

### v0.4 - 2023/12/01
- Fumos will now respect *all* your line breaks. (slightly different from cowsay)
- Added `-n`<br>
  cowsay parity: disables word-wrapping altogether, allowing arbitrary whitespaces
  Don't use with `-W` (I don't know what will happen)
- New fumofumos!<br>
  Marisa Kirisame and Flandre Scarlet have agreed to come to your terminal!
- Art tweaks<br>
  Reimu and Patchouli received slight tweaks.
- Technical changes<br>
  Fumos are now stored in a separate file. Not one file for each fumo though, that's kind of hard.

### v0.3 - 2023/10/27
- Now fumos can read from standard input!<br>
  Fumos can now read from files and other programs.
  Newlines in the input are simply ignored for now.

### v0.2 - 2023/10/20
- Added: `-W`<br>
  cowsay parity: specifies where the message should be wrapped
- Fixed issues with really narrow text boxes.

### v0.1 - 2023/10/19
- Fixed issues with the line wrapping algorithm.
- Slight art changes.

### v0.0 - 2023/10/19
The first working(ish) version.
