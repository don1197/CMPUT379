.DEFAULT_GOAL := all
LDFLAGS= -lboost_program_options 

all : shell379 t_looper

shell379 : main.cpp InputParser.cpp CommandExecutor.cpp 
	# Need C++14 or higher  
	g++ -g -std=c++17 -I include -pipe -o shell379 main.cpp InputParser.cpp CommandExecutor.cpp ${LDFLAGS} 

t_looper : t_looper.cpp 
	g++ -o t_looper t_looper.cpp  

clean : 
	rm -f *.o shell379 t_looper 