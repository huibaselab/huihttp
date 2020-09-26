###
 # @Author: Tom Hui
 # @Date: 2019-12-16 20:41:56
 # @Description: 
 ###
#!/bin/bash

apt-get install perl -y
apt-get install -y libmysqlclient-dev

NOW_DIR=`pwd`
PRONAME=huihttp
ROOT_DIR=~/huilib
DEP_DIR=${ROOT_DIR}/deps/
DEP_LIBEVENT=${DEP_DIR}libevent
DEP_CONFIG=${DEP_DIR}/libconfig

function mk_if_not()
{
	dd=$1
	if [ ! -d ${dd} ]
	then 
		mkdir -p ${dd}
	fi
}

mk_if_not ${ROOT_DIR}

mk_if_not ${DEP_DIR}

mk_if_not ${ROOT_DIR}/${PRONAME}


if [ ! -d ${DEP_LIBEVENT} ]
then 
    cp ${NOW_DIR}/deps/libevent.tar.gz ${DEP_DIR}/
	cd ${DEP_DIR}
	tar xvfz libevent.tar.gz
	rm -f libevent.tar.gz
	cd libevent
	./configure
	make -j8
	cd ${NOW_DIR}
fi

if [ ! -d ${DEP_CONFIG} ]
then 
    cp ${NOW_DIR}/deps/libconfig.tar.gz ${DEP_DIR}/
	cd ${DEP_DIR}
	tar xvfz libconfig.tar.gz
	rm -f libconfig.tar.gz
	cd libconfig
	./configure
	make -j8
	cd ${NOW_DIR}
fi


cd ${NOW_DIR}
make -j8
make install
cd ${NOW_DIR}
rm -fr *


exit 0