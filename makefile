BUILD_TYPE="debug"

#-------------------------------------------------------------------
#  Part 2: Invoke the call to make in the build directory
#-------------------------------------------------------------------
run: build
	ninja 

.PHONY: config, configure
config:configure
build/CMakeCache.txt: configure
configure:
	cd build && cmake -GNinja -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..

.PHONY: build
build: build/CMakeCache.txt
	cd build && ninja

# # install dependencies
provision: 
	echo 'dependency packages do not yet exist :('
#	dnf install libais-devel

clean: 
	rm -rf build/*

