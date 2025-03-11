#!/bin/sh

if [ -z $1 ]; then
	echo "No sch file indicated"
	echo "usage:"
	echo "annotate file"
	echo "	file is generated by gschem"
	exit 1
fi

cat $1 | sed "/refdes=\([A-Za-z][A-Za-z]*\)[0-9][0-9]*/s//refdes=\1?/g" > $1.tmp
#mv $1.tmp $1
