# Flipper Zero Apps

A collection of custom applications for [Flipper Zero](https://flipperzero.one/).

![Flipper Zero](https://img.shields.io/badge/Flipper%20Zero-FF6600?style=for-the-badge&logo=flipper&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

## Apps

| App | Description | Category |
|-----|-------------|----------|
| [Big Clock](apps/big-clock/) | Full-screen digital clock with adjustable brightness | Tools |

## Installation

### Prerequisites

- [Flipper Zero](https://flipperzero.one/) device
- [ufbt](https://github.com/flipperdevices/flipperzero-ufbt) (micro Flipper Build Tool)

```bash
pip install ufbt
```

### Building All Apps

```bash
# Build a specific app
cd apps/big-clock
ufbt

# Build and install to connected Flipper
ufbt launch
```

### Manual Installation

1. Build the app you want
2. Copy the `.fap` file from `dist/` to your Flipper's SD card at `/ext/apps/<category>/`

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
