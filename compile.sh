cd build
make -j4
if [ $? -eq 0 ]; then
	cd ../
	./build/game
else
	cd ../
fi