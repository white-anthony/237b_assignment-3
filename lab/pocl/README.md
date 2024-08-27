# POCL
`pocl` stands for "portable OpenCL" and will be a tool you can use debugging OpenCL.

## Install
To install execute the following

```bash
cd ~
git https://github.com/pocl/pocl.git
cd pocl
```

In `lib/CL/CMakeLists.txt`, update line 204 to be `if(ANDROID OR LINUX)`.
In `lib/CL/devices/pthread/pthread_scheduler.c`, update line 712 to be `#if defined(__linux__) && !defined(__ANDROID__) && FALSE`.

```bash
pkg install llvmgold
mkdir build
cd build
unset LD_LIBRARY_PATH
export LDFLAGS=" -llog"
export CFLAGS=" -U__ANDROID__"
cmake -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_BUILD_TYPE=Debug ..
make -j3
. ../tools/scripts/devel-envs.sh
```

This can be tested by running `clinfo`

## Debugging
We'll use POCL to do debugging of our previous vector_add code.  Copy your previous code  into the lab/vector_add folder.
