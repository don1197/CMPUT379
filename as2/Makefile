.DEFAULT_GOAL := all
LDFLAGS= -lboost_program_options -lpthread
SRC= src/*

all : prodcom 

prodcom : ${SRC} 
	# Need C++14 or higher  
	
	g++ -std=c++17 -I include -o prodcom ${SRC} ${LDFLAGS} 

clean : 
	rm -f *.o prodcom  