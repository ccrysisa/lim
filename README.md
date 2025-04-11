# LIM

Lim is a Lisp Virtual Machine which support assemble and interpret lisp program.

## Quick Start

```sh
$ make examples
$ ./lime tests/fib.lim
```

## Usage

```sh
# Build the project
$ make

# Build examples
$ make examples

# clean
$ make clean

# Assemble source code to program for virtual machine
$ ./build/lasm -i <input.lasm> -o <output.lim>

# Emulate program by virtual machine
$ ./build/lime -i <input.lim>

# Disassemble program
$ ./build/delasm -i <input.lim>

# Generate compile_commands.json (make sure you have intsalled bear)
$ make clean
$ bear -- make
```

## Components

### lasm

Assembly language for the Virtual Machine. For exampes see [./tests](./tests/) folder.

### lime

LIM emulator. Used to run programs generated by [lasm](#lasm).

### delasm

Disassembler for the binary files generates by [lasm](#lasm).
