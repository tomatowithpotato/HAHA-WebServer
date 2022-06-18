cmake_minimum_required(VERSION 3.0)
project(haha)

set(CMAKE_VERBOSE_MAKEFILE ON)                  # make 过程中显示信息

# 设置编译参数
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++2a -Wall -Wno-deprecated -Werror -Wno-unused-function \
-Wno-builtin-macro-redefined")

include_directories(. ./haha/include)
# include_directories(${YAML_CPP_INCLUE})
include_directories(/usr/local/include)             # 添加头文件路径
link_directories(/usr/local/lib)                    # 添加库文件路径


aux_source_directory(${PROJECT_SOURCE_DIR}/haha/src SRC_LIST)        # 将制定路径内的源文件全部加入变量中
aux_source_directory(${PROJECT_SOURCE_DIR}/haha/src/base SRC_BASE_LIST)        # 将制定路径内的源文件全部加入变量中
aux_source_directory(${PROJECT_SOURCE_DIR}/haha/src/http SRC_http_LIST)        # 将制定路径内的源文件全部加入变量中

set(
    LIB_SRC
    ${SRC_LIST}
    ${SRC_BASE_LIST}
    ${SRC_http_LIST}
)

MESSAGE( STATUS "LIB_SRC = ${LIB_SRC}.")

add_library(haha SHARED ${LIB_SRC})                                # SHARED 指示生成so文件 即动态链接库

set(
    ALL_LIBS
    haha
    pthread
    uuid
)

add_executable(httpServerTest.out tests/test_httpServer.cc)             # 设置要生成可执行文件的代码
add_dependencies(httpServerTest.out haha)                               # 设置依赖关系
target_link_libraries(httpServerTest.out ${ALL_LIBS})                   # 设置要链接的库

add_executable(dogServletTest.out tests/test_servlet.cc)                # 设置要生成可执行文件的代码
add_dependencies(dogServletTest.out haha)                               # 设置依赖关系
target_link_libraries(dogServletTest.out ${ALL_LIBS})                   # 设置要链接的库

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)                   # 设置可执行文件的位置
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)                      # 设置动态链接库的位置