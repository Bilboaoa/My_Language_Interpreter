## Installation
To use the interpreter you will first need to install Just following the instructions provided by the README.md which is located [here](https://github.com/casey/just)

### Building: cmake
```bash
just build
```

### Running the interpreter
From within the build catalogue run the program using:
```bash
./bilb path_to_file
```
where path to file is the file that you want to interpret.

### Formating: clang
```bash
just format
```


### Testing: catch2
```bash
just test
```

### Deleting the built program
```bash
just clean
```


### Generating documentation: doxygen
```bash
just doc
```

### Linter: cppcheck
```bash
just lint
```