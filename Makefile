CXXFLAGS = -O0 -g3  -pthread 
LDFLAGS = -lpthread -lmuduo_net -lmuduo_base -lz -lprotobuf -lmysqlclient
BASE_SRC = 
LIB_SRC =
BINARIES = server
HEADERS=$(wildcard *.h)

all:cpp11

cpp11: CXXFLAGS += -std=c++11 -I ../build/debug-install-cpp11/include/ -I /usr/include/mysql/ -L ../build/debug-install-cpp11/lib 

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


