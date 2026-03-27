#!/bin/bash


function readfile()
{
    #`ls $1`
    for file in `ls $1`
    do
        if [ -d $1"/"$file ]
        then
            readfile $1"/"$file
        else
            echo $1"/"$file
           `cat $1"/"$file > $1"/"$file".new"`
           `rm $1"/"$file `
           `mv $1"/"$file".new"  $1"/"$file`
        fi
    done
}


folder="."
echo $0
if [ $# -eq 1 ]
then
    if [ -d $1 ]
    then
        echo "unpack folder "$1
        readfile $1
    else
        echo "unpack document "$1
	`cat $1 > $1".new"`
        `rm $1 `
        `mv $1".new"  $1`
    fi
else
    echo "unpack currentfolder"
    readfile $folder
fi


#dir=.
#for i in `ls ${dir}`
#do
#    echo ${i}
#done

