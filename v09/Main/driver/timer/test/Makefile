# Makefile
# gtest_main.ccはGoogleTestが用意してくれているmain関数、
# gmock-gtest-all.ccがGoogleTest全部入りファイルです
# -lpthreadをつけることにも注意。
# makeかmake allしたらビルドして実行まで行います。

TESTNAME = test_timer

GTEST_DIR = ../../../../../..
TEST_DIR = .
CODE_DIR = ..
INC_DIR = ../../..

INCLUDE = -I$(GTEST_DIR) -I$(INC_DIR)/include

SRCS = $(CODE_DIR)/timer_driver.c
OBJECTS = timer_driver.o

all: $(OBJECTS) $(TESTNAME)
	./$(TESTNAME)

$(TESTNAME): $(OBJECTS)
	g++ -o $(TESTNAME) test_timer.cpp $(GTEST_DIR)/googletest/googletest/src/gtest_main.cc $(GTEST_DIR)/gmock-gtest-all.cc $(INCLUDE) -lpthread $(OBJECTS) -D DEBUG_GTEST

$(OBJECTS): $(SRCS)
	gcc -c $(SRCS) $(INCLUDE) -D DEBUG_GTEST

clean:
	rm *.o $(TESTNAME)

gtest-gen:
	python $(GTEST_DIR)/googletest/googlemock/scripts/fuse_gmock_files.py $(GTEST_DIR) 
