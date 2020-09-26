
ROOTPATH=/hui
PRONAME=huihttp
LIB_PATH=${ROOTPATH}/${PRONAME}/lib
INCLUDE_PATH=${ROOTPATH}/${PRONAME}/include
DIRS = src

all:
	for dir in ${DIRS}; do make -C $$dir -j8; echo ; done;


rebuild:
	for dir in ${DIRS}; do make -C $$dir rebuild; echo ; done;

clean:
	for dir in ${DIRS}; do make -C $$dir clean; echo ; done;

install:
	mkdir -p ${LIB_PATH}
	cd lib; cp *.a ${LIB_PATH}/;
	mkdir -p ${INCLUDE_PATH}/;
	cp src/*.h ${INCLUDE_PATH}/;