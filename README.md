Tweaked version of 
emeocv
======

Read and recognize the counter of an electricity meter with OpenCV.

![OpenCV for the electricity meter](https://github.com/wof2/emeocv/blob/master/emeocv.png)

![OpenCV for the electricity meter statistics](https://github.com/wof2/emeocv/blob/master/emeocv2.png)

Rationale 
=============
I couldn't make the original program (https://github.com/skaringa/emeocv) work on my counter. Its extaction algoritm is based on digit height in pixels. My counter has multiple lines of printed text of size similar to the actual digits. I didn't like the idea of masking the unwanted text on counter. 

Also the original algorithm didn't work for my setup where my mobile phone moved a bit during inspection of the results after a few days of working. I've come up with a bit more sophisiticated algoritm that finds the area of interest based on two pieces of paper sticky notes. You are now more flexible with your camera mount which can be moved with no significant effect on the recognition efficiency.

The third reason was the problem with configuring my Raspberry Pi camera (ArduCam OV5647) to take reliable show shutter shots. I've spent many hours trying to get the proper v4l2-ctl configuration (low framerate, high exposition, night-mode etc). Nothing worked. Finally I used Rapsbian's tool 'raspistill' that takes great shots with no tweaking needed. Now ypu can use -k option to run any command line to provide images for recognition. This way you could even use emeocv to transfer images from remote machine and analyze them locally (usefull for mobile phone setup).

The final change is the output format - I don't like RRD for its unpleasant interface and bad looking diagrams. CSV is the choice  for me. It can be then managed with any program you like. I used Google spreadsheets ().


Changes to original emeocv
=============
* Alternative digits extraction algorithm (you need 2 small color markers now). 
* Saves results to CSV format instead of RRDtool (take a look at the results here: https://docs.google.com/spreadsheets/d/e/2PACX-1vQxh9ScohMIchp7ZPCb14Tf4HCx9nxCGeZ6HHjwop5cn3TON8AjzHbRnm3P407_hFohKvVHLNZScnzn/pubhtml)
* Auto-install of dependecies for Debian/Ubuntu/Linux Mint/Raspbian (just run ./debian_ubuntu_dependencies.sh)
* Rescaling of analyzed images based on configuration option (maxImageHeight) - usefull if you use mobile phone to take snapshots
* You can now use numpad keys to train the algorithm
* You can now print training set statistics understand which digits are coved and which need more tranining (use -p option)
* Configuration file is now reloaded after every image in adjust mode (-a). This way you can tweak your config (cannyThreshold, digitYAlignment) during runtime

Tested with OpenCV 3.2 in 2 configurations:
* Mobile phone as a camera: Samsung Galaxy J3 with 'Network timelapse camera' App (https://play.google.com/store/apps/details?id=com.survey7.cameraupload_full)
* Raspbian on Raspberry PI ZERO W or Raspberry Pi 4 2GB and ArduCam OV5647 camera 

Prerequisites
=============
* g++, make
* Run debian_ubuntu_dependencies.sh to get everything below... 

or:
* OpenCV 2 or 3 (Debian/Ubuntu/Raspbian: `apt-get install libopencv-dev`) - developed with OpenCV 2.3.1 and later ported to OpenCV 3.2.0
* RRDtool (`apt-get install rrdtool librrd-dev`)
* log4cpp (`apt-get install liblog4cpp5-dev`)
* libexif (`apt-get install libexif-dev`)


Build
=====

    cd emeocv
    make #for Debug purposes    
    make RELEASE=true #for production purposes
    
        
Usage
=====
    Navigate to emeocv Debug or Release subdirectory. Then run:

    Usage: ./emeocv [-i <dir>|-c <cam>|k] [-l|-t|-a|-w|-o <dir>] [-s <delay>] [-v <level>

    Image input:
      -i <image directory> : read image files (png) from directory.
      -c <camera number> : read images from camera.
      -k : read images from camera. Use CLI command provided in config.yml

    Operation:
      -a : adjust camera.
      -o <directory> : capture images into directory.
      -l : learn OCR.
      -t : test OCR.
      -p : print OCR training set statistics.
      -w : write OCR data to RR database. This is the normal working mode.

    Options:
      -s <n> : Sleep n milliseconds after processing of each image (default=1000).
      -v <l> : Log level. One of DEBUG, INFO, ERROR (default).
  
    
Color Markers
=====  
![Color markers](https://github.com/wof2/emeocv/blob/master/emeocv3.png)

Please remember to stick the color markers to hint th position of the counter digits. Take a look to at the picture above to get the idea of the proper placement. Blue markers worked the best for me. You can set any color (in HSV Model - H: 0-179, S: 0-255, V: 0-255) in config.yml:
    counterMarkerHLOW: 85 # Hue value minimum
    counterMarkerHHI: 110 # Hue value maximum
    counterMarkerSLOW: 120 # Minimum saturation level
    counterMarkerVLOW: 120 # Minimum 'Value' level
You can take a picture of your marker, open it in any graphical editor or online tool to get the right values (remember that GIMP has different scale: H = 0-360, S = 0-100 and V = 0-100). 

Google Spreadsheet
=====  
Clone my google Spreadsheet and paste your own CSV to get nice diagrams and a pivot table: https://docs.google.com/spreadsheets/d/1ENgyxMoM0D1TPhFQw3DrsRWjOFa0U0sc-wnM41yUeaI/edit?usp=sharing.


Tutorial
=====  
There is a tutorial that explains use case and function of the original program:
[OpenCV practice: OCR for the electricity meter](https://www.mkompf.com/cplus/emeocv.html) or
[OpenCV Praxis: OCR für den Stromzähler](https://www.kompf.de/cplus/emeocv.html) (in german language).


License
=======
Copyright 2015,2019 Martin Kompf

Changes by wof2 under same license

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
