redo-ifchange $1.cpp

CFLAGS="-O3 -DGL_GLEXT_PROTOTYPES"
INCLUDE="-I./src -I./lib/boost"

g++ $CFLAGS $INCLUDE -MD -MF $1.d -c -o $3 $1.cpp

read DEPS <$1.d
redo-ifchange ${DEPS#*:}
rm $1.d
