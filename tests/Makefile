SOURCE_DIR = ../src
SOURCE_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)
SOURCE_FILES += $(wildcard $(SOURCE_DIR)/*/*.cpp)

lang: test_lang.cc
	g++ -ggdb test_lang.cc -o lang

timestamp: test_timeStamp.cc
	g++ -ggdb test_timeStamp.cc -o timestamp

tcpserver: test_tcpServer.cc
	g++ -ggdb test_tcpServer.cc $(SOURCE_FILES) -o tcpserver