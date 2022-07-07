SOURCE_DIR = ../haha_json

SOURCE_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)

INCLUDE_DIR = -I ../haha_json

BINARY_DIR = ../bin

parse_test: test_json.cpp ${SOURCE_FILES}
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_json.cpp $(SOURCE_FILES) -o $(BINARY_DIR)/parseTest.out

lang_test: test_lang.cpp
	g++ -std=c++2a -ggdb test_lang.cpp -o $(BINARY_DIR)/langTest.out