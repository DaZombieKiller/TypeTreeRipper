# TypeTreeRipper

An experimental C++ alternative to [TypeTreeDumper](https://github.com/PersonForever/TypeTreeDumper) that does not require symbols.

# Building

Open the directory in Visual Studio and switch the solution explorer to the CMake Targets View.

# Usage

This currently functions as a proxy for `version.dll` which will do a primitive type tree dump into `types.txt` and only supports non-development Unity player builds.

Support for the editor and development builds requires defining revisions of relevant data structures in the code.
