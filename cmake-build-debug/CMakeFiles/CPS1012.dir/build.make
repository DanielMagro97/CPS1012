# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/danmag/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/171.4073.41/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/danmag/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/171.4073.41/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/danmag/CLionProjects/CPS1012

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/danmag/CLionProjects/CPS1012/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/CPS1012.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/CPS1012.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CPS1012.dir/flags.make

CMakeFiles/CPS1012.dir/main.c.o: CMakeFiles/CPS1012.dir/flags.make
CMakeFiles/CPS1012.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/danmag/CLionProjects/CPS1012/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/CPS1012.dir/main.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/CPS1012.dir/main.c.o   -c /home/danmag/CLionProjects/CPS1012/main.c

CMakeFiles/CPS1012.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CPS1012.dir/main.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/danmag/CLionProjects/CPS1012/main.c > CMakeFiles/CPS1012.dir/main.c.i

CMakeFiles/CPS1012.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CPS1012.dir/main.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/danmag/CLionProjects/CPS1012/main.c -o CMakeFiles/CPS1012.dir/main.c.s

CMakeFiles/CPS1012.dir/main.c.o.requires:

.PHONY : CMakeFiles/CPS1012.dir/main.c.o.requires

CMakeFiles/CPS1012.dir/main.c.o.provides: CMakeFiles/CPS1012.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/CPS1012.dir/build.make CMakeFiles/CPS1012.dir/main.c.o.provides.build
.PHONY : CMakeFiles/CPS1012.dir/main.c.o.provides

CMakeFiles/CPS1012.dir/main.c.o.provides.build: CMakeFiles/CPS1012.dir/main.c.o


# Object files for target CPS1012
CPS1012_OBJECTS = \
"CMakeFiles/CPS1012.dir/main.c.o"

# External object files for target CPS1012
CPS1012_EXTERNAL_OBJECTS =

CPS1012: CMakeFiles/CPS1012.dir/main.c.o
CPS1012: CMakeFiles/CPS1012.dir/build.make
CPS1012: CMakeFiles/CPS1012.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/danmag/CLionProjects/CPS1012/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable CPS1012"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CPS1012.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CPS1012.dir/build: CPS1012

.PHONY : CMakeFiles/CPS1012.dir/build

CMakeFiles/CPS1012.dir/requires: CMakeFiles/CPS1012.dir/main.c.o.requires

.PHONY : CMakeFiles/CPS1012.dir/requires

CMakeFiles/CPS1012.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CPS1012.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CPS1012.dir/clean

CMakeFiles/CPS1012.dir/depend:
	cd /home/danmag/CLionProjects/CPS1012/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/danmag/CLionProjects/CPS1012 /home/danmag/CLionProjects/CPS1012 /home/danmag/CLionProjects/CPS1012/cmake-build-debug /home/danmag/CLionProjects/CPS1012/cmake-build-debug /home/danmag/CLionProjects/CPS1012/cmake-build-debug/CMakeFiles/CPS1012.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CPS1012.dir/depend
