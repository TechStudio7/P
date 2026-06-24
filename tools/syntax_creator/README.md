Syntax Creator
================

Syntax Creator is an experimental standalone tool (planned) that lets language authors add custom syntaxes to P/P++/P# at runtime.

Status: experimental

Planned features
- Load a JSON/YAML syntax descriptor file and register tokens/transformations
- Provide a small REPL for testing new syntax rules
- Emit a plugin that `ptranspiler` can load at startup to apply syntax transforms

Build

This directory contains a placeholder C implementation and a `CMakeLists.txt` to build the tool when implemented.
