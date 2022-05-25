SOURCE_DIR = ./haha/src
SOURCE_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)
SOURCE_FILES += $(wildcard $(SOURCE_DIR)/*/*.cpp)

INCLUDE_DIR = -I haha/include

tcpserver: test_tcpServer.cc
	g++ -std=c++14 -ggdb $(INCLUDE_DIR) test_tcpServer.cc $(SOURCE_FILES) -lpthread -o ./bin/tcpserver.out