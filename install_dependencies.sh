#!/bin/bash

# Update package list
sudo dnf update -y

# Install development tools and C++ compiler
sudo dnf groupinstall -y "Development Tools"
sudo dnf install -y gcc-c++ cmake

# Install TgBot-cpp dependencies
sudo dnf install -y openssl-devel zlib-devel boost-devel

# Install CPR dependencies
sudo dnf install -y libcurl-devel

# Install pugixml
sudo dnf install -y pugixml-devel

# Install nlohmann-json
sudo dnf install -y json-devel

# Clone and build TgBot-cpp
git clone https://github.com/reo7sp/tgbot-cpp.git
cd tgbot-cpp
cmake .
make
sudo make install
cd 

# Clone and build CPR
git clone https://github.com/whoshuu/cpr.git
cd cpr
mkdir build
cd build
cmake ..
make
sudo make install
cd

echo "All dependencies have been installed."
