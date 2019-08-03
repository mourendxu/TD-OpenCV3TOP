# TD-OpenCV3TOP
## Touchdesigner OpenCV3 C++ TOP for FaceDetect

_**Contributors**_

- **[Greg Finger of PLGRM Visuals](https://github.com/gregfinger)** - *Development, Demo Projects, Testing and Foreman/Instigator*
- **Da Xu** - *Development and Documentation*




_**The Binary Release has been built against:**_
Touchdesigner 2019.12330
- OpenCV version 3.4.4 for **Mac** and 3.4.5 for **Windows**.

_**The Binary Release has been tested against:**_
Touchdesigner Build 2019.17550


## Table of Contents
1. [Release Notes](https://github.com/mourendxu/TD-OpenCV3TOP#release-notes-v15-3312019)
2. [How Does It Work?](https://github.com/mourendxu/TD-OpenCV3TOP#how-does-it-work)
3. [Usage](https://github.com/mourendxu/TD-OpenCV3TOP#usage)
4. [Build Instructions](https://github.com/mourendxu/TD-OpenCV3TOP#build-instructions)
    1. [Building Checklist](https://github.com/mourendxu/TD-OpenCV3TOP#checklist-before-you-run-build)
    2. [OpenCV Build Instructions for Mac](https://github.com/mourendxu/TD-OpenCV3TOP#opencv-build-instructions-for-mac)
    3. [Build Instructions for Mac](https://github.com/mourendxu/TD-OpenCV3TOP#build-instructions-for-cv3-facedetect-top)
    4. [Build Instructions for Windows](https://github.com/mourendxu/TD-OpenCV3TOP#windows)
5. [Todos/Other](https://github.com/mourendxu/TD-OpenCV3TOP#other)


_**Functionality/Purpose:**_

This plugin adds the OpenCV FaceDetect functionality to TD via [Custom Operators](https://docs.derivative.ca/Custom_Operators). 


Though OpenCV has already been integrated into TD, it's done via Python, which is not terribly convenient when you have to deal with streams of data. This plugin now allows you to pipe data from any TOP straight into the OpenCV algorithms and spit out data in a way that can be acted upon immediately. It makes the whole process much more streamlined and therefore faster.





## Release Notes v1.5.1 8/3/2019
We are dropping support for the previous style of Cplusplus operators. If you need copies of those, binary or source code, please check the 1.5 tag.

We have tested this release against the latest stable build at this time, Build 2019.17550. Everything works as intended.



## Release Notes v1.5 3/31/2019
Yes, we skipped over a few minor versions. But since we added quite a few things to this release, we feel we deserve the 1.5.


- The plugin is now **cross-platform**. We now have binary releases for both **Mac** and **Windows**, along with the necessary code and project files to build it on your own, if you so choose.
- The plugin now supports both Touchdesigner **Stable** and **Experimental** branches. The Experimental branch has implemented a new way of dealing with C++ plugins, and it allows users to utilize C++ plugins without a Commercial or Pro License. Make sure you use the correct binary release for the branch of TD you are using.
- Greg Finger has created a demo toe file for the Touchdesigner Experimental branch.
- Extensive updates have been made to the documentation. We now include instructions on how to build OpenCV on Mac, as well as build instructions for the plugin.
- scaleFactor has been exposed to the user via parameters.
- The plugin now forces 32 bit float output format.




## How Does It Work?

- Once face(s) has been detected, the detection results are then passed out via **Info Dat AND Pixel Packing**.
Results are packed into the output frame (RGBA32Float) in the following manner:

- In the output frame, the 1st Pixel's R contains how many faces have been detected. 

- We then use the RGBA channels of the subsequent pixels to store (x, y, width, height) of the detected face(s), in the order that they have been detected.


- There is a Sanity Check toggle that let's you see exactly what the detector sees. *This feature is only available on Windows.*

- If no face was found, the first pixel will be 0, across its RGBA channels.


Please check Greg Finger's demo Touchdesigner project to see how to use this custom operator. 


## Additional Notes
We have exposed most of the parameters of the detectMulti call as custom parameters. Min and Max search sizes have been expressed as a ratio in relation to the input height. You can drop an Info CHOP down to see the exact size of each in pixels.



Try different cascade files, they do effect result and speed. You can find additional cascade files in the OpenCV3 distro.



## Usage
See Greg Finger's demo projects for usage, particularly on how to deal with the output of the plugin.


### Binary Release
We have restructured our directories to take full advantage of the new [Custom Operators folder location](https://docs.derivative.ca/Custom_Operators#Using_Custom_OPs_-_Plugin_Folder_Locations). With the latest build from Derivative, you should be able to just download the repo, and run the demo toe. No extra work needed.

We now include OpenCV with our releases. You no longer need to do that yourself.


#### - Installing Custom Operator
If you wan to "install" the plugin for all your projects, please follow the instructions from Touchdesigner Wiki on [how to install Custom OPs](https://docs.derivative.ca/Custom_Operators) 


## Build Instructions

### **Checklist/TODOs before you run Build**

* OpenCV Library. This includes the headers, dynamic library for compiling, and DLL or dylib for running. **Windows** users can download binary releases from OpenCV's website. **Mac** users will have to download the source code and build their own, build instructions for the OpenCV library are below.
* Set the correct Headers search path. This will most likely be *opencv_install_dir\build\include* on **Windows** and *opencv_install_dir/include* on **Mac**. 
* Set the correct Library search path. This will most likely be *opencv_install_dir\build\x64\vc15\lib* on **Windows** and *opencv_install_dir/lib* on **Mac**.
* Set the dynamic library name. See specific instructions below.



### OpenCV Build Instructions for Mac

1. Download and install [CMake binary](https://cmake.org/download/)
2. Download and extract OpenCV source code, i.e. */Users/your_name/Documents/opencv.3.4.4* 
3. Open CMake, Click on *Browse Source*, and navigate to the directory where you have extracted the source code to, i.e. */Users/your_name/Documents/opencv.3.4.4*
4. Create a build directory. i.e. */Users/your_name/Documents/opencv.3.4.4/build*
5. Click on *Browse Build*, and navigate to the build directory that you have created in Step 4.
6. Click *Conﬁgure*. Choose *Unix Makeﬁle* in the dialog that pops up. Then let CMake ﬁgure out the build conﬁgurations. 
7. Once Step 6 has ﬁnished, check **BUILD_opencv_world** option in the newly generated conﬁguration options.
8. Modify **CMAKE_INSTALL_PREFIX** to point to where you may want to install the build to. i.e. */Users/your_name/Documents/opencv_3.4.4*
9. Click *Generate* to generate the make ﬁles.
10. Open up Terminal, and cd into the build directory that you have created in Step 4.
11. Enter the command *make*. This will build the library.
12. Once the build has ﬁnished successfully, enter the command *make install* to install the built binaries, headers, etc into the directory you have speciﬁed in **CMAKE_INSTALL_PREFIX**.
13. Done.




### Build Instructions for CV3 FaceDetect TOP
#### Mac OSX
*You will need to make the following modifications to the xcode project that you've downloaded from our repos.*

At this point, you should have a binary build of the OpenCV library of your choice on your local hard drive. *opencv_install_dir* from here and on will be used to refer to this location.

1. Add/Modify Framework reference to opencv library. 
    1. If you are using our xcode project, which is recommended, you will need to first delete the existing reference to OpenCV world dylib, under **Frameworks** and **General -> Linked Frameworks and Libraries**, see below.
    2. Then navigate to **General -> Linked Frameworks and Libraries**. *If you don't know how to get to this panel, please ask Google*
    3. Use Finder and navigate to your OpenCV install directory, under *lib*, there should be several world libraries. Two of them will be aliases or symlinks to the actual build. 
    4. Then from Finder, Drag and Drop (DO NOT use the plus sign, this will dereference the alias) the alias with only the major and minor version, i.e. *libopencv_world.3.4.dylib*, on to **General -> Linked Frameworks and Libraries**. This ﬁle will be needed for linking and packaging.

2. Add/Modify Search Paths for Headers and Libraries 
    1. Navigate to **Build Settings -> Search Paths**. *Again, if you don't know how to get to this panel, please ask Google*
    2. Add *opencv_install_dir/include* to **Header Search Paths**
    3. Add *opencv_install_dir/lib* to **Library Search Paths**
    4. MAKE SURE you have added paths to both the Debug and Release schemes/profiles. You can check this by clicking on the triangle next to **Header Search Paths** and **Library Search Paths**

3. Add/Modify Copy Bundle Resources
    1. Navigate to **Build Phases -> Copy Bundle Resources**
    2. If there is already an item in this phase, remove it.
    3. Click the plus sign, and add from Frameworks, the major and minor named opencv lib, i.e. libopencv_world.3.4.dylib, that was added in Step 1. 

4. Build. Choose Build For Proﬁling, this will build the Release version of the plugin.

*The following steps are only necessary if you are using an OpenCV library that's below or above 3.4.* 

5. Once the build is successful and complete, by default, the compiled *.plugin will be saved in a temporary directory.
Please follow [these instructions](https://docs.derivative.ca/Experimental:Custom_Operators#Building_on_macOS_using_XCode) from Derivative to get at your plugin. Alternatively, you can also set your *File -> Project Settings -> Per-User Project Settings -> Advanced -> Build Location* to Custom and Relative to Workspace. This will put all your builds relative to your xcode project.

6. Once you have access to the compiled plugin, you need to open **Terminal** and run the following command on your plugin. *Make sure you are in the same directory as your plugin*
    1. *otool -L plugin_name.plugin/Contents/MacOS/executable_name* 
    
    Make sure you replace the plugin_name and executable names accordingly. *\*Tip, use tab to auto-complete your path and file names. Google tab auto complete if in doubt*
    
    2. In the output, look for the @loader_path string, speciﬁcally the library name(without the path) it is pointing to.

7. Add/Modify in Run Script Phase. The purpose of this step is to modify the executable to use the dylib that was copied into the plugin bundle during the Copy Resources phase. This way, you do not need to install Opencv in an expected path, it will also help with confusions caused by different versions, as homebrew is not very fond of different versions. 
    1. Go to Build Phase, Add the Run Script phase. If you are using the Xcode project we have provided, you will just need to modify the first line of the shell script, this step can be skipped.
    2. Add/Modify the following shell script (There are ONLY two lines. NOTE THE LINE BREAKS! Make sure DYLIB in the shell script is set to the same lib name you got from Step 6. Name ONLY, no paths, as the path will be added for you by the script. 

    ````
    export DYLIB=libopencv_world.3.4.dylib 
    install_name_tool -change @rpath/$DYLIB @loader_path/../ Resources/$DYLIB “$TARGET_BUILD_DIR/$TARGET_NAME.plugin/ Contents/MacOS/$PRODUCT_NAME”
    ````

8. Build again. The resulting plugin bundle will contain the needed dylib and the executable will know where to find it.

9. Whew!


#### Windows

1.  Install OpenCV. There are ofﬁcial binary releases, it’s recommended that you use one of those. 
    1. Download the version of binary release you intend to use, and extract to a directory, i.e. *C:\OpenCV3.4*

2. Update Header Path and Linking Path, for the project, to point to the installed OpenCV
**BE AWARE** of which Proﬁle you are modifying the properties for. For most of the items below, you can choose All, exceptions will be noted.
    1. Modify Header Path - Right click on your Project (NOTE: Not the Solution), choose Properties 
        1. Under **C/C++ -> General -> Additional Include Directories**, modify the value to include your *opencv_install_dir\build\include* 
    2. Modify Linker/Library Path
        1. Under **Linker -> General -> Additional Library Directories**, modify the value to include your *opencv_install_dir\build\x64\vc15\lib\*. Based on empirical evidence, using either vc14 or vc15 doesn't seem to make a difference.
        2. Under **Linker -> Input -> Additional Dependecies**, you need to add/ modify the name of the opencv_world lib that’s in the lib directory you added in the step above. \*Name ONLY, no path. You can just type it in, make sure you add a semicolon to separate it from the other entries.
            1. For Release Conﬁguration, use the standard lib, i.e. *opencv_world345.lib*
            2. For Debug Conﬁguration, use the debug lib, i.e. *opencv_world345d.lib*

3. Build
    1. In the main window, under the top menus, Select *Release* and *x64* for Solution Conﬁguration.
    2. From the **Build** menu, choose **Build Solution**. If this is **NOT** your ﬁrst time building this solution, I personally prefer to use Rebuild Solution. Just to flush out any compiling gremlins. 
    3. If you are building for the Experimental Touchdesigner branch, you will need to first [install the plugin](https://docs.derivative.ca/Experimental:Custom_Operators). Either way, ou will need to copy opencv world dll into the same directory as the compiled plugin dll to run the plugin. The opencv dll should be located in *opencv_install_dir\build\x64\vc15\bin*. 



### Other
* If the CS gods be willing, we might do a CUDA version of this plugin. The documentation might be as long as this one.
* Have fun and go make a TD version of TikTok.
