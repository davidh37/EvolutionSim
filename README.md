Compiling On Windows 10:

Download Msys2
(Run in Msys2 terminal: pacman -S mingw-w64-ucrt-x86_64-gcc)
Open Msys2 mingw64 terminal
(check option keys -> 'Ctrl+Shift+letter shortcuts')
Enter: pacman -S mingw64/mingw-w64-x86_64-SDL2 mingw64/mingw-w64-x86_64-glm mingw64/mingw-w64-x86_64-cmake mingw64/mingw-w64-x86_64-gcc
cd to repository and enter following commands
	mkdir build
	cd build
	cmake ..
	ninja
	./sdl_app.exe
(note that outside of this folder the mingw libstd DLLs and SDL2 DLL need to be copied into the folder)
