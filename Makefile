CFLAGS = -std=c99 -g -Wall -Wshadow --pedantic -Wvla -Werror
COVFLAGS = -fprofile-arcs -ftest-coverage
GCC = gcc $(CFLAGS) $(COVFLAGS)
OBJS = pa13.o bmp.o filter.o #validate.o

pa13: $(OBJS) bmp.h filter.h
	$(GCC) $(OBJS) -o pa13

vtester: bmp.o filter.o validate.o bmp.h filter.h
	$(GCC) bmp.o filter.o validate.o -o vtester

pa13.o: pa13.c bmp.h filter.h
	$(GCC) -c pa13.c -o pa13.o

bmp.o: bmp.c bmp.h filter.h
	$(GCC) -c bmp.c -o bmp.o

filter.o: filter.c bmp.h filter.h
	$(GCC) -c filter.c -o filter.o

validate.o: validate.c bmp.h filter.h
	$(GCC) -c validate.c -o validate.o


# "make test" will run the three input files
test: pa13
	-./pa13 inputs/img1.bmp output1.bmp edge.filter
	-./pa13 inputs/img1.bmp output2.bmp sharpen.filter
	-./pa13 inputs/img1.bmp output3.bmp blur.filter
	-./pa13 inputs/book.bmp output4.bmp edge.filter

test2: vtester
	-./vtester inputs/img2.bmp output2.bmp

holder:

	#-./pa13 inputs/test2 output2
	#-./pa13 inputs/test3 output3
	#-./pa13 inputs/test4 output4
	#-./pa13 inputs/test5 output5
	#-./pa13 inputs/test6 output6
	#-./pa13 inputs/test7 output7
	#-./pa12 inputs/test8
	#-./pa12 inputs/test9
	#-./pa12 inputs/test10
	#-./pa12 inputs/test11
	#-./pa12 inputs/test12

# "make memory" calls valgrind to check memory errors
memory: pa13
	valgrind --leak-check=full --show-reachable=yes ./pa13 inputs/img1.bmp output1.bmp sharpen.filter
	#valgrind --leak-check=full --show-reachable=yes ./pa13 inputs/img2.bmp output2.bmp sharpen.filter
	#valgrind --leak-check=full --show-reachable=yes -v ./pa13 inputs/test3 output3
	#valgrind --leak-check=full --show-reachable=yes -v ./pa13 inputs/test4 output4
	#valgrind --leak-check=full --show-reachable=yes -v ./pa13 inputs/test5 output5
	#valgrind --leak-check=full --show-reachable=yes -v ./pa13 inputs/test6 output6
	#valgrind --leak-check=full --show-reachable=yes -v ./pa13 inputs/test7 output7
	#valgrind --leak-check=full --show-reachable=yes -v ./pa12 inputs/test8
	#valgrind --leak-check=full --show-reachable=yes -v ./pa12 inputs/test9
	#valgrind --leak-check=full --show-reachable=yes -v ./pa12 inputs/test10
	#valgrind --leak-check=full --show-reachable=yes -v ./pa12 inputs/test11
	#valgrind --leak-check=full --show-reachable=yes -v ./pa12 inputs/test12
tmemory: pa13
	valgrind --leak-check=full --show-reachable=yes -v ./pa13 inputs/test8

clean:
	/bin/rm -f *.o
	/bin/rm -f testgen
	/bin/rm -f *.gcda *.gcno gmon.out *gcov
	/bin/rm -f *.out
	/bin/rm -f output1.bmp output2.bmp output3.bmp output4.bmp pa13 vtester
