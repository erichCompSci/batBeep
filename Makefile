batbeep: batbeep.c
	gcc -o batbeep batbeep.c 

all: batbeep
	@echo Finished!

clean:
	@rm batbeep
	@clear; ls --color=auto

