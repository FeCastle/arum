export LD_LIBRARY_PATH=../bprobe/lib:$LD_LIBRARY_PATH
export LD_PRELOAD=$(pwd)/../bprobe/lib/libdyninstAPI_RT.so
export DYNINSTAPI_RT_LIB=$(pwd)/../bprobe/lib/libdyninstAPI_RT.so
export DYNINST_LIBC=$(pwd)/../bprobe/lib/libc-2.13.so
