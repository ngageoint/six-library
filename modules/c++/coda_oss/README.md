This is our implementation of various C++11/14/17/20 routines that work with the ancient compilers
we sometimes still need to use.  It's called `coda_oss` instead of `std_` only because some people
won't like the name `coda_oss` (of course, it "can't" be `std`); `coda_oss` is far less likely
to collide with other code (someone else could *also* be trying to do the same thing we are).

By making this a very low-level module with very few other dependencies, it is easier to use
in other code and avoid circular dependencies. For example, the `sys` and `mem` modules can't
be used in `str`.

