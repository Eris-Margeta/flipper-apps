# Flipper Zero Apps

A collection of custom applications for [Flipper Zero](https://flipperzero.one/).

![Flipper Zero](https://img.shields.io/badge/Flipper%20Zero-FF6600?style=for-the-badge&logo=flipper&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![CI](https://github.com/Eris-Margeta/flipper-apps/actions/workflows/ci.yml/badge.svg)

## Apps

| App | Description | Version | Category |
|-----|-------------|---------|----------|
| [Big Clock](apps/big-clock/) | Bedside/tableside clock with adjustable brightness (0-100%) | 1.1 | Tools |

## Quick Start

### Prerequisites

- [Python](https://python.org/) 3.10+
- [Poetry](https://python-poetry.org/) (recommended) or pip
- [Flipper Zero](https://flipperzero.one/) device

### Development Setup

```bash
# Clone the repository
git clone https://github.com/Eris-Margeta/flipper-apps.git
cd flipper-apps

# Install dependencies with Poetry
poetry install

# Navigate to an app and build
cd apps/big-clock
poetry run ufbt

# Build and install to connected Flipper Zero
poetry run ufbt launch
```

<details>
<summary>Alternative: Using pip</summary>

```bash
# Create virtual environment
python -m venv .venv
source .venv/bin/activate  # Windows: .venv\Scripts\activate

# Install ufbt
pip install ufbt

# Build and run
cd apps/big-clock
ufbt launch
```

</details>

## Building Apps

```bash
cd apps/<app-name>
poetry run ufbt           # Build only
poetry run ufbt launch    # Build + install + run on Flipper
```

The compiled `.fap` file will be in `dist/`.

## Creating a New App

1. Copy the template:
   ```bash
   cp -r apps/_template apps/your-app-name
   ```

2. Update files:
   - `application.fam` - Set unique `appid`, `name`, version
   - `app.c` - Implement your app
   - `README.md` - Document your app
   - `VERSION` - Set version number

3. Create icon and screenshots

4. Build and test:
   ```bash
   cd apps/your-app-name
   poetry run ufbt launch
   ```

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

## Repository Structure

```
flipper-apps/
├── .github/workflows/   # CI configuration
├── apps/
│   ├── _template/       # Template for new apps
│   └── big-clock/       # Big Clock application
├── docs/
│   └── PUBLISHING.md    # Catalog publishing guide
├── CONTRIBUTING.md      # Contribution guidelines
├── SECURITY.md          # Security policy
├── LICENSE              # MIT License
└── README.md            # This file
```

## App Structure

Each app contains:

```
apps/<app-name>/
├── application.fam      # App manifest
├── *.c / *.h            # Source files
├── README.md            # Documentation
├── changelog.md         # Version history
├── VERSION              # Version number
├── icon.png             # 10x10px 1-bit icon
└── screenshots/         # qFlipper screenshots
```

## Publishing to Flipper Catalog

See [docs/PUBLISHING.md](docs/PUBLISHING.md) for instructions on publishing apps to the official Flipper App Catalog.

## Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) before submitting a pull request.

## Security

For security concerns, see [SECURITY.md](SECURITY.md).

## License

This project is licensed under the MIT License - see [LICENSE](LICENSE) for details.

## Author

**Eris Margeta** ([@Eris-Margeta](https://github.com/Eris-Margeta))

## Acknowledgments

- [Flipper Devices](https://github.com/flipperdevices) for the hardware and SDK
- [Flipper Zero Firmware](https://github.com/flipperdevices/flipperzero-firmware)
