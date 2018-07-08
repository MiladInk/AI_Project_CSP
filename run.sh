#! /bin/bash
make $1
if [ "$3" == "nodisp" ]; then
./$1 < $2
else
./$1 < $2 | python3 display.py
fi
