OUT=_build/notify-forwarder

SRCS=src/watch_fsevents.cc \
	src/watch_null.cc \
	src/notify_udp.cc \
	src/receive_udp.cc \
	src/inject_utimes.cc \
	src/main.cc \
	src/main_receive.cc \
	src/main_watch.cc

OBJS=$(SRCS:.cc=.o)
DEPFILES=$(SRCS:.cc=.d)

CXXWARNINGS=-Weverything -Wno-c++98-compat -Wno-weak-vtables -Wno-padded
WATCH_PLUGIN_TYPE?=FSEventsWatchPlugin

CXXFLAGS=-MD -std=c++11 -stdlib=libc++ -g -O0 $(CXXWARNINGS) -D WATCH_PLUGIN_TYPE=$(WATCH_PLUGIN_TYPE)
LDFLAGS=-framework CoreServices

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
