#! /bin/bash

awk '{ if($1!="#" && $2 != "#" ) { if($2 < $1) print $2 " " $1; else if($2!=$1) print $1 " " $2;  } }' $1 | sort -u > $2
