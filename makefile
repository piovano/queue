test: test_block test_typed test_simultaneous test_performance

test_block: test_block.o queue_union.o queue_memory.o queue_multi_file.o queue_file.o queue_abstract.o
	g++ -g -pthread test_block.o queue_union.o queue_memory.o queue_multi_file.o queue_file.o queue_abstract.o -o test_block

test_block.o: test_block.cpp queue_union.h queue_memory.h queue_multi_file.h queue_file.h queue_abstract.h queue_common.h
	g++ -g -c test_block.cpp

test_typed: test_typed.o queue_union.o queue_memory.o queue_multi_file.o queue_file.o queue_abstract.o
	g++ -g -pthread test_typed.o queue_union.o queue_memory.o queue_multi_file.o queue_file.o queue_abstract.o -o test_typed

test_typed.o: test_typed.cpp queue_union.h queue_memory.h queue_multi_file.h queue_file.h queue_abstract.h queue_common.h
	g++ -g -c test_typed.cpp

test_simultaneous: test_simultaneous.o queue_memory.o queue_abstract.o
	g++ -g -pthread test_simultaneous.o queue_memory.o queue_abstract.o -o test_simultaneous

test_simultaneous.o: test_simultaneous.cpp queue_memory.h queue_abstract.h queue_common.h
	g++ -g -c test_simultaneous.cpp

test_performance: test_performance.o queue_union.o queue_memory.o queue_multi_file.o queue_file.o queue_abstract.o
	g++ -g -lrt -pthread test_performance.o queue_union.o queue_memory.o queue_multi_file.o queue_file.o queue_abstract.o -o test_performance

test_performance.o: test_performance.cpp queue_union.h queue_memory.h queue_multi_file.h queue_file.h queue_abstract.h queue_common.h
	g++ -g -c test_performance.cpp

queue_union.o: queue_union.h queue_union.cpp queue_memory.h queue_multi_file.h queue_file.h queue_abstract.h queue_common.h
	g++ -g -c queue_union.cpp

queue_memory.o: queue_memory.h queue_memory.cpp queue_abstract.h queue_common.h
	g++ -g -c queue_memory.cpp

queue_multi_file.o: queue_multi_file.h queue_multi_file.cpp queue_file.h queue_abstract.h queue_common.h
	g++ -g -c queue_multi_file.cpp

queue_file.o: queue_file.h queue_file.cpp queue_abstract.h queue_common.h
	g++ -g -c queue_file.cpp

queue_abstract.o: queue_abstract.h queue_abstract.cpp queue_common.h
	g++ -g -c queue_abstract.cpp

clean:
	rm -rf *.o