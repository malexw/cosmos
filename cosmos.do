for file in `find . -name libraries -prune -o -name '*.cpp' -print`; do
	DEPS="$DEPS ${file%.cpp}.o ";
done

redo-ifchange $DEPS

LIBS="-lGL -lGLU -lSDL_image -lopenal -lalut"

g++ $DEPS $LIBS -o $3
