all:
	 $(MAKE) -C app/Camera3D/src/modules/core
	 $(MAKE) -C app/Camera3D/src/lib
	 $(MAKE) -C app/Camera3D/src/main
	
clean:
	 $(MAKE) -C app/Camera3D/src/modules/core/ clean
	 $(MAKE) -C app/Camera3D/src/lib clean
	 $(MAKE) -C app/Camera3D/src/main clean
	 
run:
	/bin/bash -c "echo; time app/Camera3D/src/main/main $(config)"
