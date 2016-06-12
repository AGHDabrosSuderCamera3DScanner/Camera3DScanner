all:
	 $(MAKE) -C app/src/modules/communication
	 $(MAKE) -C app/src/modules/core
	 $(MAKE) -C app/src/modules/display
	 $(MAKE) -C app/src/modules/persistance
	 $(MAKE) -C app/src/lib
	 $(MAKE) -C app/src/main_linux
	
clean:
	 $(MAKE) -C app/src/modules/communication clean
	 $(MAKE) -C app/src/modules/core/ clean
	 $(MAKE) -C app/src/modules/display clean
	 $(MAKE) -C app/src/modules/persistance clean
	 $(MAKE) -C app/src/lib clean
	 $(MAKE) -C app/src/main_linux clean
	 
run:
	/bin/bash -c "echo; time app/src/main_linux/main"
