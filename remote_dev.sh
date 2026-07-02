//written by Google Gemini Flash

#!/bin/bash

# --- CONFIGURATION ---
SSH_USER="cham"
SSH_HOST="desktop-jj36384"
REMOTE_DIR="C:/Users/cham/GPawn-Analyzer"

echo "=== 1. Syncing Source Code to Windows GPU Server ==="
# Create the remote directory first to ensure it exists
ssh "$SSH_USER@$SSH_HOST" "mkdir -p $REMOTE_DIR"

# Tar the local files (excluding build blocks) and pipe them directly over SSH to extract on Windows
# This is incredibly fast and avoids rsync completely
tar --exclude='./build' --exclude='./.git' --exclude='./.DS_Store' -czf - . | \
    ssh "$SSH_USER@$SSH_HOST" "tar -xzf - -C $REMOTE_DIR"

if [ $? -ne 0 ]; then
    echo "❌ Error: Code sync failed!"
    exit 1
fi

echo "=== 2. Triggering Clean Remote Build & Test Execution ==="
ssh "$SSH_USER@$SSH_HOST" << EOF
    cd "$REMOTE_DIR"
    
    :: Clean and recreate build directory using Windows syntax
    if exist build ( rmdir /s /q build )
    mkdir build
    cd build
    
    echo [Remote] Running CMake...
    cmake .. -DCMAKE_BUILD_TYPE=Release
    
    echo [Remote] Compiling Project...
    cmake --build . --config Release
    
    if %ERRORLEVEL% NEQ 0 (
        echo ❌ [Remote] Compilation Failed!
        exit /b 1
    )
    
    echo [Remote] Running Executable Instantly...
    echo --------------------------------------------------
    Release\GPawnAnalyzer.exe
    echo --------------------------------------------------
EOF

echo "=== 3. Pipeline Run Complete ==="
