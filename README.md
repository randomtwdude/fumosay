# fumosay
Like cowsay, but featuring Touhou fumos, written in C.

```
Written in the spirit of funkyness.
```

![Kettle](https://github.com/randomtwdude/fumosay/blob/main/images/fumosay-1.2.1.png?raw=true)

## Features
- ᗜˬᗜ<br>
  Adorable fumos on your terminal!
- Expressions<br>
  Have you seen a Patchouli fumo going :3?
- Fast & efficient<br>
  Gloriously unsafe C incantations.
- Fancy algorithms<br>
  Uses a minimum raggedness algorithm like TeX. (no other fancies of TeX though)
- Rainbows and Unicode<br>
  UTF-8 is now supported!
- Portable<br>
  Works on all your OSes. (antiviruses don't like it though)

## Fumos
They look best with the font [Cascadia Code](https://github.com/microsoft/cascadia-code).

Every Touhou character with a Gift fumo is included, *plus*:
- Mystia
- Kyouko
- Akyuu (R.I.P.)
- Keine
- Byakuren
- Medicine
- Shinmyoumaru
- Ellen

## Updates
More or less weekly update. See [changelog](changelog.md).

## Install
Fumosay is now on the Arch User Repository: `paru fumosay`

For other Linux users:
1. Download the latest package from the [release](https://github.com/randomtwdude/fumosay/releases) section.
2. Extract the package (`tar xf fumosay-{version}.tar.gz`)
3. `cd fumosay-{version}`
```
gcc -o fumosay fumosay.c fumoutil.c fumolang.c -lm -lunistring
# move the fumos somewhere comfy (like ~/.local/bin)
```

For other platforms, Actually Portable Executable [available](https://github.com/randomtwdude/fumosay/releases/tag/fumo1.2) for version 1.2.

> The APE is outdated because I forgot how to build it :skull:

## Use
- `fumosay Hello World!` | `fumosay -n < textfile` | `echo Hi | fumosay -f tenshi`

### `fsgq` fumosay-gensoquote wrapper script
`fsgq2.py` is a python script that displays Touhou quotes from [gensoquote](https://github.com/dmyTRUEk/gensoquote) with the corresponding fumo. <br>

To use it, give it execute permission: `chmod +x fsgq2.py`, then simply run it: `./fsgq2.py`. You can also choose characters and pass flags like `./fsgq2.py reimu marisa -gc`.
> :warning: `fumosay`, unlike `gensoquote`, is not memory-safe :p

Have fun!

> The contents of this program may not reflect the true personal views of the characters featured.
