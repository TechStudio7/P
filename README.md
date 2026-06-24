[Image](assets/logos/P.png) [Image](assets/logos/Ppp.png) [Image](assets/logos/Psharp.png)

# P / P++ / P# — Minimal Python-with-Braces experiment

Note: this is a fanmade programming language created by the author of this repository. It is not affiliated with or endorsed by the creators of Python, C, C++, or C#.

This repository contains a small C-based transpiler (`ptranspiler`) that converts a C-style, brace-delimited Python variant (called `P`) into standard Python by turning braces and semicolons into indentation and newlines.

Goals
- Provide a tiny, approachable implementation that "makes Python harder" by adding C/C++/C#-style braces and syntax.
- Offer a starting point to extend with `P++` (C++-style operators) and `P#` (C#-style sugar).

Build

On Windows with GCC (MinGW) or on Unix with `gcc`:

```
make
```

Or compile directly:

```
gcc -O2 -o ptranspiler src/main.c
```

Usage

```
./ptranspiler examples/example.p > examples/example.py
python examples/example.py
```

Notes
- This is a forgiving proof-of-concept; it intentionally keeps parsing simple and will not handle every C/Python corner-case.
- Extend `src/main.c` to add more robust parsing, expressions, and P++/P# features.

Logos

Small logos for the project are included in `assets/logos/`:

- `assets/logos/P.png` — main P logo
- `assets/logos/Ppp.png` — P++ logo
- `assets/logos/Psharp.png` — P# logo

You can reference these in documentation or use them when packaging the language assets.

CMake + YAML

This project includes a `CMakeLists.txt` for building the transpiler and a `project.yml` with basic metadata and build instructions.

To build with CMake:

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

The YAML file `project.yml` lists the project name, version and the logo paths used by the project. If you prefer to keep the PNG logos you provided, copy them into `assets/logos/` with names `P.png`, `Ppp.png`, and `Psharp.png`.