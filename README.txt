This repo is a collection of files needed to program one of the "ESP32 S3 Development Board with 1.77 Inch LCD Display BT 2.5dbi WiFi Module AI Internet of Things W5500 ETH N16R8 CH340K Type-C" being sold on AliExpress, among other places.  This board caught my eye because it's relatively inexpensive but includes a number of features that are not present on many ESP32 development boards.  This has an ESP32 S3 CPU, TF card slot, an external WiFi antenna, built-in LCD display, and a Ethernet port, among other things.  All in all, a lot of stuff in a nice package.

I ordered a couple of them but it came with no documentation at all, including the vendor and manufacturer's web sites.  I reached out to the seller on AliExpress and they referred me to a Google Drive URL at:

https://drive.google.com/drive/folders/1Gxr7WR7fbcamoN3QuOrcjBe8Z2pYCBrl?usp=sharing

This has much of what was needed, especially the schematic.

I'm using the Arduino toolchain for this board but it needs to be treated as a "Generic ESP32 S3" board since there is no board library for this specific board yet.  I'm working on a demonstration application to try to show how to use all the various components on the board and the start of that is in the Examples/LCD folder.

If you happen across this repo and find or develop anything you think might be helpful to others, please create a PR.  Thanks!
