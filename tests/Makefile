SOURCE_DIR = ../haha/src

SOURCE_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)
SOURCE_FILES += $(wildcard $(SOURCE_DIR)/*/*.cpp)

INCLUDE_DIR = -I ../haha/include

BINARY_DIR = ../bin

lang: test_lang.cc
	g++ -ggdb test_lang.cc -o lang

timestamp: test_timeStamp.cc
	g++ -ggdb test_timeStamp.cc -o timestamp

tcpserver: test_tcpServer.cc
	g++ -ggdb $(INCLUDE_DIR) test_tcpServer.cc $(SOURCE_FILES) -lpthread -o $(BINARY_DIR)/tcpserverTest.out

thread: test_thread.cc
	g++ -ggdb $(INCLUDE_DIR) test_thread.cc $(SOURCE_FILES) -lpthread -o $(BINARY_DIR)/threadTest.out


threadpool: test_threadpool.cc
	g++ -ggdb $(INCLUDE_DIR) test_threadpool.cc $(SOURCE_FILES) -lpthread -o $(BINARY_DIR)/threadpoolTest.out

log: test_log.cc
	g++ -ggdb $(INCLUDE_DIR) test_log.cc $(SOURCE_FILES) -lpthread -o $(BINARY_DIR)/loglTest.out