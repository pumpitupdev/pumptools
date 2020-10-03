#!/bin/env bash

if [ ! "$1" ]; then
	echo "Usage: $0 <directory>"
	exit 1
fi

for SRC in `find $1 -depth`
do
    DST=`dirname "${SRC}"`/`basename "${SRC}" | tr '[A-Z]' '[a-z]'`
    if [ "${SRC}" != "${DST}" ]
    then
        [ ! -e "${DST}" ] && mv -T "${SRC}" "${DST}" || echo "${SRC} was not renamed"
    fi
done
