# A `quickstack2` example: dynamic linking

Compiling with the correct symbol visibility settings is a bit tricky with
quickstack2: BFD needs to be able to find all the symbols to get their names and
they are not, by default, available. Consider the following C++ program:

```cpp
#include <chrono>
#include <thread>

class Vec
{
public:
  void operator[](const std::size_t i)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    index += i;
  }

protected:
  std::size_t index = 0;
};


int main()
{
  Vec vec;
  for (unsigned int i = 0; i < 100000; ++i)
    {
      vec[i];
    }
}
```

We can compile and run this with
```
> g++ -O0 test.cc
> ./a.out # this may take awhile
```

if we start quickstack2 while this is running we get
```
> ./quickstack2 -b -f --debug=0 -p $(pidof a.out )

Thread 1 (LWP 3622):
```

i.e., nothing: quickstack2 cannot find the symbols in the executable file that
correspond to the current address. The easy fix for this is to use `ld.gold`,
i.e., the `gold` linker:

```
> gcc -fuse-ld=gold -O0 test.cc
```

and then run the program and quickstack we get

```
> ./quickstack2 -b -f --debug=0 -p $(pidof a.out )

Thread 1 (LWP 6749):
#01  0x00005611165667fb in Vec::operator[] () from a.out
#02  0x000056111656675a in main () from a.out
```

which is what we expected.
