# grepr
A small grep-like CLI tool written in C for pattern searching.

# Installation

### Requirements

- CMake 3.16+
- C11-compatible compiler (GCC, Clang, etc.)

### Build

```bash
mkdir build
cd build
cmake ..
cmake build .
```

The **grepr** executable will be created in the build directory.

# Usage

```bash
grepr [OPTIONS] <pattern> <filename>...
```
### Options
- `-n` - Show line numbers 
- `-h` - Hide file headers when matching multiple files 
- `-v` - Inverted mode (prints lines without matches) 
- `--help` - Display help message

### Supported regex operators

- `.` matches any character
- `^` matches beginning of string
- `$` matches end of string
- `*` match zero or more (greedy)
- `+` match one or more (greedy)
- `?` match zero or one (non-greedy)
- `[abc]` Character class, match if one of {'a', 'b', 'c'}
- `[^abc]` Inverted class, match if NOT one of {'a', 'b', 'c'}
- `[a-zA-Z]` Character ranges, the character set of the ranges { a-z | A-Z }
- `\s` Whitespace, \t \f \r \n \v and spaces
- `\S` Non-whitespace
- `\w` Alphanumeric, [a-zA-Z0-9_]
- `\W` Non-alphanumeric
- `\d` Digits, [0-9]
- `\D` Non-digits

# License
This project is licensed under the MIT License. \
The included getopt.c and getopt.h are licensed under the GNU Lesser General Public License v2.1 or later.