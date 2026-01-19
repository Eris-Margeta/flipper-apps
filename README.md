# Flipper Zero Apps

A collection of custom applications for [Flipper Zero](https://flipperzero.one/).

![Flipper Zero](https://img.shields.io/badge/Flipper%20Zero-FF6600?style=for-the-badge&logo=flipper&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

## Apps

| App | Description | Category |
|-----|-------------|----------|
| [Big Clock](apps/big-clock/) | Full-screen digital clock with adjustable brightness | Tools |

## Quick Start

### Prerequisites

- [Python](https://python.org/) 3.10+
- [Poetry](https://python-poetry.org/) (recommended) or pip
- [Flipper Zero](https://flipperzero.one/) device

### Development Setup (with Poetry)

```bash
# Clone the repository
git clone https://github.com/Eris-Margeta/flipper-apps.git
cd flipper-apps

# Install dependencies with Poetry
poetry install

# Activate the virtual environment
poetry shell

# Navigate to an app and build
cd apps/big-clock
ufbt

# Build and install to connected Flipper Zero
ufbt launch
```

### Development Setup (with pip)

```bash
# Clone the repository
git clone https://github.com/Eris-Margeta/flipper-apps.git
cd flipper-apps

# Create virtual environment (optional but recommended)
python -m venv .venv
source .venv/bin/activate  # On Windows: .venv\Scripts\activate

# Install ufbt
pip install ufbt

# Navigate to an app and build
cd apps/big-clock
ufbt launch
```

## Building Apps

```bash
# Build a specific app
cd apps/big-clock
ufbt                    # Build only
ufbt launch             # Build + install + run on Flipper

# The compiled .fap file will be in dist/
ls dist/*.fap
```

## Manual Installation

1. Build the app with `ufbt`
2. Copy `dist/<app_name>.fap` to your Flipper's SD card at `/ext/apps/<category>/`
3. Find the app in your Flipper's menu under the appropriate category

## Repository Structure

```
flipper-apps/
├── README.md           # This file
├── LICENSE             # MIT License
├── .gitignore          # Git ignore rules
└── apps/
    └── big-clock/      # Big Clock application
        ├── README.md
        ├── application.fam
        └── big_clock.c
```

## Contributing

Contributions are welcome! Feel free to:

1. Fork the repository
2. Create a feature branch
3. Add your app to the `apps/` directory
4. Submit a pull request

### App Structure

Each app should have its own directory under `apps/` with:
- `application.fam` - Flipper app manifest
- `*.c` / `*.h` - Source files
- `README.md` - App documentation

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

**Eris Margeta** ([@Eris-Margeta](https://github.com/Eris-Margeta))

## Acknowledgments

- [Flipper Devices](https://github.com/flipperdevices) for the amazing hardware and SDK
- [Flipper Zero Firmware](https://github.com/flipperdevices/flipperzero-firmware)
