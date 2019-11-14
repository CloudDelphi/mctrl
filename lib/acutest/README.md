[![Build status (travis-ci.com)](https://img.shields.io/travis/mity/acutest/master.svg?label=linux%20build)](https://travis-ci.org/mity/acutest)
[![Build status (appveyor.com)](https://img.shields.io/appveyor/ci/mity/acutest/master.svg?label=windows%20build)](https://ci.appveyor.com/project/mity/acutest/branch/master)


# Acutest Readme

Home: http://github.com/mity/acutest


## What Is Acutest

Acutest is C/C++ unit testing facility aiming to be as simple as possible, to
not to stand in the developer's way and to minimize any external dependencies.

To achieve that, the complete implementation resides in a single C header file,
and its core depends only on few standard C library functions.


## Overview

**Main features:**
* Unit tests in C or C++ are supported.
* No need to install/setup/configure any testing framework. Acutest is just
  a single header file, `acutest.h`.
* The header provides the program entry point (function `main()`).
* Minimal dependencies: Core features only depend on few standard C headers,
  optional features may use more if available on the particular system.
* Trivial interface for writing unit tests: Few preprocessor macros described
  further below.
* Rudimentary support for [Test Anything Protocol](https://testanything.org/)
  (use `--tap` option).
* Rudimentary support for xUnit-compatible XML output (use `--xml-output=FILE`).

**C++ specific features:**
* Acutest catches any C++ exception thrown from any unit test function. When
  that happens, the given test is considered to fail.
* If the exception is derived from `std::exception`, `what()` is written out
  in the error message.

**Unix specific features:**
* By default, every unit test is executed as a child process.
* By default, if the output is directed to a terminal, the output is colorized.

**Windows specific features:**
* By default, every unit test is executed as a child process.
* By default, if the output is directed to a terminal, the output is colorized.
* Acutest installs a SEH filter to print out uncaught SEH exceptions.

Any C/C++ module implementing one or more unit tests and including `acutest.h`,
can be built as a standalone program. We call the resulted binary as a "test
suite" for purposes of this document. The suite is then executed to run the
tests, as specified with its command line options.

By default, all unit tests in the program are run and (on Windows and Unix)
every unit test is executed in a context of its own subprocess. Both can be
overridden on the command line.

We say any unit test succeeds if all conditions (preprocessor macros `TEST_CHECK`
or `TEST_CHECK_`) called throughout its execution pass, the test does not throw
any exception (C++ only), and (on Windows/Unix) the unit test subprocess is not
interrupted/terminated (e.g. by a signal on Unix or SEH on Windows).

Exit code of the test suite is 0 if all executed unit tests pass, 1 if any of
them fails, or any other number if an internal error occurs.


## Writing Unit Tests

### Basic Use

To use Acutest, simply include the header file `acutest.h` on the beginning of
the C/C++ source file implementing one or more unit tests. Note the header
provides implementation of the `main()` function.

```C
#include "acutest.h"
```

Every test is supposed to be implemented as a function with the following
prototype:

```C
void test_example(void);
```

The tests can use some preprocessor macros to validate the test conditions.
They can be used multiple times, and if any of those conditions fails, the
particular test is considered to fail.

`TEST_CHECK` is the most commonly used testing macro which simply tests a
boolean condition and fails if the condition evaluates to false (or zero).

For example:

```C
void test_example(void)
{
    void* mem;
    int a, b;

    mem = malloc(10);
    TEST_CHECK(mem != NULL);

    mem = realloc(mem, 20);
    TEST_CHECK(mem != NULL);
}
```

Note that the tests should be completely independent on each other. Whenever
the test suite is invoked, the user may run any number of tests in the suite,
in any order. Furthermore by default, on platforms where supported, each unit
test is executed as a standalone (sub)process.

Finally, the test suite source file has to list the unit tests, using the
macro `TEST_LIST`. The list specifies name of each test (it has to be unique)
and pointer to a function implementing the test. I recommend names which are
easy to use on command line: especially avoid space and other special
characters which might require escaping in shell; also avoid dash (`-`) as a
first character of the name, as it could then be interpreted as a command line
option and not as a test name.

```C
TEST_LIST = {
   { "example", test_example },
   ...
   { NULL, NULL }
};
```

Note the test list has to be ended with zeroed record.

For a basic test suites this is more or less all you need to know. However
Acutest provides some more macros which can be useful in some specific
situations. We cover them in the following sub-sections.

### Testing C++ Exceptions

For C++, there is an additional macro `TEST_EXCEPTION` for verifying the given
code (typically just a function or a method call) throw the expected type of
exception. The check fails if the function does not throw any exception or
if it throws anything incompatible.

For example:

```C++
void test_example(void)
{
    TEST_EXCEPTION(CallSomeFunction(), std::exception);
}
```

### Richer Failure Diagnosis

If a condition check fails, it is often useful to provide some additional
information about the situation so the problem is easier to debug. Acutest
provides the macros `TEST_MSG` and `TEST_DUMP` for this purpose.

The former one outputs any `printf`-like message, the other one outputs a
hexadecimal dump of a provided memory block.

Note that both macros only output anything in the most recently checking
macro failed.

So for example:

```C
void test_example(void)
{
    char produced[100];
    char expected[] = "Hello World!";

    SomeSprintfLikeFunction(expected, "Hello %s!", world);
    TEST_CHECK(strlen(produced) == strlen(expected));
    TEST_MSG("Expected: %s", expected);
    TEST_MSG("Produced: %s", produced);

    /* Or if the function would provide some binary stuff, we might rather
     * use TEST_DUMP instead and output a hexadecimal dump. */
    TEST_DUMP("Expected:", expected, strlen(expected));
    TEST_DUMP("Produced:", produced, strlen(produced));
}
```

(Note that `TEST_MSG` requires the compiler with variadic macros support.)

### Loops over Test Vectors

Sometimes, it is useful to construct the testing function as a loop over some
data providing a collection of test vectors and their respective expected
outputs. For example imagine our unit test is supposed to verify some kind
of a hashing function and we've got a collection of test vectors for it in
its specification.

In such cases, it is very useful to get some name in the output log so that
if any check fails, it is easy to identify the guilty test vector. However,
it may be impractical to add such name to every checking macro.

To solve this, Acutest provides the macro `TEST_CASE`. The macro specifies
a string serving as the test vector name. When used, Acutest makes sure that
the provided name precedes any message from subsequent condition checks in its
output log (until `TEST_CASE` is used again or the whole unit test ends).

For example lets assume we are testing `SomeFunction()` which is supposed,
for a given byte array of some size, return another some another array of bytes
in a newly `malloc`-ed buffer. Then we can do something like this:

```C
struct TestVector {
    const char* name;
    const uint8_t* input;
    size_t input_size;
    const uint8_t* expected_output;
    size_t expected_output_size;
};

struct TestVector test_vectors[] = {
    /* some data */
};

void test_example(void)
{
    int i;
    const uint8_t* output;
    size_t output_size;

    for(i = 0; i < sizeof(test_vectors) / sizeof(test_vectors[0]); i++) {
        struct TestVector* vec = &test_vectors[i];

        /* Output the name of the tested test vector. */
        TEST_CASE(vec.name);

        /* Now, we can check the function produces what it should for the
         * current test vector. If any of the following checking macros
         * produces any output (either because the check fails, or because
         * high `--verbose` level is used), Acutest also outputs the  currently
         * tested vector's name. */
        output = SomeFunction(vec->input, vec->input_size, &output_size);
        if(TEST_CHECK(output != NULL)) {
            TEST_CHECK(output_size == vec->expected_output_size);
            TEST_CHECK(memcmp(output, vec->expected_output, output_size) == 0);
            free(output);
        }
    }
}
```

To explicitly reset the test vector name, typically after such a loop if the
unit test makes some subsequent checks not related to any particular test
vector anymore, use `NULL` as the macro's parameter:

```C
void test_example(void)
{
    ...

    /* Reset the test vector name. */
    TEST_CASE(NULL);

    ...
}
```


### Custom Log Messages

Many of the macros mentioned in the earlier sections have a variant which
allows to output a custom messages instead of some default ones.

All of these have the same name as the aforementioned macros with the
additional underscore suffix and they also expect `printf`-like string format
(and corresponding additional arguments).

So for example instead of
```C++
TEST_CHECK(a == b);
TEST_EXCEPTION(SomeFunction(), std::exception);
```
we can use
```C++
TEST_CHECK_(a == b, "%d is equal to %d", a, b);
TEST_EXCEPTION_(SomeFunction(), std::exception, "SomeFunction() throws std::exception");
```

Similarly instead instead of
```C
TEST_CASE("name");
```
we can use richer
```C
TEST_CASE_("iteration #%d", 42);
```

However note all of these can only be used if your compiler supports variadic
macros.


## Building the Test Suite

When we are done with implementing the tests, we can simply compile it as
a simple C/C++ program. For example, assuming `cc` is your C compiler:

```sh
$ cc test_example.c -o test_example
```


## Running Unit Tests

When the test suite is compiled, the resulted testing binary can be used to run
the tests.

By default (without any command line options), it runs all implemented unit
tests. It can also run only subset of the unit tests as specified on the
command line:

```sh
$ ./test_example                # Runs all tests in the suite
$ ./test_example test1 test2    # Runs only tests specified
$ ./test_example --skip test3   # Runs all tests but those specified
```

Note that a single command line argument can select whole group of test units
because Acutest implements several levels of unit test selection (the 1st one
matching at least one test unit is used):

1. *Exact match*: When the argument matches exactly the whole name of some unit
   test then just the given test is selected.

2. *Word match*: When the argument does not match any complete test name, but
   it does match whole word in one or more test names, then all such tests are
   selected. (Note that space ` `, tabulator `\t`, dash `-` and underscore `_`
   are seen as word delimiters in test names.)

3. *Substring match*: If even the word match failed to select any test, then
   all tests with a name which contains the argument as its substring are
   selected.

By adopting an appropriate test naming strategy, this allows user to run (or
to skip if `--skip` is used) easily whole family of related tests with a single
command line argument.

For example consider test suite `test_example` which implements tests `foo-1`,
`foo-2`, `foomatic`, `bar-1` and `bar-10`:

```sh
$ ./test_example bar-1   # Runs only the test 'bar-1' (exact match)
$ ./test_example foo     # Runs 'foo-1' and 'foo-2' (word match)
$ ./test_example oo      # Runs 'foo-1', 'foo-2' and 'foomatic' (substring match)
$ ./test_example 1       # Runs 'foo-1' and 'bar-1' (word match)
```

You may use `--list` or `-l` to just list all unit tests implemented by the
given test suite:

```sh
$ ./test_example --list
```

To see description for all the supported command line options, run the binary
with the option `--help`:

```sh
$ ./test_example --help
```


## FAQ

**Q: Wasn't this project known as "CUTest"?**

**A:** Yes. It has been renamed as the original name was found to be
[too much overloaded](https://github.com/mity/cutest/issues/6).


**Q: Do I need to distribute file `README.md` and/or `LICENSE.md`?**

**A:** No. The header `acutest.h` includes URL to our repo, copyright note and
the MIT license terms inside of it. As long as you leave those intact, we are
completely fine if you only add the header into your project. After all,
the simple use and all-in-one-header nature of it is our primary aim.


## License

Acutest is covered with MIT license, see the file `LICENSE.md` or beginning of
`acutest.h` for its full text.


## More Information

The project resides on github:

* http://github.com/mity/acutest

You can find the latest version of Acutest there, contribute with enhancements
or report bugs.
