# Setup

fish
. enable_esp_idf.fish
cd esp32_dash
idf.py build
idf.py -p /dev/serial/by-id/usb-Espressif_xyz flash