# A `quickstack2` example: running something simple

Most of the work in quickstack2 is done with `ptrace` and the BFD library: both,
particularly BFD, require that the executable is compiled with certain
visibility settings. The easiest way to get this to work is to link everything
in statically. Here is a simple C program that computes `147`:

```c
double s;
void burn(int n)
{
  for (unsigned int i = 0; i < n; ++i)
    for (unsigned int j = 1; j < 100000; ++j)
      s += 1. / j * i;
}
int main()
{
  burn(10000);
  return s;
}
```

We can compile and run this with
```
> gcc -O1 -static test.c
> ./a.out ; echo $?
147
```

if we manage to start quickstack2 while this is running we get
```
> build]$ ./quickstack2 -b -f --debug=0 -p $(pidof a.out )
Thread 1 (LWP 31456):
#01  0x0000000000401b9d in burn () from a.out
#02  0x0000000000401bc2 in main () from a.out
```

We have to be careful to see when functions are inlined or not: if we compile
with higher optimizations, i.e.,

```
> gcc -O3 -static test.c
```

and then run the program and quickstack we get

```
> ./quickstack2 -b -f --debug=0 -p $(pidof a.out )

Thread 1 (LWP 32267):
#01  0x00000000004015bc in main () from a.out
```

so the function is in fact inlined. If we switch the declaration of `burn` to

```c
void __attribute__ ((noinline)) burn(int n)
```

then we get what we expect again:

```
> ./quickstack2 -b -f --debug=0 -p $(pidof a.out )

Thread 1 (LWP 2539):
#01  0x0000000000401bbc in burn () from a.out
#02  0x000000000040159a in main () from a.out
```

As mentioned in the introduction, we should always compile with
`-fno-omit-frame-pointer` too, but that is not an issue here since `burn` only
uses a single 32-bit register.
