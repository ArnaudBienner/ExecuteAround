/*
Copyright (c) 2016 Arnaud Bienner

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <functional>
#include <iostream>
#include <memory>

#include "ExecuteAround.h"

struct A {
  void f() {
    std::cout << "f" << std::endl;
  }

  int g() {
    std::cout << "g" << std::endl;
    return 21;
  }
};

void call_before(int& nb_call_before_ref) {
  std::cout << "call before: called " << ++nb_call_before_ref << " times" << std::endl;
}

void call_after() {
  std::cout << "call after" << std::endl;
}

int main() {
  // 1) Simple example
  {
    std::cout << "Example 1: " << std::endl;
    int nb_call_before = 0;

    ExecuteAround<A, std::shared_ptr, std::function<void()>, std::function<void()>>
     execute_around(new A, std::bind(call_before, std::ref(nb_call_before)) , call_after);

    execute_around->f();

    execute_around->f();
  }

  // 2) Making all calls to a class threadsafe
  {
    std::cout << std::endl << "Example 2:" << std::endl;

    // Dummy mutex class
    struct Mutex {
      void lock() {
        std::cout << "dummy lock" << std::endl;
      }

      void unlock() {
        std::cout << "dummy unlock" << std::endl;
      }
    };

    Mutex mutex;
    ExecuteAround<A, std::shared_ptr, std::function<void()>, std::function<void()>>
      execute_around(new A, std::bind(&Mutex::lock, &mutex), std::bind(&Mutex::unlock, &mutex));
    execute_around->f();

    std::cout << std::endl << "Example2 with two calls in one expression:" << std::endl;

    // Warning! Mutex should be recursive for the next line to
    // work. Because the temporary object that will call "unlock"
    // upon destruction will still exists when the second call
    // to another execute_around class member will be called
    // (which makes a call to "lock")
    int res = execute_around->g() + execute_around->g();
    std::cout << "result: " << res << std::endl;
  }

}
