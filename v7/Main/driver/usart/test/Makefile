# Makefile
# all:より下の3行のインデント、
# gtest-gen:より下の1行のインデント
# は半角空白になっていますがタブ1個にしてください
# target.cはCなのでgccで、それ以外はC++なのでg++でビルドします
# gtest_main.ccはGoogleTestが用意してくれているmain関数、
# gtest-all.ccがGoogleTest全部入りファイルです
# -lpthreadをつけることにも注意。
# makeかmake allしたらビルドして実行まで行います。

INCLUDE = -I. -I../../../include
GTEST_DIR = ~/

all:
	gcc -c ../usart_driver.c $(INCLUDE)
	g++ test_usart.cpp $(GTEST_DIR)/googletest/googletest/src/gtest_main.cc gtest/gtest-all.cc $(INCLUDE) -lpthread usart_driver.o 
	./a.out

clean:
	rm *.o *.out

gtest-gen:
	python3 $(GTEST_DIR)/googletest/googletest/scripts/fuse_gtest_files.py ./
