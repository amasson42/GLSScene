# CLDevice

A small framework containing wrappers around openCL usages

## Make

```
make                # make libCLDevice.a
make libCLDevice.a  # create a libCLDevice.a file as a static library
make CLDevice.dylib # create a CLDevice.dylib file as dynamic library
make example_[name] # build the example in the examples directory
make clean          # cleanup artifact generated by build
make fclean         # cleanup everything
make re             # cleanup everything and rebuild

make release=true   # make in release mode
```