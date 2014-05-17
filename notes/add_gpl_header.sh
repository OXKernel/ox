#!/bin/bash
CSRC=`find . -name '*.c'`
ASRC=`find . -name '*.s'`
HSRC=`find . -name '*.h'`
H2SRC=`find . -name '*.inc'`

for f in $CSRC
do
    printf "processing %s\n" $f
    cp -f $f $f.gpl_bak
    cat ./gpl >> $f.gpl_tmp
    cat $f >> $f.gpl_tmp
    mv -f $f.gpl_tmp $f
done

for f in $ASRC
do
    printf "processing %s\n" $f
    cp -f $f $f.gpl_bak
    cat ./gpla >> $f.gpl_tmp
    cat $f >> $f.gpl_tmp
    mv -f $f.gpl_tmp $f
done

for f in $HSRC
do
    printf "processing %s\n" $f
    cp -f $f $f.gpl_bak
    cat ./gpl >> $f.gpl_tmp
    cat $f >> $f.gpl_tmp
    mv -f $f.gpl_tmp $f
done

for f in $H2SRC
do
    printf "processing %s\n" $f
    cp -f $f $f.gpl_bak
    cat ./gpla >> $f.gpl_tmp
    cat $f >> $f.gpl_tmp
    mv -f $f.gpl_tmp $f
done
