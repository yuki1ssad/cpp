.PHONY: run clean
ALL = main

run:
	@g++ $(ALL).cpp -o $(ALL).o -std=c++11 -lpthread
	@./$(ALL).o
	@make clean

clean:
	@rm *.o

# g++ main_7.cpp -o main_7.o -std=c++11 -lpthread -g && gdb ./main_7.o