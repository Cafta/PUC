# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/cafe/GitHub/PUC/2025_Semaforo_Inteligente

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cafe/GitHub/PUC/2025_Semaforo_Inteligente/build

# Include any dependencies generated for this target.
include CMakeFiles/testa_cuda.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/testa_cuda.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/testa_cuda.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/testa_cuda.dir/flags.make

CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.o: CMakeFiles/testa_cuda.dir/flags.make
CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.o: ../src/testa_cuda.cpp
CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.o: CMakeFiles/testa_cuda.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cafe/GitHub/PUC/2025_Semaforo_Inteligente/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.o -MF CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.o.d -o CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.o -c /home/cafe/GitHub/PUC/2025_Semaforo_Inteligente/src/testa_cuda.cpp

CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cafe/GitHub/PUC/2025_Semaforo_Inteligente/src/testa_cuda.cpp > CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.i

CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cafe/GitHub/PUC/2025_Semaforo_Inteligente/src/testa_cuda.cpp -o CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.s

# Object files for target testa_cuda
testa_cuda_OBJECTS = \
"CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.o"

# External object files for target testa_cuda
testa_cuda_EXTERNAL_OBJECTS =

../testa_cuda: CMakeFiles/testa_cuda.dir/src/testa_cuda.cpp.o
../testa_cuda: CMakeFiles/testa_cuda.dir/build.make
../testa_cuda: /usr/local/lib/libopencv_world.so.4.5.4
../testa_cuda: CMakeFiles/testa_cuda.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/cafe/GitHub/PUC/2025_Semaforo_Inteligente/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../testa_cuda"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testa_cuda.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/testa_cuda.dir/build: ../testa_cuda
.PHONY : CMakeFiles/testa_cuda.dir/build

CMakeFiles/testa_cuda.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/testa_cuda.dir/cmake_clean.cmake
.PHONY : CMakeFiles/testa_cuda.dir/clean

CMakeFiles/testa_cuda.dir/depend:
	cd /home/cafe/GitHub/PUC/2025_Semaforo_Inteligente/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cafe/GitHub/PUC/2025_Semaforo_Inteligente /home/cafe/GitHub/PUC/2025_Semaforo_Inteligente /home/cafe/GitHub/PUC/2025_Semaforo_Inteligente/build /home/cafe/GitHub/PUC/2025_Semaforo_Inteligente/build /home/cafe/GitHub/PUC/2025_Semaforo_Inteligente/build/CMakeFiles/testa_cuda.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/testa_cuda.dir/depend

