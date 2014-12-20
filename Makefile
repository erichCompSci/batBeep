PWD = $(shell pwd)


batbeep: batbeep.c
	gcc -o batbeep batbeep.c 

all: batbeep
	@echo Finished!

clean:
	@rm -f batbeep
	@clear; ls --color=auto

install:
	@test -x $(PWD)/batbeep || { echo "Compile the batbeep program first"; exit 1; }
	@test -f $(PWD)/batbeep.desktop || { echo "You are missing the batbeep.desktop file."; exit 1; }
ifeq ($(XDG_CURRENT_DESKTOP),Unity)
	@sudo cp -f $(PWD)/batbeep /bin/batbeep
	@cp -f $(PWD)/batbeep.desktop $(HOME)/.config/autostart/
	@echo "Succesfully installed"
else
	@echo "You do not have a Unity desktop, probably...can't install the batbeep daemon automatically"
endif



