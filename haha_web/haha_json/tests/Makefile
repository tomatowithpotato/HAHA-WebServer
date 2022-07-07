SOURCE_DIR = ../haha_json

SOURCE_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)

INCLUDE_DIR = -I ../haha_json

BINARY_DIR = ../bin

parse_test: test_parse.cpp ${SOURCE_FILES}
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_parse.cpp $(SOURCE_FILES) -o $(BINARY_DIR)/parseTest.out