# App Name v1.0

<!--
  TODO: Update this README for your app
  - Replace "App Name" with your app name
  - Replace "app-name" with your app folder name (used in release tags)
  - Replace "<your-username>" with your GitHub username
  - Update version number in title and badges

  IMPORTANT: For Flipper App Catalog submission, only use # (H1) and ## (H2) headings.
  Level 3+ headings (###, ####) are forbidden.
-->

> Short description of what your app does.

![App Screenshot](screenshots/screenshot1.png)

[![Flipper Zero](https://img.shields.io/badge/Flipper%20Zero-FF6600?style=flat&logo=flipper&logoColor=white)](https://flipperzero.one/)
[![Version](https://img.shields.io/badge/version-1.0-blue)](../../releases)
[![Release](https://img.shields.io/github/v/release/<your-username>/flipper-apps?filter=app-name-v*&label=release)](https://github.com/<your-username>/flipper-apps/releases?q=app-name)
[![CI](https://github.com/<your-username>/flipper-apps/actions/workflows/ci.yml/badge.svg)](https://github.com/<your-username>/flipper-apps/actions/workflows/ci.yml)

## Download

**[⬇️ Download Latest Release](https://github.com/<your-username>/flipper-apps/releases?q=app-name)** - Get the `.fap` file and copy to `/ext/apps/Tools/` on your Flipper.

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

![Screenshot 1](screenshots/screenshot1.png)

## Building

**Prerequisites:** [ufbt](https://github.com/flipperdevices/flipperzero-ufbt) (micro Flipper Build Tool), [Poetry](https://python-poetry.org/) (recommended)

```bash
# Using Poetry (recommended)
cd apps/app-name
poetry run python -m ufbt

# Build and install to connected Flipper Zero
poetry run python -m ufbt launch
```

The compiled `.fap` file will be in the `dist/` directory.

## Installation

**From Release (easiest):**
1. Go to [Releases](https://github.com/<your-username>/flipper-apps/releases?q=app-name)
2. Download the latest `app-name-vX.X.fap`
3. Copy to `/ext/apps/Tools/` on your Flipper Zero

**Via ufbt:**
```bash
poetry run python -m ufbt launch
```

## Tested Firmware

| Firmware | Version | Status |
|----------|---------|--------|
| Official Flipper | x.xx | ✅ Works |
| Momentum | mntm-xxx | ❓ Not tested |
| Unleashed | - | ❓ Not tested |

## Version History

See [changelog.md](changelog.md) for version history.

## License

MIT License - see [LICENSE](../../LICENSE)

## Author

<!-- TODO: Update author -->

**Your Name** ([@YourGitHub](https://github.com/YourGitHub))

---

Part of [flipper-apps](https://github.com/<your-username>/flipper-apps) monorepo.
