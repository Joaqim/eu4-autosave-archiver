#mkdir -p build; cd build

if [ "$1" == "ycm" ]; then
	./autogen.sh
	ycm_generator -f -b autotools --out-of-tree .
	exit 0
fi

if [ "$1" == "clean" ]; then 
	git clean --exclude=data/savefile_dirs.txt -xi
	exit 0
fi

./autogen.sh
cd src

if [ "$1" == "win" ]; then
	../configure --prefix=/mingw64
else
#	../configure --prefix=/usr --enable-gl3=yes
	../configure --prefix=/usr -C
	#NOTE: flag -C enables use of cache, will need to manually delete cache if new package is installed
	# might not work with some third party macros [https://www.gnu.org/savannah-checkouts/gnu/autoconf/manual/autoconf-2.69/html_node/Caching-Results.html#Caching-Results]
fi
make -j6
#./main

#git submodule foreach git clean -xdf
