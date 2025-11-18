# C.UTF8 wctype implementation

## Building

```
mkdir build && cd build
cmake -G Ninja ..
ninja
```

## Running tests

Run all tests:

```
ninja check-all
```

Run individual function tests:

```
ninja check-iswalpha
ninja check-iswdigit
...
```

## Checking the size of the generated static library

```
ninja check-size
```