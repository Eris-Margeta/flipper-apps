# Flipper Apps Justfile
# Usage:
#   just install-all     - Install all apps to Flipper (excludes _template)
#   just install <app>   - Install a specific app by folder name

# Default recipe
default:
    @just --list

# Install a specific app by folder name
install app:
    #!/usr/bin/env bash
    set -euo pipefail
    if [ "{{app}}" = "_template" ]; then
        echo "Error: Cannot install template"
        exit 1
    fi
    if [ ! -d "apps/{{app}}" ]; then
        echo "Error: App '{{app}}' not found in apps/"
        echo "Available apps:"
        ls -1 apps/ | grep -v "^_" | grep -v "\.DS_Store"
        exit 1
    fi
    echo "Installing {{app}}..."
    cd "apps/{{app}}" && poetry run python -m ufbt launch

# Install all apps (excludes _template)
install-all:
    #!/usr/bin/env bash
    set -euo pipefail
    echo "Installing all apps to Flipper..."
    for app in apps/*/; do
        app_name=$(basename "$app")
        # Skip template and hidden files
        if [[ "$app_name" == _* ]] || [[ "$app_name" == .* ]]; then
            continue
        fi
        echo ""
        echo "=== Installing $app_name ==="
        cd "$app" && poetry run python -m ufbt launch
        cd - > /dev/null
    done
    echo ""
    echo "All apps installed!"

# List available apps
list:
    @echo "Available apps:"
    @ls -1 apps/ | grep -v "^_" | grep -v "\.DS_Store"

# Build a specific app (without installing)
build app:
    #!/usr/bin/env bash
    set -euo pipefail
    if [ "{{app}}" = "_template" ]; then
        echo "Error: Cannot build template"
        exit 1
    fi
    if [ ! -d "apps/{{app}}" ]; then
        echo "Error: App '{{app}}' not found"
        exit 1
    fi
    echo "Building {{app}}..."
    cd "apps/{{app}}" && poetry run python -m ufbt

# Build all apps
build-all:
    #!/usr/bin/env bash
    set -euo pipefail
    echo "Building all apps..."
    for app in apps/*/; do
        app_name=$(basename "$app")
        if [[ "$app_name" == _* ]] || [[ "$app_name" == .* ]]; then
            continue
        fi
        echo ""
        echo "=== Building $app_name ==="
        cd "$app" && poetry run python -m ufbt
        cd - > /dev/null
    done
    echo ""
    echo "All apps built!"

# Clean build artifacts for a specific app
clean app:
    #!/usr/bin/env bash
    set -euo pipefail
    if [ ! -d "apps/{{app}}" ]; then
        echo "Error: App '{{app}}' not found"
        exit 1
    fi
    echo "Cleaning {{app}}..."
    cd "apps/{{app}}" && poetry run python -m ufbt -c

# Clean all build artifacts
clean-all:
    #!/usr/bin/env bash
    set -euo pipefail
    echo "Cleaning all apps..."
    for app in apps/*/; do
        app_name=$(basename "$app")
        if [[ "$app_name" == _* ]] || [[ "$app_name" == .* ]]; then
            continue
        fi
        echo "Cleaning $app_name..."
        cd "$app" && poetry run python -m ufbt -c
        cd - > /dev/null
    done
    echo "Done!"
