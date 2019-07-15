#!/bin/sh

function usage()
{
    echo "please input what you want to compile"
    echo -e "\t like: $BASH_SOURCE test"
    echo -e "\t then will build test.cpp , and output runable file:test"
    # echo $(basename $0)
    # echo $0
    # echo $BASH_SOURCE
}


if [ ! -n "$1" ] ;then
    usage
else
    g++ -o $1 $1.cpp glad.c -lglfw3 -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor
fi
