# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

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
CMAKE_SOURCE_DIR = /home/joseph/git/Ebony-Ayers/fast-thread-syncronization

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build

# Include any dependencies generated for this target.
include src/CMakeFiles/fts.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/fts.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/fts.dir/flags.make

src/CMakeFiles/fts.dir/cmake_pch.hxx.gch: src/CMakeFiles/fts.dir/flags.make
src/CMakeFiles/fts.dir/cmake_pch.hxx.gch: src/CMakeFiles/fts.dir/cmake_pch.hxx.cxx
src/CMakeFiles/fts.dir/cmake_pch.hxx.gch: src/CMakeFiles/fts.dir/cmake_pch.hxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/fts.dir/cmake_pch.hxx.gch"
	cd /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src && /usr/bin/ccache /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -x c++-header -include /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src/CMakeFiles/fts.dir/cmake_pch.hxx -o CMakeFiles/fts.dir/cmake_pch.hxx.gch -c /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src/CMakeFiles/fts.dir/cmake_pch.hxx.cxx

src/CMakeFiles/fts.dir/cmake_pch.hxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fts.dir/cmake_pch.hxx.i"
	cd /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -x c++-header -include /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src/CMakeFiles/fts.dir/cmake_pch.hxx -E /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src/CMakeFiles/fts.dir/cmake_pch.hxx.cxx > CMakeFiles/fts.dir/cmake_pch.hxx.i

src/CMakeFiles/fts.dir/cmake_pch.hxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fts.dir/cmake_pch.hxx.s"
	cd /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -x c++-header -include /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src/CMakeFiles/fts.dir/cmake_pch.hxx -S /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src/CMakeFiles/fts.dir/cmake_pch.hxx.cxx -o CMakeFiles/fts.dir/cmake_pch.hxx.s

src/CMakeFiles/fts.dir/fts.cpp.o: src/CMakeFiles/fts.dir/flags.make
src/CMakeFiles/fts.dir/fts.cpp.o: ../src/fts.cpp
src/CMakeFiles/fts.dir/fts.cpp.o: src/CMakeFiles/fts.dir/cmake_pch.hxx
src/CMakeFiles/fts.dir/fts.cpp.o: src/CMakeFiles/fts.dir/cmake_pch.hxx.gch
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/fts.dir/fts.cpp.o"
	cd /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src && /usr/bin/ccache /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -include /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src/CMakeFiles/fts.dir/cmake_pch.hxx -o CMakeFiles/fts.dir/fts.cpp.o -c /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/src/fts.cpp

src/CMakeFiles/fts.dir/fts.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fts.dir/fts.cpp.i"
	cd /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -include /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src/CMakeFiles/fts.dir/cmake_pch.hxx -E /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/src/fts.cpp > CMakeFiles/fts.dir/fts.cpp.i

src/CMakeFiles/fts.dir/fts.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fts.dir/fts.cpp.s"
	cd /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -include /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src/CMakeFiles/fts.dir/cmake_pch.hxx -S /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/src/fts.cpp -o CMakeFiles/fts.dir/fts.cpp.s

# Object files for target fts
fts_OBJECTS = \
"CMakeFiles/fts.dir/fts.cpp.o"

# External object files for target fts
fts_EXTERNAL_OBJECTS =

src/fts: src/CMakeFiles/fts.dir/cmake_pch.hxx.gch
src/fts: src/CMakeFiles/fts.dir/fts.cpp.o
src/fts: src/CMakeFiles/fts.dir/build.make
src/fts: src/CMakeFiles/fts.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable fts"
	cd /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/fts.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/fts.dir/build: src/fts

.PHONY : src/CMakeFiles/fts.dir/build

src/CMakeFiles/fts.dir/clean:
	cd /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src && $(CMAKE_COMMAND) -P CMakeFiles/fts.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/fts.dir/clean

src/CMakeFiles/fts.dir/depend:
	cd /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/joseph/git/Ebony-Ayers/fast-thread-syncronization /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/src /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src /home/joseph/git/Ebony-Ayers/fast-thread-syncronization/build/src/CMakeFiles/fts.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/fts.dir/depend

