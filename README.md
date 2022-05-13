Required libraries: zlib and libzip <br>
Required QT: 5.x+ (1) <br>
Required projects: eternity_common and xv2ins_common 

(1) Currently the slots editor require and old version of the QT 5.x branch. <br>
This is because DDS support was removed at some point in QT, but the installer still uses it. <br>
This will be addressed in the future by using eternity_common/DdsFile along with DirectXTex library.
