# GLScene

Big wrapper around OpenGL

## Make

```
make                # make libGLScene.a
make libGLScene.a   # create a libGLScene.a file as a static library
make GLScene.dylib  # create a GLScene.dylib file as dynamic library
make example_[name] # build the example in the examples directory
make clean          # cleanup artifact generated by build
make fclean         # cleanup everything
make re             # cleanup everything and rebuild

make release=true   # make in release mode
```
