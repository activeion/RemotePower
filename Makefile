CXXFLAGS = -O0 -g3  -pthread 
LDFLAGS = -lpthread -lmuduo_net -lmuduo_base -lz -lprotobuf
BASE_SRC = 
LIB_SRC =
BINARIES = server mobile_client  power_client
HEADERS=$(wildcard *.h)

all:cpp11

cpp11: CXXFLAGS += -I ../build/debug-install-cpp11/include/ -I /usr/include/mysql/ -L ../build/debug-install-cpp11/lib  -L /usr/lib/libmysql*.so
cpp11: protoc_middleman $(BINARIES) 
$(BINARIES): $(HEADERS)
$(BINARIES):
	g++ $(CXXFLAGS) -o $@ $(LIB_SRC) $(BASE_SRC) $(filter %.cc,$^) $(LDFLAGS)


clean:
	rm -f $(BINARIES) core
	rm -f protoc_middleman remote_power.pb.cc remote_power.pb.h

protoc_middleman: remote_power.proto
	protoc --cpp_out=. remote_power.proto
	@touch protoc_middleman

server: CXXFLAGS += -Wall
server: server.cc codec.cc remote_power.pb.cc protoc_middleman

mobile_client: mobile_client.cc codec.cc remote_power.pb.cc protoc_middleman

power_client: power_client.cc codec.cc remote_power.pb.cc protoc_middleman
