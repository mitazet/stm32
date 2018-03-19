# Makefile
# all:より下の3行のインデント、
# gtest-gen:より下の1行のインデント
# は半角空白になっていますがタブ1個にしてください
# target.cはCなのでgccで、それ以外はC++なのでg++でビルドします
# gtest_main.ccはGoogleTestが用意してくれているmain関数、
# gtest-all.ccがGoogleTest全部入りファイルです
# -lpthreadをつけることにも注意。
# makeかmake allしたらビルドして実行まで行います。

TESTNAME = test_usart

GTEST_DIR = ../../../../../..
TEST_DIR = .
CODE_DIR = ..
INC_DIR = ../../..

INCLUDE = -I$(GTEST_DIR) -I$(INC_DIR)/include

SRCS = $(CODE_DIR)/usart_driver.c
OBJECTS = usart_driver.o

all: $(OBJECTS) $(TESTNAME)
	./$(TESTNAME)

$(TESTNAME): $(OBJECTS)
	g++ -o $(TESTNAME) test_usart.cpp $(GTEST_DIR)/googletest/googletest/src/gtest_main.cc $(GTEST_DIR)/gmock-gtest-all.cc $(INCLUDE) -lpthread $(OBJECTS) 

$(OBJECTS): $(SRCS)
	gcc -c $(SRCS) $(INCLUDE)

clean:
	rm *.o $(TESTNAME)

gtest-gen:
	python $(GTEST_DIR)/googletest/googlemock/scripts/fuse_gmock_files.py $(GTEST_DIR) 
