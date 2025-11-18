# C.UTF8 wctype implementation

## Building

```
mkdir build && cd build
cmake -G Ninja ..
ninja
```

## Running tests

```
ninja check-wctype
```

## Checking the size of the generated static library

```
ninja check-size
```