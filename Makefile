OUT=_build/notify-forwarder

SRCS=src/watch_null.cc \
	src/notify_udp.cc \
	src/receive_udp.cc \
	src/inject_utimes.cc \
	src/main.cc \
	src/main_receive.cc \
	src/main_watch.cc

OSNAME=$(shell uname -s)

CXXFLAGS=-MD -g -O0 -std=c++11

ifeq ($(OSNAME),Darwin)
	SRCS+=src/watch_fsevents.cc

	CXXWARNINGS=-Weverything -Wno-c++98-compat -Wno-weak-vtables -Wno-padded
	LDFLAGS=-framework CoreServices
	CXXFLAGS+=-stdlib=libc++

	WATCH_PLUGIN_TYPE=FSEventsWatchPlugin
endif

ifeq ($(OSNAME),Linux)
	WATCH_PLUGIN_TYPE=NullWatchPlugin
endif

ifeq ($(OSNAME),FreeBSD)
	CXXWARNINGS=-Weverything -Wno-c++98-compat -Wno-weak-vtables -Wno-padded
	CXXFLAGS+=-stdlib=libc++

	WATCH_PLUGIN_TYPE=NullWatchPlugin
endif

CXXFLAGS+=$(CXXWARNINGS) -D WATCH_PLUGIN_TYPE=$(WATCH_PLUGIN_TYPE)

OBJS=$(SRCS:.cc=.o)
DEPFILES=$(SRCS:.cc=.d)

_build/notify-forwarder: _build $(OBJS)
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

_build:
	mkdir _build

clean:
	rm -f $(OBJS)
	rm -f $(DEPFILES)
	rm -rf _build

all: $OUT

.PHONY: all clean

-include $(DEPFILES)
