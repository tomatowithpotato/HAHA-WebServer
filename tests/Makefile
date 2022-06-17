SOURCE_DIR = ../haha/src

SOURCE_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)
SOURCE_FILES += $(wildcard $(SOURCE_DIR)/*/*.cpp)

INCLUDE_DIR = -I ../haha/include

BINARY_DIR = ../bin

lang: test_lang.cc
	g++ -std=c++2a -ggdb test_lang.cc -o $(BINARY_DIR)/langTest.out

timestamp: test_timeStamp.cc
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_timeStamp.cc $(SOURCE_FILES) -lpthread -luuid -o $(BINARY_DIR)/timeStampTest.out

timerheap: test_timerheap.cc
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_timerheap.cc $(SOURCE_FILES) -lpthread -luuid -o $(BINARY_DIR)/timerheapTest.out

tcpserver: test_tcpServer.cc
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_tcpServer.cc $(SOURCE_FILES) -lpthread -luuid -o $(BINARY_DIR)/tcpserverTest.out

thread: test_thread.cc
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_thread.cc $(SOURCE_FILES) -lpthread -luuid -o $(BINARY_DIR)/threadTest.out


threadpool: test_threadpool.cc
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_threadpool.cc $(SOURCE_FILES) -lpthread -luuid -o $(BINARY_DIR)/threadpoolTest.out

log: test_log.cc
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_log.cc $(SOURCE_FILES) -lpthread -luuid -o $(BINARY_DIR)/loglTest.out

stringview: test_stringview.cc
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_stringview.cc -o $(BINARY_DIR)/stringviewlTest.out


httpRequest: test_httpRequest.cc $(SOURCE_FILES)
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_httpRequest.cc $(SOURCE_FILES) -lpthread -luuid -o $(BINARY_DIR)/httpRequestTest.out

httpCT: test_httpCT.cc $(SOURCE_FILES)
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_httpCT.cc $(SOURCE_FILES) -lpthread -luuid -o $(BINARY_DIR)/httpCTTest.out

httpServer: test_httpServer.cc $(SOURCE_FILES)
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_httpServer.cc $(SOURCE_FILES) -lpthread -luuid -o $(BINARY_DIR)/httpServerTest.out

dogServlet: test_servlet.cc $(SOURCE_FILES)
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_servlet.cc $(SOURCE_FILES) -lpthread -luuid -o $(BINARY_DIR)/dogServletTest.out