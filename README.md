# TypeTreeRipper

An experimental C++ alternative to [TypeTreeDumper](https://github.com/PersonForever/TypeTreeDumper) that does not require symbols.

# Building

Open the directory in Visual Studio and switch the solution explorer to the CMake Targets View.

# Usage (Windows)

The TypeTreeRipper module functions as a proxy for `version.dll` or `winhttp.dll`, but can also be injected at process start.

When loaded, the module waits for the engine to initialize and then initiates a type tree dump. The data will be written to `release.ttbin` (when dumping from the editor, an `editor.ttbin` will also be produced) and then the process will terminate.

At this time, only non-development Unity player builds are supported. Support for development builds and the editor requires additional definitions of `Object` and `TransferBase` in the source.

The included `TypeTreeRipper.Converter` tool can be used to convert `.ttbin` files to `.tpk` or the legacy struct dump format.
