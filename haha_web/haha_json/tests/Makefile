SOURCE_DIR = ../haha_json

SOURCE_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)

INCLUDE_DIR = -I ../haha_json

BINARY_DIR = ../bin

parse_test: test_json.cpp ${SOURCE_FILES}
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_json.cpp $(SOURCE_FILES) -o $(BINARY_DIR)/parseTest.out

cast_test: test_type_cast.cpp ${SOURCE_FILES}
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_type_cast.cpp $(SOURCE_FILES) -o $(BINARY_DIR)/typeCastTest.out

copy_test: test_jsonCopy.cpp
	g++ -std=c++2a -ggdb $(INCLUDE_DIR) test_jsonCopy.cpp $(SOURCE_FILES) -o $(BINARY_DIR)/jsonCopyTest.out

lang_test: test_lang.cpp
	g++ -std=c++2a -ggdb -o0 test_lang.cpp -o $(BINARY_DIR)/langTest.out