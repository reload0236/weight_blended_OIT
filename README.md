Weighted Blended Order-Independent Transparent
============================

This is the code implementation based on papper "https://jcgt.org/published/0002/02/09/paper.pdf".
And the source coude is based on the libs of <<the OpenGL Programming Guide (Red Book), 9th Edition>>.

Building
--------

We are using CMake as our build system. To build the samples, enter
the "build" subdirectory and enter "cmake -G "{your generator here}" .."
Project files or makefiles will be produced in the build subdirectory.
We have tested Unix makefiles ("Unix Makefiles") and Visual Studio project files.
Visual Studio 2013 ("Visual Studio 12") was used to develop most of the samples.
An update to add support for 2015 will land shortly. Code should compile, but the
templates for the project files are not in the repository at this time.
Other compilers and IDEs should work, but we haven't tested them.

Running
-------