#!/usr/bin/env bash
## fsgq 	- fumosay & gensoquote wrapper script
# ver 4 	- May 14 2024
# target 	- fumosay v1.1.6 + gensoquote v3+

if [[ "$1" =~ help ]]; then
	printf -- "fsgq - fumosay & gensoquote wrapper script\n"
	printf -- "Usage: fsgq.sh [\"debug\"] [character] [options for fumosay]\n"
	printf -- "------------------------------------------\n"
	printf -- "Touhou in console with just the dialogue!\n"
	exit 0
fi

# check for dependencies
if ! command -v -- fumosay > /dev/null 2>&1; then
	printf -- "Missing dependency: fumosay.\n"
	# wow
	printf -- "Would you like this script to install it?\n"
	read -r -p "This will install: fumosay. [y/N] " resp

	if [[ "$resp" =~ ^([yY][eE][sS]|[yY])$ ]]; then

		if ! command -v -- mktemp > /dev/null 2>&1; then
			printf -- "Missing mktemp! Aborting...\n"
			exit 1
		fi

		read -r -p 'Choose an installation location (the executable will be installed to this-path/bin) (default: $HOME/.local): ' location
		if [[ ! $location ]]; then
			location="$HOME/.local"
		fi

		# CHANGE ME
		ver=1.1.6 # LOOK HERE! <=======

		printf -- "===> Retreiving package...\n"
		workdir=$(mktemp -d)
		trap 'rm -rf -- "$workdir"' EXIT

		cd "$workdir"
		wget "https://github.com/randomtwdude/fumosay/releases/download/fumo${ver}/fumosay-${ver}.tar.gz" -q --show-progress \
		|| curl -OL "https://github.com/randomtwdude/fumosay/releases/download/fumo${ver}/fumosay-${ver}.tar.gz"

		if [[ $(sha256sum fumosay-${ver}.tar.gz | awk '{print $1;}') -ne "35c7bfd91aaecf200fb2e4725828b634e6d439f68f93133a2894136051a9fb64" ]]; then
			printf -- "Integrity check failed!\n"
			exit 2
		fi

		tar xf "fumosay-${ver}.tar.gz" && cd "fumosay-${ver}" || exit 2
		gcc -o fumosay fumosay.c && sudo install -Dm755 fumosay -t "$location" || exit 2
		printf -- "===> fumosay installed to %s !\n" "$location"

		if ! command -v -- fumosay > /dev/null 2>&1; then
			printf -- "BUT it is not in path!\n"
			printf -- "Hint: You need to add %s/bin to your PATH variable.\n" "$location"
			exit 1
		fi

	else
		# no install
		exit 1
	fi
fi

if ! command -v -- gensoquote > /dev/null 2>&1; then
	printf -- "Missing dependency: gensoquote\n"
	# wow
	printf -- "Would you like this script to install it?\n"
	read -r -p "This will install: gensoquote. [y/N] " resp

	if [[ "$resp" =~ ^([yY][eE][sS]|[yY])$ ]]; then
		# check if we have cargo
		if ! command -v -- cargo > /dev/null 2>&1; then
			printf -- "Missing cargo! Please install rustup with your package manager of choice.\n"
			printf -- "Or follow the official guide at https://www.rust-lang.org/tools/install\n"
			exit 1
		fi

		if ! command -v -- mktemp > /dev/null 2>&1; then
			printf -- "Missing mktemp! Aborting...\n"
			exit 1
		fi


		read -r -p 'Choose an installation location (default: $HOME/.local/bin): ' location
		if [[ ! $location ]]; then
			location="$HOME/.local/bin"
		fi

		printf -- "===> Retreiving source...\n"
		workdir=$(mktemp -d)
		trap 'rm -rf -- "$workdir"' EXIT

		cd "$workdir"
		git clone https://github.com/dmyTRUEk/gensoquote && cd gensoquote || exit 2
		cargo build -r && mkdir -p "$location" && cp "./target/release/gensoquote" "$location/gensoquote" || exit 2
		printf -- "===> gensoquote installed to %s !\n" "$location"

		if ! command -v -- gensoquote > /dev/null 2>&1; then
			printf -- "BUT it is not in path!\n"
			printf -- "Hint: You need to add %s to your PATH variable.\n" "$location"
			exit 1
		fi

	else
		exit 1
	fi
fi

### ACTUAL CONTENT ###

if [[ "$1" =~ debug ]]; then
	debug_mode=1
	shift 1
fi

# list of available fumos
fumolist=$(fumosay -l | tail -n +3)
if [[ ! $fumolist ]]; then
	printf -- "Fumosay is slient. Oh no!\n"
	exit 1
fi

# filter fumo list
if [[ $1 ]] && [[ ! $1 =~ ^- ]]; then
	fumo_specified=1
	list=$(printf -- "$fumolist" | grep -i "$1")
	if ! [[ -n $list ]]; then
		printf -- "Can't find this character in fumosay! Defaulting...\n"
		list="$fumolist"
	fi
else
	list="$fumolist"
fi

quote_found=0
while true; do
	# get a random fumo from filtered list
	fumo=$(printf -- "$list" | shuf -n 1 )

	if [[ $fumo =~ Puppo ]]; then
		fumo="Sakuya"
	fi

	if [[ $debug_mode ]]; then
		printf -- "[DEBUG] Called character: \"%s\"\n" "$fumo"
	fi

	# call gensoquote
	gq=$(gensoquote -c $fumo 2>/dev/null)

	# check
	if ! [[ -n $gq ]]; then
		list=$(printf -- "$list" | sed "/$fumo/d")
		if ! [[ -n $list ]]; then
			printf -- "No quotes found for any specified character!\n"
			exit 1
		fi
	else
		break
	fi
done

# capture character name
# We have to do this because fumosay only accepts the entire name
sayer=$(echo "$gq" | grep -o -P "(?<=^\-- )([\w\s](?! to))+\w")

# captures anything between the quotes
saying=$(echo "$gq" | grep -o -P "(?<=^\").*(?=\")")

# we have two sakuyas
if [[ "$sayer" =~ Sakuya ]] && (( $RANDOM % 2 == 1 )); then
	sayer="puppo"
fi

# say
if [[ $fumo_specified ]]; then
	shift 1
fi
echo "$saying" | fumosay -f "$sayer" $@

if [[ $debug_mode ]]; then
	printf -- "[DEBUG] Original gensoquote:\n%s" "$gq"
fi

exit 0
