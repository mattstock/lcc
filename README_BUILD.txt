In addition to the changes to support compiling 32 bit on a 64 bit platform, here's the commands to actually to the BUILD-LCC.sh script:

TMPDIR=/tmp CC=gcc CFLAGS="-g -m32" ./BUILD-LCC.sh --exec-prefix=/home/mstock/lcc

or if you only want to do the basic build to modify the md files you can get away with:

export BUILDDIR=~/lcc
export LCCDIR=$BUILDDIR
make CFLAGS="-g -m32" HOSTFILE="etc/bexkat.c" all
