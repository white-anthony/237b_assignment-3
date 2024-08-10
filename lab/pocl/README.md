# POCL
`pocl` stands for "portable OpenCL" and will be a tool you can use debugging OpenCL.

## Install
To install execute the following

```bash
git submodule update --init --recursive
cd pocl_src
```

In `lib/CL/CMakeLists.txt`, remove lines 322-325.

```bash
pkg install llvmgold
mkdir build
unset LD_LIBRARY_PATH
cmake -DCMAKE_C_FLAGS="-llog" -DCMAKE_INSTALL_PREFIX=~/.pocl ..
make -j2
make install
```

## Debugging
We'll use POCL to do debugging of our previous vector_add code.  Copy your previous code  into the lab/vector_add folder.