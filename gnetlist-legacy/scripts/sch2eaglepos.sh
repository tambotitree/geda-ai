#!/bin/sh
# By Braddock Gaskill (braddock@braddock.com), August 2004.  This
# software is hereby declared to be in the public domain by Braddock
# Gaskill, the author.
FNAME="$1"
if [ -z "$FNAME" ]; then
    cat << EOF

$0 <inputfile.sch>

This script will read a gschem schematic and attempt to
extract the relative positions of the components in the schematic,
and generate corresponding MOVE instructions for Eagle.  You will
likely have to adjust XOFFSET, YOFFSET, XSCAL, and YSCALE at the
top of the script to obtain usable positions.

By Braddock Gaskill (braddock@braddock.com), August 2004

EOF
    exit 255
fi
XOFFSET=40000
YOFFSET=33000
#XSCALE=10000
#YSCALE=10000
XSCALE=9000
YSCALE=9000

tmpdir=/tmp/$$
mkdir -m 0700 -p $tmpdir
rc=$?
if test $rc -ne 0 ; then
	cat << EOF

$0: ERROR -- Failed to create $tmpdir with 0700 permissions.  mkdir returned $rc.

Make sure that $tmpdir does not already exist and that you have permissions to
create it.

EOF
	exit 1
fi
tmpf=${tmpdir}/tmpf
grep -B1 refdes= "$FNAME" |sed 's/=/ /' | cut -d" " -f2,3 |grep -v '^--' >${tmpf}


while read REPLY; do
    # the directory on the client to backup
    X=`echo $REPLY | cut -d' ' -f1`
    Y=`echo $REPLY | cut -d' ' -f2`
    read REPLY;
    PART="$REPLY"
    X=`echo "scale=5; ($X - $XOFFSET) / $XSCALE" |bc`
    Y=`echo "scale=5; ($Y - $YOFFSET) / $YSCALE" |bc`
    echo "MOVE '$PART' ($X $Y);"
done < $tmpf
rm -fr "${tmpdir}"
