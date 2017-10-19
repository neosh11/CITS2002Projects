PROJECT = shell
HEADER = myshell.h mystuff.h
OBJ = execute.o globals.o myshell.o parser.o mystuff.o

C99 = cc -std=c99
FLAGS = -Wall -pedantic -Werror 

$(PROJECT) : $(OBJ)
	$(C99) $(FLAGS) -o $(PROJECT) $(OBJ)

%.o : %.c $(HEADER)
	$(C99) $(FLAGS) -c $<

clean :
	rm -f $(PROJECT) $(OBJ)