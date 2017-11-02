 #CITS2002 Project 2 2017
 #Names:             Neosh Sheikh, Cameron Wright
 #Student number(s):   21959462, 21958958
 #Date:                2/11/2017
 
PROJECT = shell
HEADER = myshell.h mystuff.h execute.h
OBJ = execute.o globals.o myshell.o parser.o mystuff.o processArray.o arrayList.o

C99 = cc -std=c99
FLAGS = -Wall -pedantic -Werror 

$(PROJECT) : $(OBJ)
	$(C99) $(FLAGS) -o $(PROJECT) $(OBJ)

%.o : %.c $(HEADER)
	$(C99) $(FLAGS) -c $<

clean :
	rm -f $(PROJECT) $(OBJ)
