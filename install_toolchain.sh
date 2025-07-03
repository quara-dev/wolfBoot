#!/bin/bash
#
# ARM GCC Toolchain Installation Script
# Installs ARM GCC toolchain for embedded development
#

set -e  # Exit on any error

# Configuration
ARM_TOOLCHAIN_URL="https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz"
ARM_TOOLCHAIN_FILENAME="$(basename ${ARM_TOOLCHAIN_URL})"
ARM_TOOLCHAIN_EXTRACT_DIRECTORY="/opt/$(basename ${ARM_TOOLCHAIN_FILENAME} .tar.xz)"
INSTALL_PREFIX="/usr/local"
TEMP_DIR="/tmp"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Helper functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Show help message
show_help() {
    cat << EOF
ARM GCC Toolchain Installation Script

Usage: $0 [OPTIONS]

OPTIONS:
    --prefix=DIR    Install toolchain binaries to DIR/bin (default: /usr/local)
    --help, -h      Show this help message

Examples:
    $0                          # Install to /usr/local/bin (default)
    $0 --prefix=/opt/arm-gcc    # Install to /opt/arm-gcc/bin
    sudo $0 --prefix=/usr       # Install to /usr/bin (system-wide)

Note: You may need sudo privileges depending on the installation directory.
EOF
}

# Parse command line arguments
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --prefix=*)
                INSTALL_PREFIX="${1#*=}"
                shift
                ;;
            --prefix)
                INSTALL_PREFIX="$2"
                shift 2
                ;;
            --help|-h)
                show_help
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
}

# Check system requirements and dependencies
check_dependencies() {
    log_info "Checking system dependencies..."
    
    local missing_deps=()
    
    # Check for required tools
    if ! command -v tar >/dev/null 2>&1; then
        missing_deps+=("tar")
    fi
    
    if ! command -v wget >/dev/null 2>&1 && ! command -v curl >/dev/null 2>&1; then
        missing_deps+=("wget or curl")
    fi
    
    # Check for xz-utils (needed for .tar.xz files)
    if ! command -v xz >/dev/null 2>&1; then
        missing_deps+=("xz-utils")
    fi
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        log_error "Missing required dependencies: ${missing_deps[*]}"
        log_info "Please install them using your package manager:"
        
        # Detect package manager and suggest installation command
        if command -v apt-get >/dev/null 2>&1; then
            log_info "  sudo apt-get update && sudo apt-get install tar wget xz-utils"
        elif command -v yum >/dev/null 2>&1; then
            log_info "  sudo yum install tar wget xz"
        elif command -v dnf >/dev/null 2>&1; then
            log_info "  sudo dnf install tar wget xz"
        elif command -v pacman >/dev/null 2>&1; then
            log_info "  sudo pacman -S tar wget xz"
        else
            log_info "  Install: tar, wget (or curl), and xz-utils using your package manager"
        fi
        
        exit 1
    fi
    
    log_info "All required dependencies are available"
}

# Check if running as root for system-wide installation
check_permissions() {
    if [[ $EUID -eq 0 ]]; then
        log_info "Running as root - will install system-wide"
    else
        log_warn "Not running as root - checking write permissions for ${INSTALL_PREFIX}/bin"
        
        # Create install directory if it doesn't exist
        if [[ ! -d "${INSTALL_PREFIX}/bin" ]]; then
            if ! mkdir -p "${INSTALL_PREFIX}/bin" 2>/dev/null; then
                log_error "Cannot create directory ${INSTALL_PREFIX}/bin. Please run with sudo or choose a different --prefix."
                exit 1
            fi
        fi
        
        # Check if we can write to the install directory
        if [[ ! -w "${INSTALL_PREFIX}/bin" ]]; then
            log_error "Cannot write to ${INSTALL_PREFIX}/bin. Please run with sudo or choose a different --prefix."
            exit 1
        fi
        
        log_info "Write permissions confirmed for ${INSTALL_PREFIX}/bin"
        log_warn "You may need to add ${INSTALL_PREFIX}/bin to your PATH"
    fi
}

# Download toolchain
download_toolchain() {
    log_info "Downloading ARM GCC toolchain..."
    cd ${TEMP_DIR}
    
    if [[ -f "${ARM_TOOLCHAIN_FILENAME}" ]]; then
        log_info "Toolchain archive already exists, skipping download"
    else
        if command -v wget >/dev/null 2>&1; then
            wget --progress=dot:giga "${ARM_TOOLCHAIN_URL}"
        elif command -v curl >/dev/null 2>&1; then
            curl -L -o "${ARM_TOOLCHAIN_FILENAME}" "${ARM_TOOLCHAIN_URL}"
        else
            log_error "Neither wget nor curl found. Please install one of them."
            exit 1
        fi
        
        # Verify download
        if [[ ! -f "${ARM_TOOLCHAIN_FILENAME}" ]]; then
            log_error "Download failed - ${ARM_TOOLCHAIN_FILENAME} not found"
            exit 1
        fi
        
        log_info "Download completed successfully"
    fi
}

