#!/usr/bin/env python
## fsgq2 - fumosay & gensoquote wrapper: Enhanced!
# ver 2 - Oct 15 2024

## debug is now '--debug' (with double-dash)
## you can specify multiple characters now
## automatic expression based on message (powered by VADER, `pip install vaderSentiment`)

import argparse
import subprocess
import random
import re
import sys

from vaderSentiment.vaderSentiment import SentimentIntensityAnalyzer

# init
parser = argparse.ArgumentParser(
    description="fumosay & gensoquote wrapper",
    usage="fsgq2.py [--debug] [character(s)] [flag(s) for fumosay]"
)
parser.add_argument("--debug", action='store_true', help="Output more info.")
parser.add_argument("character", nargs='*', help="Pick some characters if you want.")

analyzer = SentimentIntensityAnalyzer()

# parse arguments
args, flags = parser.parse_known_args()

# get the list of fumos
fumo_list = subprocess.run(["fumosay", "-l"], stdout=subprocess.PIPE).stdout.decode("utf-8")
fumo_list = fumo_list.split('\n')[2:-1]

# refine the list of fumos
if args.character:
    fumo_list_bak = fumo_list
    fumo_list = [fumo for fumo in fumo_list if fumo.casefold() in map(str.casefold, args.character)]
    if not fumo_list:
        print("Can't find " + " or ".join(args.character) + " in fumosay! Defaulting...\n")
        fumo_list = fumo_list_bak

while True:
    fumo = random.choice(fumo_list)
    if fumo == "puppo":
        fumo = "Sakuya"
    if args.debug:
        print("[DEBUG] Chosen - " + fumo)

    # get quote
    gq = (subprocess.run(["gensoquote", "-c", fumo],
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL).stdout.decode("utf-8"))

    # check
    if (gq == ""):
        fumo_list.remove(fumo)
        if not fumo_list:
            print("No quotes found for any specified character!")
            sys.exit(1)
    else:
        break


# extract quote
sayer  = re.search(r"(?<=\n-- )([\w\s](?! to))+\w", gq).group(0)
saying = re.search(r"(?<=^\").*(?=\")", gq).group(0)

# gimmick
ps = analyzer.polarity_scores(saying)

match ps["compound"]:
    case p if p <= -0.1:
        expr = '1'
    case p if p > -0.1 and p <= 0.25:
        expr = ''
    case p if p > 0.25 and p <= 0.4:
        expr = '3'
    case p if p > 0.4 and p <= 0.8:
        expr = '4'
    case _:
        expr = 'b'

# call fumos
if "Sakuya" in sayer and random.randint(0, 10) < 2:
    sayer = "puppo"

subprocess.run(["fumosay", "-f", sayer, "-E", expr] + flags, input=saying.encode("utf-8"))

if args.debug:
    print("[DEBUG] Original gensoquote:\n" + gq)
    print("VADER says:")
    print(str(ps))
