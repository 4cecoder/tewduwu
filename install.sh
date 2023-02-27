#!/usr/bin/env bash

# Check if tewduwu binary exists
if [ -x "$(command -v ./tewduwu)" ]; then
  echo "tewduwu binary found, installing to /usr/local/bin..."
  cp ./tewduwu /usr/local/bin/
  echo "tewduwu installed successfully."
else
  echo "tewduwu binary not found. Please build it using build.sh before running this script."
fi