# Extract and install toolchain
install_toolchain() {
    log_info "Extracting toolchain to ${ARM_TOOLCHAIN_EXTRACT_DIRECTORY}..."
    
    # Create extraction directory
    if [[ $EUID -eq 0 ]]; then
        mkdir -p "${ARM_TOOLCHAIN_EXTRACT_DIRECTORY}"
    else
        sudo mkdir -p "${ARM_TOOLCHAIN_EXTRACT_DIRECTORY}"
    fi
    
    # Extract toolchain
    if [[ $EUID -eq 0 ]]; then
        tar xJf "${ARM_TOOLCHAIN_FILENAME}" -C /opt
    else
        sudo tar xJf "${ARM_TOOLCHAIN_FILENAME}" -C /opt
    fi
    
    # Remove downloaded archive
    rm "${ARM_TOOLCHAIN_FILENAME}"
    
    log_info "Installing toolchain binaries to ${INSTALL_PREFIX}/bin..."
    
    # Navigate to bin directory
    cd "${ARM_TOOLCHAIN_EXTRACT_DIRECTORY}/bin/"
    
    # Remove documentation to save space
    if [[ $EUID -eq 0 ]]; then
        rm -rf ../share/doc 2>/dev/null || true
    else
        sudo rm -rf ../share/doc 2>/dev/null || true
    fi
    
    # Create symlinks to binaries
    local installed_count=0
    for file in * ; do
        if [[ -f "${file}" && -x "${file}" ]]; then
            log_info "Installing ${file} -> ${INSTALL_PREFIX}/bin/${file}"
            if [[ $EUID -eq 0 ]]; then
                ln -sf "${PWD}/${file}" "${INSTALL_PREFIX}/bin/${file}"
            else
                # For non-root installs, just copy the files
                cp "${file}" "${INSTALL_PREFIX}/bin/${file}"
                chmod +x "${INSTALL_PREFIX}/bin/${file}"
            fi
            ((installed_count++))
        fi
    done
    
    log_info "Installed ${installed_count} ARM toolchain binaries"
}

# Verify installation
verify_installation() {
    log_info "Verifying installation..."
    
    # Check if install directory is in PATH
    if [[ ":$PATH:" != *":${INSTALL_PREFIX}/bin:"* ]]; then
        log_warn "${INSTALL_PREFIX}/bin is not in your PATH"
        export PATH="${INSTALL_PREFIX}/bin:$PATH"
        log_info "Temporarily added ${INSTALL_PREFIX}/bin to PATH for verification"
    fi
    
    if command -v arm-none-eabi-gcc >/dev/null 2>&1; then
        local version=$(arm-none-eabi-gcc --version | head -n1)
        log_info "Successfully installed: ${version}"
        
        # Show some key tools
        log_info "Available ARM tools:"
        local tools=(
            "arm-none-eabi-gcc"
            "arm-none-eabi-g++"
            "arm-none-eabi-objdump"
            "arm-none-eabi-objcopy"
            "arm-none-eabi-size"
            "arm-none-eabi-gdb"
        )
        
        for tool in "${tools[@]}"; do
            if command -v "$tool" >/dev/null 2>&1; then
                echo "  ✓ $tool"
            else
                echo "  ✗ $tool (not found)"
            fi
        done
        
        return 0
    else
        log_error "Installation verification failed. arm-none-eabi-gcc not found in PATH."
        log_warn "You may need to add ${INSTALL_PREFIX}/bin to your PATH:"
        log_warn "  export PATH=\"${INSTALL_PREFIX}/bin:\$PATH\""
        return 1
    fi
}

# Cleanup function
cleanup() {
    log_info "Cleaning up temporary files..."
    cd /
    rm -f "${TEMP_DIR}/${ARM_TOOLCHAIN_FILENAME}"
}

# Main installation process
main() {
    # Parse command line arguments
    parse_args "$@"
    
    log_info "Starting ARM GCC Toolchain Installation"
    log_info "Toolchain URL: ${ARM_TOOLCHAIN_URL}"
    log_info "Extract location: ${ARM_TOOLCHAIN_EXTRACT_DIRECTORY}"
    log_info "Install prefix: ${INSTALL_PREFIX}"
    
    # Check system requirements
    if [[ -f /etc/os-release ]]; then
        local distro=$(grep '^ID=' /etc/os-release | cut -d'=' -f2 | tr -d '"')
        log_info "Detected Linux distribution: ${distro}"
    else
        log_warn "Cannot detect Linux distribution"
    fi
    
    # Check system requirements and permissions
    check_dependencies
    check_permissions
    
    # Set trap for cleanup
    trap cleanup EXIT
    
    # Perform installation
    download_toolchain
    install_toolchain
    
    # Verify installation
    if verify_installation; then
        log_info ""
        log_info "🎉 ARM GCC Toolchain installation completed successfully!"
        log_info ""
        log_info "To use the toolchain, ensure ${INSTALL_PREFIX}/bin is in your PATH:"
        log_info "  export PATH=\"${INSTALL_PREFIX}/bin:\$PATH\""
        log_info ""
        log_info "Add this to your ~/.bashrc or ~/.zshrc to make it permanent:"
        log_info "  echo 'export PATH=\"${INSTALL_PREFIX}/bin:\$PATH\"' >> ~/.bashrc"
        log_info ""
        log_info "You can now build ARM projects with commands like:"
        log_info "  arm-none-eabi-gcc --version"
        log_info "  arm-none-eabi-objdump --version"
        log_info ""
        log_info "To build WolfBoot with Meson:"
        log_info "  meson setup builddir --cross-file cross/arm-none-eabi.txt"
        log_info "  meson compile -C builddir"
    else
        log_error "Installation completed but verification failed"
        exit 1
    fi
}

# Run main function if script is executed directly
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi