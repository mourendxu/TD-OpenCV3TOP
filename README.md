# TD-OpenCV3TOP
Touchdesigner OpenCV3 C++ TOP for FaceDetect


Adding the OpenCV3 FaceDetect Functionality to TD via a C++ TOP. This will require a **Commercial or a Pro license** for TD. You do gain quite a bit of speed when compared to using Python and the OpenCV that came with Touchdesigner. This method is also much more streamlined than the Python approach. 



Created by Da Xu, at the request of Greg Finger of PLGRM Visuals.

This project was written with OpenCV3 3.4.1. But it should compile just fine with the 3.4.2, which is the latest version of OpenCV3.

The code should also work on Mac. You just need to setup your own XCode project and compile your own OpenCV3 binaries, as there isn't a binary distro of the lib for Mac.






## FEATURES

- Once face(s) has been detected, the detected face(s)' info are passed out via **Info Dat AND Pixel Packing**.
Info is packed into the output frame (RGBA32Float) in the following manner:

- The 1st Pixel's R contains how many faces have been detected. 

- I use the RGBA channels of the subsequent pixels to keep (x, y, width, height) of the detected face, in the order that they have been detected.


- There is a Sanity Check toggle that let's you see exactly what the detector sees.


Please check Greg Finger's demo Touchdesigner project to see how to use this C++ TOP. AGAIN, you WILL NEED a Commercial or Pro license.

## Additional Notes
I have exposed most of the parameters of the detectMulti call as customm parameters. Min and Max search sizes have been expressed as a ratio in relation to the input height. You can drop an Info TOP down to see the exact size of each in pixels.



Try different cascade files, they do effect result and speed. You can find additional cascade files in the OpenCV3 distro.



## Demo
I have included a compiled version of the DLL. However, you still need to download the OpenCV dlls from opencv.org to make the demo run. You also need the latest version of Touchdesigner, which at this moment is 2018.25000.
