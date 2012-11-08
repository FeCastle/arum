export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH
export LD_PRELOAD=/home/derek/test/lib/libdyninstAPI_RT.so
export DYNINSTAPI_RT_LIB=/home/derek/test/lib/libdyninstAPI_RT.so
export DYNINST_LIBC=/home/derek/test/lib/libc-2.13.so
make
./example
unset DYNINST_LIBC
unset DYNINSTAPI_RT_LIB
unset LD_LIBRARY_PATH
