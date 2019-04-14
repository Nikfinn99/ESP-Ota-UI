# ESP-Ota-UI

This is a small tool to help with uploading to ESP8266 boards.
Current features include managing multiple OTA devices, flashing Program or SPIFFS memory over Serial or WiFi.

The entire UI status is saved in `OTA-Config.ini` when the tool is closed, so once you open the program again, everything sould be as as it was.

The project was created using `Qt Creator 4.7.2` and `Qt 5.11.2`.
Newer versions are likely to work also.

## Installation
The releases are created for Windows.

1. Download the DLL Bundle from releases (first release)
2. Download the .exe and .py files from the release you want to use
3. Create a Folder for this Tool
4. Extract dll files and move all downloaded files together with ESP-Ota-UI.exe, esptool.exe and espota.py into this folder
5. Happy Uploading!

## Usage

1. Select compiled .bin File.

#### Serial Upload
2. Select `Serial` as `Upload Method`
3. Select COM Port
4. Select Upload Speed
5. (optional) Erase Flash (this will also clear wifi settings)
6. Select `Program` or `SPIFFS` as target location.
7. Click Upload
8. Switch to `Console` tab for output of upload tool

#### WiFi OTA Upload
2. Switch to `Devices` tab
3. Enter new device name at the bottom
4. Select corresponding IP Adress for this device
5. Click `Add Device`
6. Switch to `Upload Tab`
7. Select `IP` as `Upload Method`
8. Select the device you want to upload to from `Address`
9. (optional) Select upload port
10. (optional) Select upload password
11. (recommended) Check `Debug` checkbox (enables popup messages and progress bar)
12. Select `Program` or `SPIFFS` as target location.
13. Click Upload
14. The upload progress is displayed on the `Upload` tab.
