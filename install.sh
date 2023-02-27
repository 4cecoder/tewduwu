#!/usr/bin/env bash

# Check if tewduwu binary exists
if [ -x "$(command -v ./tewduwu)" ]; then
  echo "tewduwu binaiwee found, instawwing to /usr/local/bin..."
  cp ./tewduwu /usr/local/bin/
  echo "tewduwu installed successfuwwy."
else
  echo "tewduwu binary not found."
  echo "Please buiwd it using one of the build.sh scripts before running this install scwipt."
fi
