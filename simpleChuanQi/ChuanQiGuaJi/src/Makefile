OBJS = ../../base/base.o ../../base/log.o ../../base/mysqlCtrl.o ../../base/logIntf.o
OBJS += libevent-net/listener.o libevent-net/net_util.o libevent-net/read_callback.o
OBJS += tinyxml/tinystr.o tinyxml/tinyxml.o tinyxml/tinyxmlerror.o tinyxml/tinyxmlparser.o
OBJS += game/protocal.o game/db.o game/roleCtrl.o game/factionCtrl.o game/mainServer.o game/j2.o game/config.o  game/rank.o  game/shop.o  game/broadCast.o
OBJS += game/skill.o  game/itemCtrl.o game/friend.o game/arena.o  game/item.o game/mailCtrl.o game/sign.o game/jsonpacket.o  game/mineCtrl.o game/activity.o
OBJS += game/task.o game/quickfight.o game/fightCtrl.o game/role.o game/roleskill.o game/common.o game/mercenary.o game/packMgr.o game/rolerune.o game/TimerMgr.o
OBJS += game/roleWorldBoss.o game/WorldBoss.o
OBJS += game/database.o game/logs_db.o 
OBJS += game/service.o  game/backend_session.o  game/input_stream.o  game/output_stream.o  game/session.o

STATIC_LIB = $(PRJ_ROOT)/depends/libs/linux/libmysqlclient_r.a $(PRJ_ROOT)/depends/inc/LIBEVENT-LINUX/libevent.a

LDFLAGS = -ldl -lpthread -lm -lz -lrt -luuid

TARGET = ./miniSvrGame

$(TARGET): $(OBJS) .depend
	$(CXX) -static $(OBJS) $(STATIC_LIB) $(LDFLAGS) -o $@

include makefile.common


