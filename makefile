BUILD_TYPE="debug"

#-------------------------------------------------------------------
#  Part 2: Invoke the call to make in the build directory
#-------------------------------------------------------------------
.PHONY: default
default: mon

.PHONY: config, configure
config:configure
build/CMakeCache.txt: configure
configure:
	cd build && cmake -GNinja -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..

.PHONY: build
build/src/ingest: build
build/src/trackmon: build
build: build/CMakeCache.txt
	cd build && ninja

ingest: build/src/ingest
	build/src/ingest -v

mon: build/src/trackmon
	build/src/trackmon

# # install dependencies
provision: 
	echo 'dependency packages do not yet exist :('
#	dnf install libais-devel

clean: 
	rm -rf build/*

