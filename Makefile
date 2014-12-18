batbeep: batbeep.c
	gcc -o batbeep batbeep.c 

all: batbeep
	@echo Finished!

clean:
	@rm batbeep
	@clear; ls --color=auto

install:
	@test -w /etc/rc.local || { echo "rc.local either does not exist or you are not root"; exit 1; }
	
