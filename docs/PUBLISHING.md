# Publishing Apps to Flipper App Catalog

This guide explains how to publish apps from this monorepo to the official Flipper Zero App Catalog.

## How the Catalog Works

The Flipper App Catalog does **not** host your source code. It only contains `manifest.yml` files that **point to** your GitHub repository:

```
YOUR MONOREPO (flipper-apps)          CATALOG (flipper-application-catalog)
├── apps/                             ├── applications/
│   └── big-clock/                    │   └── Tools/
│       ├── big_clock.c               │       └── big_clock/
│       ├── application.fam           │           └── manifest.yml  ← points to your repo
│       ├── icon.png                  │
│       ├── changelog.md              │
│       └── screenshots/              │
└── ...                               └── ...
```

When someone installs your app, Flipper's infrastructure pulls from YOUR repo using the commit SHA specified in the manifest.

## Prerequisites for Each App

Before publishing, ensure your app directory contains:

| File | Required | Description |
|------|----------|-------------|
| `application.fam` | Yes | App manifest with `appid`, `name`, `fap_version`, `fap_category`, `fap_icon` |
| `README.md` | Yes | Usage instructions, hardware requirements |
| `changelog.md` | Yes | Version history (see format below) |
| `icon.png` | Yes | 10x10px 1-bit PNG app icon |
| `screenshots/` | Yes | At least one qFlipper screenshot (unmodified) |

### changelog.md Format

```markdown
## v1.1
- Added feature X
- Fixed bug Y

## v1.0
- Initial release
```

### Creating the Icon

Use Poetry environment to create 10x10px 1-bit icons:

```bash
poetry shell
python3 << 'EOF'
from PIL import Image

# Create 10x10 1-bit image
img = Image.new('1', (10, 10), 0)

# Design your icon pixel by pixel
pixels = [
    [0, 0, 0, 1, 1, 1, 1, 0, 0, 0],
    # ... 10 rows of 10 pixels (1=white, 0=black)
]

for y, row in enumerate(pixels):
    for x, pixel in enumerate(row):
        img.putpixel((x, y), pixel)

img.save('apps/your-app/icon.png')
EOF
```

### Taking Screenshots

1. Connect Flipper Zero to computer
2. Open **qFlipper** app
3. Run your app on the Flipper
4. Click the screen preview in qFlipper
5. Save screenshot (do NOT resize or modify)
6. Place in `apps/your-app/screenshots/`

## Publishing Process

### Step 1: Prepare Your App

Ensure all required files are in place and committed:

```bash
cd apps/your-app
ls -la
# Should show: application.fam, README.md, changelog.md, icon.png, screenshots/
```

### Step 2: Fork the Catalog (One-Time)

```bash
gh repo fork flipperdevices/flipper-application-catalog --clone=true
```

This creates:
- GitHub fork at `your-username/flipper-application-catalog`
- Local clone (outside this monorepo)

### Step 3: Get Your Commit SHA

After committing all app changes to this repo:

```bash
git add -A
git commit -m "Prepare app-name for catalog submission"
git push
git rev-parse HEAD
# Copy this SHA for the manifest
```

### Step 4: Create the Manifest

In your catalog fork, create `applications/<Category>/<app_id>/manifest.yml`:

```yaml
sourcecode:
  type: git
  location:
    origin: https://github.com/Eris-Margeta/flipper-apps.git
    commit_sha: <your-commit-sha>
    subdir: apps/your-app
description: "@README.md"
changelog: "@changelog.md"
screenshots:
  - screenshots/screenshot1.png
  - screenshots/screenshot2.png
```

### Step 5: Submit Pull Request

```bash
cd path/to/flipper-application-catalog
git checkout -b your-username/app_id_version
git add applications/<Category>/<app_id>/manifest.yml
git commit -m "Add <app_name> to catalog"
git push -u origin your-username/app_id_version
```

Then open PR at: `https://github.com/your-username/flipper-application-catalog/pull/new/your-username/app_id_version`

## Updating an App

1. Make changes in this monorepo
2. Increment `fap_version` in `application.fam`
3. Update `changelog.md`
4. Commit and push
5. Get new commit SHA
6. Update `commit_sha` in catalog manifest
7. Submit new PR to catalog

## Validation

Before submitting, validate your manifest:

```bash
cd path/to/flipper-application-catalog
python3 -m venv venv
source venv/bin/activate
pip install -r tools/requirements.txt
python3 tools/bundle.py applications/<Category>/<app_id>/manifest.yml bundle.zip
```

## Resources

- [Official Contributing Guide](https://github.com/flipperdevices/flipper-application-catalog/blob/main/documentation/Contributing.md)
- [Manifest Format](https://github.com/flipperdevices/flipper-application-catalog/blob/main/documentation/Manifest.md)
- [App Manifest (.fam) Docs](https://developer.flipper.net/flipperzero/doxygen/app_manifests.html)
