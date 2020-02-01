.DEFAULT_GOAL := all
LDFLAGS= -lboost_program_options 

all : shell379 

shell379 : main.cpp InputParser.cpp CommandExecutor.cpp 
	# Need C++14 or higher  
	g++ -g -std=c++17 -I include -pipe -o shell379 main.cpp InputParser.cpp CommandExecutor.cpp ${LDFLAGS} 

clean : 
	rm -f *.o shell379  