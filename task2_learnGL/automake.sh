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
    exit 1
else
    g++ -o $1 $1.cpp ./third_part_src/glad.c ./tools_src/shader.cpp -lglfw3 -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor
fi

if [ $? = 0 ]; then
    mv $1 ./output
    # ./output/$1
fi
