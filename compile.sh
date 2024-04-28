#!/bin/bash
rm ./tmp/main.o 1>& /dev/null
rm ./lib/snllib.o 1>&/dev/null

if [ $# -ne 2 ];then
	echo "Usage : "$0" snl_cource_code output"
	exit 1
fi

src=$1
dst=$2

./snl_compiler $src >./tmp/main.s
#
i686-linux-gnu-gcc ./src/snllib.c -masm=intel -fno-stack-protector -c -o ./lib/snllib.o
i686-linux-gnu-as ./tmp/main.s -o ./tmp/main.o
i686-linux-gnu-ld ./tmp/main.o ./lib/snllib.o -o $dst
