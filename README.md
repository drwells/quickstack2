# quickstack2

## what is quickstack2?
quickstack2 is a tool to take call stack traces with minimal overheads.

There are a couple of tools for take stack traces such as gdb, pstack, but these
tools have serious overheads. In many cases, target process stops for 0.2-N
seconds. It is dangerous to use such tools in production environment.
quickstack makes it possible to take stack traces in less than one
millisecond. This is much smaller overhead so you can frequently take stack
traces in production environment.

quickstack internally scans stack frames and guesses caller functions. For
64bit applications, it is highly recommended to build with
`-fno-omit-frame-pointer`.

quickstack2, as the name implies, is based on quickstack and contains some
additional command line options, performance improvements (it can start and
finish in about 50 milliseconds, while quickstack is closer to 300
milliseconds), and support for ARM64. Some of the defaults (notably for printing
function line numbers) are different, but the main ptrace-based approach is the
same.

## compiling quickstack2
* Install binutils 2.22+ and elfutils-libelf-devel
* Install cmake
* cmake .
* make
* make install

Install dependencies (Ubuntu):
sudo apt-get install binutils-dev
sudo apt-get install libelf-dev
sudo apt-get install libiberty-dev


## quick example
```
[user$] quickstack2 -f -p $(pidof mysqld)
```

## License
BSD 3-Clause License
