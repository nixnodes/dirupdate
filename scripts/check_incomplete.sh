#!/bin/bash
#
## Checks a release for incomplete/corrupt files by comparing SFV data with filesystem
#
## Usage: /glroot/bin/dirupdate -d -exec "/glftpd/bin/scripts/check_incomplete.sh '{dir}' '{exe}' '{glroot}'"
#
#
## Verbose output
VERBOSE=0
#
## Optional corruption checking
CHECK_CORRUPT=0
#
#########################################################################


[ -z "$1" ] && exit 1
[ -z "$2" ] && exit 1

GLROOT=$3

DIR=$GLROOT$1
EXE=$2

[ -n "$4" ] && VERBOSE=1

! [ -d "$DIR" ] && exit 1

proc_dir() {
	for i in $1/*; do
		if [ -f "$i" ] && echo $i | grep -P "\.sfv$" > /dev/null; then
			while read l; do
				FFL=$(echo $l | sed 's/ [A-Fa-f0-9]*$//')
				FCRC=$(echo $l | grep -o -P "[A-Fa-f0-9]*$")
				FFT=$(dirname $i)/$FFL
				! [ -f "$FFT" ] && echo "WARNING: $DIR: incomplete, missing file: $FFL" && continue
				[ $CHECK_CORRUPT -gt 0 ] && CRC32=$($EXE --crc32 $FFT) && [ $CRC32 != $FCRC ] && echo "WARNING: $DIR: corrupted: $FFL, CRC32: $CRC32, should be: $FCRC" && continue
				[ $VERBOSE -gt 0 ] && echo "OK: $FFL: $FCRC"
			done < "$i"
		else
			pdbn=$(basename "$i")
			[ -d "$i" ] && [[ "$pdbn" != ".." ]] && [[ "$pdbn" != "." ]] && proc_dir $i
		fi
	done
}

proc_dir $DIR

exit 1