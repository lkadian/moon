# toy

A compiler written from scratch for a toy language :) Features include classes, inheritance, functions, recursion, integer types, arrays, console i/o

## Example

```
fib(integer n) : integer
  do
    if (n <= 1)
    then
      return (n);
    else
      return (fib(n - 1) + fib(n - 2));
    ;
  end

main
  local
    integer x;
  do
    read(x);
    write(fib(x));
  end
```

For more examples see `test/fixtures`

## Usage

```
toy [options] file...

-e, --exe   Execute the generated code after compilation.
-h, --help  Display this information.
```

## Building

In the project root:

```
mkdir build
cd build
cmake ..
```

By now Makefiles should be created. Then, to build executables and do all the linking:

```
make
```

## Running the executables

To run the compiler:

```
./toy [options] file...
```

To run the tests:

```
./run_tests
```

To run moon code:

```
./moon file...
```

## Implementation details

| Phase             | Description                                                                                                                                                                          |
| ----------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| Lexing            | Converts the source file's character stream into a sequence of tokens, using the "hand-written" approach.                                                                            |
| Parsing           | Implementation of an LL(1) parser generator which loads the grammar found in `etc` and creates a parsing table. The token stream is then parsed using this table, producing the AST. |
| Semantic analysis | Several checks for semantic errors/warnings like undefined variables, multiple declarations, circular dependencies, etc. as well as type checking.                                   |
| Code generation   | Generation of "moon" assembly code, which is to be executed by the Moon processor (virtual machine).                                                                                 |
