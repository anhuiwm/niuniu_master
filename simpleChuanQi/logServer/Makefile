OBJS = base/base.o base/log.o base/mysqlCtrl.o base/db.o
OBJS += srcLogserver/logServer.o  srcLogserver/timer.o  srcLogserver/j2.o 

STATIC_LIB = $(PRJ_ROOT)/depends/libs/linux/libmysqlclient_r.a

LDFLAGS = -ldl -lpthread -lm -lz -lrt -luuid -lutil

TARGET = ./svrLog

$(TARGET): $(OBJS)
	$(CXX) -static $(OBJS) $(STATIC_LIB) $(LDFLAGS) -o $@

PRJ_ROOT=/home/simplechuanqi/simpleChuanQi/logServer
SRC_ROOT=$(PRJ_ROOT)/srcLogserver
MYSQL_INC = $(PRJ_ROOT)/depends/inc/mysql-5.1.30-include/

CXX = g++
CXXFLAGS += -g -Wno-unused-function -Wall -fno-strict-aliasing -fPIC -std=c++11 -DUSE_EPOLLWR_IOVEC=0 -DUSE_EPOLLWR_SENDBUF=0 -D__LOGSERVER__  $(INCLUDE)
SRCS = $(patsubst %.o,%.cpp,$(OBJS))

INCLUDE = -I$(MYSQL_INC) -I$(PRJ_ROOT) -I$(PRJ_ROOT)/base -I$(PRJ_ROOT)/rapidjson/include -I$(SRC_ROOT)


clean:
	rm -rf $(OBJS) 

log.o: log.cpp log.h
base.o: base.cpp base.h log.h zlibapi.h
mysqlCtrl.o: mysqlCtrl.cpp mysqlCtrl.h base.h log.h
db.o: db.cpp db.h base.h log.h
logServer.o: logServer.cpp timer.h j2.h logCase.hpp  logDefine.hpp
timer.o: timer.cpp timer.h j2.h
j2.o: j2.cpp j2.h

