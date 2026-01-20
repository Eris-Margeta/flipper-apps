# Flipper Zero Apps

A collection of custom applications for [Flipper Zero](https://flipperzero.one/).

![Flipper Zero](https://img.shields.io/badge/Flipper%20Zero-FF6600?style=for-the-badge&logo=flipper&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![CI](https://github.com/Eris-Margeta/flipper-apps/actions/workflows/ci.yml/badge.svg)

## Apps

| App | Description | Version | Category |
|-----|-------------|---------|----------|
| [Big Clock](apps/big-clock/) | Bedside/tableside clock with adjustable brightness (0-100%) | 1.2 | Tools |
| [Reality Clock](apps/reality-clock/) | Real dimensional stability monitor using CC1101 multi-band RSSI analysis | 4.0 | Tools |

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

## Just Commands

This project includes [just](https://github.com/casey/just) command recipes for convenient building and installation.

### Install just

```bash
# macOS
brew install just

# Linux
cargo install just
# or: apt install just

# Windows
choco install just
```

### Available Commands

| Command | Description |
|---------|-------------|
| `just install-all` | Install all apps to Flipper (excludes `_template`) |
| `just install <app>` | Install a specific app by folder name |
| `just build-all` | Build all apps without installing |
| `just build <app>` | Build a specific app |
| `just clean-all` | Clean all build artifacts |
| `just clean <app>` | Clean a specific app's build artifacts |
| `just list` | List available apps |

(there's a bug on just install all; it requires adding a wait time before a person manually exits each app on the flipper device before another installation starts. TODO: add later)

### Usage Examples

```bash
# Install all apps to your Flipper
just install-all

# Install only the big-clock app
just install big-clock

# Build without installing
just build reality-clock

# See available apps
just list
```

### Alternative: Python Version

If you're using a virtualenv or global pip install instead of Poetry, use `justfile.python`:

```bash
# Activate your virtualenv first, then:
just -f justfile.python install-all
just -f justfile.python install big-clock
```

| Justfile | Uses | When to Use |
|----------|------|-------------|
| `justfile` (default) | `poetry run python -m ufbt` | Poetry users (recommended) |
| `justfile.python` | `python3 -m ufbt` | Virtualenv or global pip users |

## Development Tips

See [docs/DEVELOPMENT_TIPS.md](docs/DEVELOPMENT_TIPS.md) for lessons learned, including:

- **Flicker-free brightness control** - The NightStand Clock approach
- **Always-on backlight** - Proper use of notification sequences
- **Rolling buffers** - Stable sensor readings
- **Hardware access** - SubGHz radio, temperature sensor
- **Code organization** - Recommended patterns

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
│   ├── big-clock/       # Big Clock application
│   └── reality-clock/   # Reality Dimension Clock (experimental)
├── docs/
│   ├── PUBLISHING.md       # Catalog publishing guide
│   └── DEVELOPMENT_TIPS.md # Tips & tricks for app development
├── justfile             # Just commands (Poetry)
├── justfile.python      # Just commands (Python/pip)
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

## Publishing to Flipper App Catalog

Follow this workflow to develop and submit apps to the [official Flipper App Catalog](https://github.com/flipperdevices/flipper-application-catalog).

### Development & Submission Workflow

```
┌─────────────────────────────────────────────────────────────────────────┐
│  1. DEVELOP        Create feature branch, implement your app            │
│         ↓                                                               │
│  2. PR TO MAIN     Open PR, CI runs automatically to validate build     │
│         ↓                                                               │
│  3. MERGE          After review, merge to main branch                   │
│         ↓                                                               │
│  4. TAG            Create version tag: git tag appname-v1.0             │
│         ↓                                                               │
│  5. PUSH TAG       Push tag: git push origin appname-v1.0               │
│         ↓                                                               │
│  6. RELEASE        GitHub Action builds app, creates release with .fap  │
│         ↓                                                               │
│  7. SUBMIT         PR to flipper-application-catalog with commit hash   │
└─────────────────────────────────────────────────────────────────────────┘
```

### Step-by-Step Guide

**1. Develop on a feature branch**
```bash
git checkout -b feat/my-new-app
# ... develop your app ...
```

**2. Open PR to main**
- CI workflow runs automatically on PRs
- Validates build, checks file structure, verifies version consistency
- Green CI = safe to merge

**3. Merge to main**
- After code review and CI passes, merge your PR
- Your code is now on the stable `main` branch

**4-5. Create and push a version tag**
```bash
git checkout main
git pull origin main
git tag reality-clock-v4.0    # App-specific tag
git push origin reality-clock-v4.0
```

**6. GitHub Release is created automatically**
- Release workflow triggers on tag push
- Builds the app and uploads `.fap` file as release asset
- Release notes generated automatically

**7. Submit to Flipper App Catalog**
```bash
# Get the commit hash from main
git rev-parse HEAD
```

Then submit a PR to [flipper-application-catalog](https://github.com/flipperdevices/flipper-application-catalog) with your `manifest.yml` pointing to that commit hash.

### Why Submit from Main?

The Flipper App Catalog clones your repo at the specific commit hash in your manifest. That commit **must be on a public, stable branch** (main). If you reference a commit from a feature branch that gets deleted or rebased, the catalog build will break.

### Versioning Strategy

This monorepo supports two release types that can coexist:

| Release Type | Purpose | Example Tags |
|--------------|---------|--------------|
| **App-specific** | Individual app updates | `reality-clock-v4.0`, `big-clock-v1.2-beta` |
| **Monorepo bundle** | Coordinated stable release | `v2024.01`, `v1.0.0` |

**Recommended workflow:**
```
reality-clock-v4.0        # Stable release of reality-clock
reality-clock-v4.1-beta   # Beta testing new features
reality-clock-v4.1        # Promote beta to stable
v2024.02                  # Stable bundle (all apps known-good)
```

### Tag Naming Convention

**App-specific releases:**
```
<app-folder-name>-v<version>[-prerelease]

Examples:
  reality-clock-v4.0          # Stable release
  reality-clock-v4.1-beta     # Beta release
  reality-clock-v4.1-rc1      # Release candidate
  big-clock-v1.2-alpha        # Alpha release
  big-clock-v1.2-nightly      # Nightly build
```

**Monorepo releases:**
```
v<version>[-prerelease]

Examples:
  v2024.01                    # January 2024 stable bundle
  v1.0.0                      # Semantic version stable
  v2024.02-beta               # Beta bundle
```

### Version Validation

The release workflow **validates that your tag matches the VERSION file**:

```
Tag: reality-clock-v4.0
VERSION file: 4.0
✅ Match - release proceeds

Tag: reality-clock-v4.1
VERSION file: 4.0
❌ Mismatch - release fails
```

**Before tagging, always update:**
1. `apps/<app>/VERSION` - The version number
2. `apps/<app>/application.fam` - The `fap_version` field
3. `apps/<app>/changelog.md` - Document changes

### Pre-release Tags

Adding these suffixes marks the release as a pre-release on GitHub:

| Suffix | Meaning |
|--------|---------|
| `-alpha` | Early development, unstable |
| `-beta` | Feature complete, testing |
| `-rc1`, `-rc2` | Release candidate |
| `-dev` | Development snapshot |
| `-nightly` | Automated nightly build |

Pre-releases are clearly marked with ⚠️ in the release notes.

### Quick Release Commands

```bash
# 1. Update version files
echo "4.1" > apps/reality-clock/VERSION
# Also update application.fam fap_version="4.1"

# 2. Commit changes
git add .
git commit -m "Bump reality-clock to v4.1"

# 3. Create and push tag
git tag reality-clock-v4.1
git push origin main
git push origin reality-clock-v4.1

# 4. GitHub Actions creates the release automatically
```

See [docs/PUBLISHING.md](docs/PUBLISHING.md) for detailed manifest.yml setup and catalog requirements.

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
- [@nymda](https://github.com/nymda) for the [NightStand Clock](https://github.com/nymda/FlipperNightStand) brightness approach
