# App Name

<!-- TODO: Replace with your app name and description -->

Short description of what your app does.

![Flipper Zero](https://img.shields.io/badge/Flipper%20Zero-FF6600?style=flat&logo=flipper&logoColor=white)
![Version](https://img.shields.io/badge/version-1.0-blue)
![CI](https://github.com/<your-username>/flipper-apps/actions/workflows/ci.yml/badge.svg)

## Features

<!-- TODO: List your app's features -->

- Feature 1
- Feature 2
- Feature 3

## Controls

| Button | Action |
|--------|--------|
| UP | Description |
| DOWN | Description |
| LEFT | Description |
| RIGHT | Description |
| OK | Description |
| BACK | Exit application |

## Screenshots

<!-- TODO: Add screenshots taken with qFlipper -->

## Building

### Prerequisites

- [ufbt](https://github.com/flipperdevices/flipperzero-ufbt) (micro Flipper Build Tool)

### Build Commands

```bash
# Using Poetry (recommended)
poetry run ufbt

# Using pip
pip install ufbt
ufbt

# Build and install to connected Flipper Zero
poetry run ufbt launch
```

### Output

The compiled `.fap` file will be in the `dist/` directory.

## Installation

### Via ufbt (recommended)

```bash
poetry run ufbt launch
```

### Manual Installation

1. Build the app with `ufbt`
2. Copy `dist/<app_name>.fap` to your Flipper Zero's SD card at `/ext/apps/<Category>/`

## Version

See [VERSION](VERSION) file.

## License

MIT License - see [LICENSE](../../LICENSE)

## Author

<!-- TODO: Update author -->

**Your Name** ([@YourGitHub](https://github.com/YourGitHub))

---

Part of [flipper-apps](https://github.com/<your-username>/flipper-apps) monorepo.
