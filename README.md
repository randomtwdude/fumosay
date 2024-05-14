# fumosay
Like the classic cowsay, but featuring Touhou fumos, written in C.

```
Written in the spirit of funkyness.
```

<img src="https://github.com/randomtwdude/fumosay/assets/105645765/2375b4d5-abe7-47ef-97c7-2fedc9ba84a9" width=750>

## Features
- Fumo<br>
  41 adorable fumos come to your machine!
- Expressions<br>
  Have you seen a Patchouli fumo going :3 ?
- Fast & efficient<br>
  Glorious C speeds.
- Fancy algorithms<br>
  Uses a minimum raggedness algorithm like TeX. (no other fancies of TeX though)
- Portable<br>
  Runs on almost all your systems thanks to [Cosmopolitan libc](https://github.com/jart/cosmopolitan).

## Fumos
The fumos are designed with the font Cascadia Code.

<details>
  <summary>List of all available fumos!</summary>

  - Hakurei Reimu
  - Patchouli Knowledge
  - Kirisame Marisa
  - Flandre Scarlet
  - Yorigami Joon
  - Komeiji Koishi
  - Houraisan Kaguya
  - Fujiwara no Mokou
  - Saigyouji Yuyuko
  - Konpaku Youmu
  - Remilia Scarlet
  - Kochiya Sanae
  - Inaba Tewi
  - Yakumo Yukari
  - Komeiji Satori
  - Alice Margatroid
  - Hinanawi Tenshi
  - Cirno
  - Shameimaru Aya
  - Izayoi Sakuya
  - Inu Sakuya
  - Moriya Suwako
  - Hong Meiling
  - Ibuki Suika
  - Yorigami Shion
  - Chen
  - Ran Yakumo
  - Rumia
  - Reisen Udongein Inaba
  - Yagokoro Eirin
  - Kazami Yuuka
  - Hata no Kokoro
  - Inubashiri Momiji
  - Himekaidou Hatate
  - Kawashiro Nitori
  - Ibaraki Kasen
  - Toyosatomimi no Miko
  - Mononobe no Futo
  - Usami Renko
  - Maribel Hearn
  - Junko
</details>

## Updates
More or less weekly update. See [changelog](changelog.md).

## Install
Fumosay is now on the Arch User Repository: `paru fumosay`<br>
For other Linux users:
1. Download the latest package from the [release](https://github.com/randomtwdude/fumosay/releases) section.
2. Extract the package (`tar xf fumosay-{version}.tar.gz`)
3. `cd fumosay-{version}`
```
gcc -o fumosay fumosay.c
# move the fumos somewhere comfy (like ~/.local/bin)
```

For other platforms, Actually Portable Executable (updated to `v1.1.6`) [available](https://github.com/randomtwdude/fumosay/releases/tag/fumo1.1.6).

## Use
- `fumosay Hello World!` | `fumosay -n < textfile` | `echo Hi | fumosay -f tenshi`
### `fsgq` fumosay-gensoquote wrapper script
`fsgq.sh` is a shell script that displays Touhou quotes from [gensoquote](https://github.com/dmyTRUEk/gensoquote) with the corresponding fumo. Grab it from the [release](https://github.com/randomtwdude/fumosay/releases/tag/fumo1.1.3) section.<br>

To use it, give it execute permission: `chmod +x fsgq.sh`, then simply run it: `./fsgq.sh`. You can also choose a specific character and pass flags like `./fsgq.sh marisa -gc`.
> :warning: `fumosay`, unlike `gensoquote`, is not memory-safe :p

Have fun!

> The contents of this program may not reflect the true personal views of the characters featured.
