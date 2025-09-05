# Cross (X) Platform FrameWork for doing stupid stuff

I finally acknowledged that this is a framework in the name.
To build, you must find a copy of GLEW, which you can find [here](https://github.com/nigels-com/glew/releases), I couldn't add as a submodule as the repository is incomplete.

This is my procedure for creating the project:

```
root > mkdir build  
root > cd build  
root/build > cmake ..
```

In the moment of the `cmake` command, you can add `-G` for your favorite generator, I guess, I usually use Ninja.

For the demo executable, all the resources are in the demo folder.  
If you want, take a look at the source in `src_demo/demo.cpp`.