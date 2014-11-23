In addition to the changes to support compiling 32 bit on a 64 bit platform, here's the commands to actually to the BUILD-LCC.sh script:

TMPDIR=/tmp CC=gcc CFLAGS=-g -m32 ./BUILD-LCC.sh --exec-prefix=/home/mstock/lcc x86-linux
