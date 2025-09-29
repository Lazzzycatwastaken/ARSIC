# ARSIC Library

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)

A simple C++ library for converting images to ASCII art.

## What it does

This library converts images (JPG, PNG, BMP, etc.) into text-based ASCII art using different character sets and rendering modes.

## Quick Start

```cpp
#include "ascii_art.h"

using namespace ascii_art;

// Basic usage
Interpreter interpreter;
std::string ascii = interpreter.convert_from_file("image.jpg");
std::cout << ascii;

// With custom settings
Config config;
config.mode = Mode::HIGH_FIDELITY;
config.target_width = 120;
config.contrast = 1.2f;

Interpreter custom_interpreter(config);
std::string detailed_ascii = custom_interpreter.convert_from_file("photo.png");
```

## Rendering Modes

- **CLEAN**: Simple, readable characters (` .-:=+*#%@`)
- **HIGH_FIDELITY**: Maximum detail with extensive character set
- **BLOCK**: Unicode block characters (`░▒▓█`) for solid appearance

## Building

```bash
# Compile your project with the library
g++ -std=c++17 your_code.cpp ascii_art.cpp -o your_program
```

## API Reference

### Classes
- `Interpreter` - Main conversion class
- `Image` - Image data container
- `Config` - Configuration settings

### Enums
- `Mode` - Rendering modes (CLEAN, HIGH_FIDELITY, BLOCK)

See `ascii_art.h` for complete API documentation.

## Requirements

- C++17 or later
- No external dependencies (stb_image.h included)

## Converter CLI

A small command-line tool `Converter` is included for quickly converting images or playing animated GIFs in the terminal.

Build with the provided Makefile:

```bash
make
```

Usage:

```bash
./Converter IMAGE STYLE COLORS [WIDTH] [ANIMATE] [options]
```

Arguments:
- `IMAGE` - path to the image file (png/jpg/gif/...)
- `STYLE` - `clean` | `high_fidelity` | `block`
- `COLORS` - `yes` | `no` (enable 24-bit color output)
- `WIDTH` - optional numeric target width in characters
- `ANIMATE` - `yes` | `no` (optional; only affects GIF files)

Options:
- `--speed=N` or `speed=N` or `--speed N` - playback speed (1.0 = normal, 2.0 = 2x faster)
- `--min-delay-ms=N` - minimum per-frame delay in milliseconds (clamps very small GIF delays)

Examples:

```bash
# Convert a static image with color and width 100
./Converter photo.png high_fidelity yes 100

# Play an animated GIF at double speed
./Converter animation.gif block yes yes --speed=2.0
```

Notes:
- When playing GIFs, the tool uses the GIF frame delays embedded in the file but scales them by `--speed` and enforces a small minimum delay to avoid extremely rapid playback.
- For better playback fidelity on large/colorful frames, consider increasing terminal size or reducing the `WIDTH` to lower rendering load (this might be a bigger problem).