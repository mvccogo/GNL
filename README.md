# Build instructions

After cloning the main repository, open Git in stumbledinthumbs directory and run:
```
git submodule init
git submodule update
```
This will clone the godot-cpp submodule. Afterwards, clone the header submodule:
```
cd NetworkLib/godot-cpp/
git submodule init
git submodule update
```
This will clone godot-header submodule.

Next, compile the C++ bindings:
```
cd NetworkLib/godot-cpp
scons platform=windows generate_bindings=yes -j4
```

Finally, compile the dynamic library:
```
scons platform=windows
```
