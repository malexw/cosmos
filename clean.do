for f in `find . -name libraries -prune -o -name '*.o' -print`; do
    rm $f;
done
for f in `find . -name libraries -prune -o -name '*.s' -print`; do
    rm $f;
done
rm -f cosmos
