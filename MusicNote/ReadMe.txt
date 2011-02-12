
      CFugue MusicNote Library

-------------------------------------------------

This is the source code of CFugue MusicNote library. 

Download the latest version (latest dated) source code and build it to use CFugue in your own applications. 

Visual studio solutions files are already provided in the build directory for building on Windows based- machines. 

For other platforms, use the CMake build system. (Note: CFugue uses C++0x - so please ensure that you are using latest GNU C++ compiler (ver 4.5 and above) to succeed the build.)

For sample usage, check the StaticLib test application code supplied. 

Non-windows platforms can use CFugue only as a satic library linked directly into their code, while Windows users has the option to use CFugue in shared DLL form also, which is mandatory for using CFugue from .Net applications (through P/Invoke or COM).


Directory Structure in the downloaded source:

\include: contains the #include files 

\lib: the generated library files for static linking and dynamic linking after the build

\bin: the dynamic linked dlls and sample test application executables after the build

\src: contains the source code

\test: contains the test application source code

\build: contains the VC solution/project files to build the source code. Also contains the Doxyfile to generate the documentation

\docs: the Doxygen generated documentation if you build the documentation with Doxygen 

\guide: proprietry content - removed in the download 


Copyright 

------------------------------------------------- 

This is a product of CineFx Research Labs, made available free of charge for personal and research use. For commercial usage, please contact the author. 

CFugue is distributed with the hope that it will be useful. No warranty of what-so-ever is implied, including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

Author: Gopalakrishna.Palem@Yahoo.com 

Project: http://musicnote.sourceforge.net 

Copyright (C) 2009 Gopalakrishna Palem 

-------------------------------------------------
