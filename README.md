# GNL: Godot Network Lib

This is a standalone repo for GNL, a C++ library for MMO games built with Godot. Networking is made with Boost.Asio, heavily based on Javidx9's course.
The compiled project is a .dll file that contains C++ bindings for GDNative. The goal is to turn networking as high-level as possible inside GDScripts.

# Usage

# To-do's

# Build instructions

After cloning the main repository, open Git in GNL directory and run:
```
git submodule init
git submodule update
```
This will clone the godot-cpp submodule. Afterwards, clone the header submodule:
```
cd GNL/godot-cpp/
git submodule init
git submodule update
```
This will clone godot-header submodule.

Next, compile the C++ bindings:
```
cd GNL/godot-cpp
scons platform=windows generate_bindings=yes -j4
```

Finally, compile the dynamic library:
```
scons platform=windows
```
