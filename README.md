## HUB75 TestBed

A testbed for [ESP32-HUB75-MatrixPanel-I2S-DMA](https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA) library.
It allows to configure HUB75 DMA engine and set virtual pane via WebUI. A set of simple shapes, fills, gradients and drawings are available for panel testing and debugging.

## How to build

The project could be build with [PlatformIO](https://platformio.org/).

Steps to run:
 - Configuring and building firmware
 - Making a LittleFS image file with web resources
 - Uploading firmware
 - Uploading LittleFS image
 
### External Libs used to build/use the firmware
- [EmbUI](https://github.com/vortigont/EmbUI) framework is used to construct Web interface, manage WiFi, store configuration, etc
- [FTPClientServer](https://github.com/charno/FTPClientServer) to access ESP's file system (optional)


### Configuring and Building
To build fw for ESP32 run:
```sh
platformio run
```


### Making a LittleFS image file with web resources
To handle WebUI it is required to build a LittleFS image and upload it to the controller. The image contains files from the [EmbUI](https://github.com/vortigont/EmbUI) framework and js/css files for the ESPEM project. The is no prebuild image since it is hard to maintain it, instead there is a shell script that downloads required files from github, repacks it and places under `/data` directory. Tha directory is used to create and upload LittleFS image to the controller. Run
```sh
cd resources
./respack.sh
```
It should populate `/data` dir with `js`, `css`, `index.html.gz`, etc...

Download and extract to this project root a *mklittlefs* executable for your OS from a [zipped binary here](https://github.com/earlephilhower/mklittlefs/releases)

Now the FS image and firmware could be uploaded to the controller
```sh
platformio run -t uploadfs
platformio run -t upload
```

That's it. Controller should reboot and enable WiFi.
