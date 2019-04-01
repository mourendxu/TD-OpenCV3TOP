/* Shared Use License: This file is owned by Derivative Inc. (Derivative) and
 * can only be used, and/or modified for use, in conjunction with 
 * Derivative's TouchDesigner software, and only if you are a licensee who has
 * accepted Derivative's TouchDesigner license or assignment agreement (which
 * also govern the use of this file).  You may share a modified version of this
 * file with another authorized licensee of Derivative's TouchDesigner software.
 * Otherwise, no redistribution or sharing of this file, with or without
 * modification, is permitted.
 */



/*
OpenCV3 FaceDetect C++ TOP by Da Xu
Last Modified: 2018/8/23

Purpose: 
		 Adding the OpenCV3 FaceDetect Functionality to TD via a C++ TOP
		 This will require a Commercial or a Pro license for TD. 
		 You do gain quite a bit of speed and this method is much more streamlined than the python approach.


Features: 
         Once face(s) has been detected, the detected face(s)' info are passed out via Info Dat AND Pixel Packing.
		 Info is packed into the output frame (RGBA32Float) in the following manner:
		 1st Pixel's R contains how many faces have been detected. 
		 I use the RGBA channels of the consequent pixels to keep (x, y, width, height) of the detected face, in the order
		 that they have been detected.

*/

#include "CV3FaceDetectTOP.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <cmath>

using namespace std;


// Includes for OpenCV3
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"


using namespace cv;


// These functions are basic C function, which the DLL loader can find
// much easier than finding a C++ Class.
// The DLLEXPORT prefix is needed so the compile exports these functions from the .dll
// you are creating
extern "C"
{
    DLLEXPORT
    void
    FillTOPPluginInfo(TOP_PluginInfo *info)
    {
        // This must always be set to this constant
        info->apiVersion = TOPCPlusPlusAPIVersion;
        
        // Change this to change the executeMode behavior of this plugin.
        info->executeMode = TOP_ExecuteMode::CPUMemWriteOnly;
        
        // The opType is the unique name for this TOP. It must start with a
        // capital A-Z character, and all the following characters must lower case
        // or numbers (a-z, 0-9)
        info->customOPInfo.opType->setString("Cv3facedetecttop");
        
        // The opLabel is the text that will show up in the OP Create Dialog
        info->customOPInfo.opLabel->setString("CV3 FaceDetect");
        
        // Will be turned into a 3 letter icon on the nodes
        info->customOPInfo.opIcon->setString("FDT");
        
        // Information about the author of this OP
        info->customOPInfo.authorName->setString("Da Xu");
        info->customOPInfo.authorEmail->setString("da.xu@wearesohappy.net");
        
        // This TOP works with 0 or 1 inputs connected
        info->customOPInfo.minInputs = 0;
        info->customOPInfo.maxInputs = 1;
    }
    
    DLLEXPORT
    TOP_CPlusPlusBase*
    CreateTOPInstance(const OP_NodeInfo* info, TOP_Context* context)
    {
        // Return a new instance of your class every time this is called.
        // It will be called once per TOP that is using the .dll
        return new OpenCV3TOP(info);
    }
    
    DLLEXPORT
    void
    DestroyTOPInstance(TOP_CPlusPlusBase* instance, TOP_Context *context)
    {
        // Delete the instance here, this will be called when
        // Touch is shutting down, when the TOP using that instance is deleted, or
        // if the TOP loads a different DLL
        delete (OpenCV3TOP*)instance;
    }
    
};

OpenCV3TOP::OpenCV3TOP(const OP_NodeInfo* info) : myNodeInfo(info)
{
	myExecuteCount = 0;
	myStep = 0.0;

	faceCascadeFName = "";
	//eyesCascadeFName = "";
	faceDetected = 0;


}

OpenCV3TOP::~OpenCV3TOP()
{

}

void
OpenCV3TOP::getGeneralInfo(TOP_GeneralInfo* ginfo, const OP_Inputs* inputs, void* reserved1)
{
	// Uncomment this line if you want the TOP to cook every frame even
	// if none of it's inputs/parameters are changing.
	//ginfo->cookEveryFrame = false;
	ginfo->cookEveryFrame = true;
	ginfo->memPixelType = OP_CPUMemPixelType::RGBA32Float;
	//ginfo->memPixelType = OP_CPUMemPixelType::BGRA8Fixed;
}

bool
OpenCV3TOP::getOutputFormat(TOP_OutputFormat* format, const OP_Inputs* inputs, void* reserved1)
{
	// In this function we could assign variable values to 'format' to specify
	// the pixel format/resolution etc that we want to output to.
	// If we did that, we'd want to return true to tell the TOP to use the settings we've
	// specified.
	// In this example we'll return false and use the TOP's settings

    format->bitsPerChannel = 32.0;
    format->floatPrecision = true;

	return true;
}

void
OpenCV3TOP::execute(TOP_OutputFormatSpecs* outputFormat,
                    const OP_Inputs* inputs,
                    TOP_Context *context,
                    void* reserved1)
{
	myExecuteCount++;



	//My Code

	//Reset member variables
	myErrors = 0; //Reset errors
	faceDetected = 0;
	v_Faces.clear();


	//Bringing in the input buffer
	const OP_TOPInput* input = inputs->getInputTOP(0);

	OP_TOPInputDownloadOptions options;
	options.downloadType = OP_TOPInputDownloadType::Delayed;
	options.cpuMemPixelType = OP_CPUMemPixelType::BGRA8Fixed;
	options.verticalFlip = true;  //Mat stores the image with bottom row as the first row, so, we need to flip the image first


	const uchar* src = (const uchar*)inputs->getTOPDataInCPUMemory(input, &options);


	//No input, then return
	if (!src)
		return;



	//Reading in the custom parameters
	const char *FaceCascadePar = inputs->getParFilePath("Facecascadexml");
	//const char *EyesCascadePar = inputs->getParFilePath("Eyescascadexml");
	double min_scale = inputs->getParDouble("Minscale");
	double max_scale = inputs->getParDouble("Maxscale");
    double scale_factor = inputs->getParDouble("Scalefactor");
	int nearest_neighbor = inputs->getParInt("Nearestneigh");

	//b_DetectEyes = (inputs->getParInt("Detecteyes") == 1);
#ifdef WIN32
	b_SanityCheck = (inputs->getParInt("Sanitycheck") == 1);
#else
    b_SanityCheck = false;
#endif
	b_EqualizeHist = (inputs->getParInt("Equalizehist") == 1);




	//Check if Face Cascade Exists
	if (emptyString.compare(FaceCascadePar) == 0) {

		myErrors = 1;
		return;

	} else if (faceCascadeFName.compare(FaceCascadePar) != 0) {

		if (!faceCascade.load(FaceCascadePar)) {

			myErrors = 3;
			return;

		}

		faceCascadeFName = FaceCascadePar;

	}

	//Check if Eyes Cascade Exists
	/*
	if (emptyString.compare(EyesCascadePar) == 0) {

		myErrors = 2;
		return;

	} else if (eyesCascadeFName.compare(EyesCascadePar) != 0) {

		if (!eyesCascade.load(EyesCascadePar)) {

			myErrors = 4;
			return;
		}

		eyesCascadeFName = EyesCascadePar;

	}
	*/


	int height = input->height;
	int width = input->width;

	//Scale the search min and max using scale factor and input height
	minScaledSize = height * min_scale;
	maxScaledSize = height * max_scale;


	//Loading input buffer into cv::Mat
	Mat frame(height, width, CV_8UC4, (void*)src);
	Mat frameGray;
	
	//Converting to Gray-scale
	cvtColor(frame, frameGray, COLOR_BGRA2GRAY);

	//Equalizing the image if asked
	//Though, we have found that not equalizing gives better results.
	if (b_EqualizeHist) {

		equalizeHist(frameGray, frameGray);

	}
		

	
	//Detect them faces
	faceCascade.detectMultiScale(frameGray, v_Faces, scale_factor, nearest_neighbor, 0|CASCADE_SCALE_IMAGE, Size(minScaledSize, minScaledSize), Size(maxScaledSize, maxScaledSize));	

	//How many faces were detected?
	faceDetected = v_Faces.size();


	//Pixel packing array. 
	//Setting it to nullptr, just in case.
	float* faceInfo = nullptr;

	int textureMemoryLocation = 0;

	//Getting ouptut buffer
	float* mem = (float*)outputFormat->cpuPixelData[textureMemoryLocation];

	/*
	Clearning the output buffer, as we have ran into issues of a corrupt frame when the input reduces in resolution to anything quarter and below.
	memset is super fast, so you shouldn't lose much if any performance over this.
	Though, this doesn't clear the entire output buffer due to how TD handles changing resolutions, feel free to remove this.
	*/
	memset(mem, 0, outputFormat->height * outputFormat->width * 4);

	if (faceDetected) {
		
		//Pixel Packing

		//Allocating memory for pixel packing 
		//# of faces plus 1 pixels. The extra pixel, the first pixel is for storing how many faces have been detected
		faceInfo = new float[(faceDetected+1)*4]();
		
		faceInfo[0] = (float) faceDetected;
		faceInfo[1] = faceInfo[2] = faceInfo[3] = 0;

		//Packing in them info
		for (size_t i = 1; i <= faceDetected; i++) {

			faceInfo[i*4 ] = (float)v_Faces[i-1].x;
			faceInfo[i*4 + 1] = (float)v_Faces[i-1].y;
			faceInfo[i*4 + 2] = (float)v_Faces[i-1].width;
			faceInfo[i*4 + 3] = (float)v_Faces[i-1].height;

			if (b_SanityCheck) {

				cv::rectangle(frameGray, Point(v_Faces[i].x, v_Faces[i].y), Point(v_Faces[i].x + v_Faces[i].width, v_Faces[i].y + v_Faces[i].height), 1, LINE_8, 0);
				
			}

		}

		//I could write directly into mem, but this way is just more sane. 
		//Computers are so fast these days(esp if you are running this code), by not writing directly into buffer, you lose maybe a fraction of a frame.
		//Plus, the code is bit more readable this way.
		memcpy(mem, faceInfo, sizeof(float) * 4 * (faceDetected+1));

		delete[] faceInfo;
	}
	else {

		
		faceInfo = new float[4]{ 0, 0, 0, 0 }; 
		memcpy(mem, faceInfo, sizeof(float) * 4 ); 
		delete[] faceInfo;
	}

	if (b_SanityCheck) {
		//See what the detect call sees
		imshow("Sanity Check", frameGray);
	}
	

    outputFormat->newCPUPixelDataLocation = textureMemoryLocation;
    textureMemoryLocation = !textureMemoryLocation;
}




int32_t
OpenCV3TOP::getNumInfoCHOPChans(void *reserved1)
{
	// We return the number of channel we want to output to any Info CHOP
	// connected to the TOP. In this example we are just going to send one channel.
	return 6;
}

void
OpenCV3TOP::getInfoCHOPChan(int32_t index, OP_InfoCHOPChan* chan, void* reserved1)
{
	// This function will be called once for each channel we said we'd want to return
	// In this example it'll only be called once.

	if (index == 0)
	{
        chan->name->setString("executeCount");
		chan->value = (float)myExecuteCount;
	}

	

	if (index == 1) {
        chan->name->setString("load_errors");
		chan->value = (float)myErrors;

	}

	if (index == 2) {
		chan->name->setString("face_detected");
		chan->value = (float)faceDetected;
	}

	if (index == 3)
	{
		chan->name->setString("minScaledSize");
		chan->value = (float)minScaledSize;
	}

	if (index == 4) {

		chan->name->setString("maxScaledSize");
		chan->value = (float)maxScaledSize;
	}
	/*
	if (index == 5) {

		chan->name = "detect_eyes";
		if (b_DetectEyes) {
			chan->value = 1.0;
		}
		else {

			chan->value = 0.0;
		}
	}
	*/
}


//const char* 
//OpenCV3TOP::getErrorString() {
void OpenCV3TOP::getErrorString(OP_String *error, void *reserved1) {

    if (myErrors == 1) {

        error->setString("Face Cascade XML File is missing");

    }

}


bool		
OpenCV3TOP::getInfoDATSize(OP_InfoDATSize* infoSize, void* reserved1)
{
	infoSize->rows = 4;
	infoSize->cols = 50;  //Assuming a max of 50 faces, which is alot of faces
	// Setting this to false means we'll be assigning values to the table
	// one row at a time. True means we'll do it one column at a time.
	infoSize->byColumn = false;
	return true;
}

void
OpenCV3TOP::getInfoDATEntries(int32_t index,
                              int32_t nEntries,
                              OP_InfoDATEntries* entries,
                              void *reserved1)
{
	// It's safe to use static buffers here because Touch will make it's own
	// copies of the strings immediately after this call returns
	// (so the buffers can be reuse for each column/row)
	static char tempBuffer1[4096];
//    static char tempBuffer2[4096];
	static char tmpBuffer[50][50];

	//Sanity O Sanity
	size_t maxOutput = faceDetected;
	if (faceDetected >= nEntries) {

		maxOutput = nEntries;
	}
	
	//X
	if (index == 0) {
#ifdef WIN32
		strcpy_s(tempBuffer1, "fx");
#else // macOS
		strlcpy(tempBuffer1, "fx", sizeof(tempBuffer1));
#endif
		entries->values[0]->setString(tempBuffer1);

		for (size_t i = 0; i < maxOutput; i++) {
#ifdef WIN32
			sprintf_s(tmpBuffer[i], "%d", v_Faces[i].x);
#else // macOS
			snprintf(tmpBuffer[i], sizeof(tmpBuffer[i]), "%d", v_Faces[i].x);
#endif
			entries->values[i+1]->setString(tmpBuffer[i]);
		}
	}

	//Y
	if (index == 1) {
#ifdef WIN32
		strcpy_s(tempBuffer1, "fy");
#else // macOS
		strlcpy(tempBuffer1, "fy", sizeof(tempBuffer1));
#endif
		entries->values[0]->setString(tempBuffer1);

		for (size_t i = 0; i < maxOutput; i++) {
#ifdef WIN32
			sprintf_s(tmpBuffer[i], "%d", v_Faces[i].y);
#else // macOS
			snprintf(tmpBuffer[i], sizeof(tmpBuffer[i]), "%d", v_Faces[i].y);
#endif
			entries->values[i+1]->setString(tmpBuffer[i]);
		}
	}

	//Height
	if (index == 2) {
#ifdef WIN32
		strcpy_s(tempBuffer1, "fheight");
#else // macOS
		strlcpy(tempBuffer1, "fheight", sizeof(tempBuffer1));
#endif
		entries->values[0]->setString(tempBuffer1);

		for (size_t i = 0; i < maxOutput; i++) {
#ifdef WIN32
			sprintf_s(tmpBuffer[i], "%d", v_Faces[i].height);
#else // macOS
			snprintf(tmpBuffer[i], sizeof(tmpBuffer[i]), "%d", v_Faces[i].height);
#endif
			entries->values[i+1]->setString(tmpBuffer[i]);
		}
	}

	//Width
	if (index == 3) {
#ifdef WIN32
		strcpy_s(tempBuffer1, "fwidth");
#else // macOS
		strlcpy(tempBuffer1, "fwidth", sizeof(tempBuffer1));
#endif
		entries->values[0]->setString(tempBuffer1);

		for (size_t i = 0; i < maxOutput; i++) {
#ifdef WIN32
			sprintf_s(tmpBuffer[i], "%d", v_Faces[i].width);
#else // macOS
			snprintf(tmpBuffer[i], sizeof(tmpBuffer[i]), "%d", v_Faces[i].width);
#endif
			entries->values[i+1]->setString(tmpBuffer[i]);
		}
	}



}

void
OpenCV3TOP::setupParameters(OP_ParameterManager* manager, void *reserved1)
{


	//Face Cascade
	{

		OP_StringParameter sp;

		sp.name = "Facecascadexml";
		sp.label = "Face Cascade XML";
		OP_ParAppendResult res = manager->appendFile(sp);
		assert(res == OP_ParAppendResult::Success);

	}

	//Eyes Cascade
	/*
	{

		OP_StringParameter sp;

		sp.name = "Eyescascadexml";
		sp.label = "Eyes Cascade XML";

		OP_ParAppendResult res = manager->appendFile(sp);
		assert(res == OP_ParAppendResult::Success);
	}
	*/

	// Cascade Minimum Search Scale, search box will be height * scale
    {
        OP_NumericParameter    np;
        
        np.name = "Minscale";
        np.label = "Cascade Min Search Scale";
        
        np.minSliders[0] = 0.0001;
        np.maxSliders[0] = 1.0;
        
        np.minValues[0] = 0.0001;
        np.maxValues[0] = 1.0;
        
        np.clampMins[0] = true;
        np.clampMaxes[0] = true;
        
        np.defaultValues[0] = 0.05;
        
        OP_ParAppendResult res = manager->appendFloat(np);
        assert(res == OP_ParAppendResult::Success);
    }
    
    // Cascade Max Search Scale, search box will be height * scale
    {
        OP_NumericParameter    np;
        
        np.name = "Maxscale";
        np.label = "Cascade Max Search Scale";
        
        np.minSliders[0] = 0.0001;
        np.maxSliders[0] = 1.0;
        
        np.minValues[0] = 0.0001;
        np.maxValues[0] = 1.0;
        
        np.clampMins[0] = true;
        np.clampMaxes[0] = true;
        
        np.defaultValues[0] = 0.75;
        
        OP_ParAppendResult res = manager->appendFloat(np);
        assert(res == OP_ParAppendResult::Success);
    }
    
    //Scale Factor
    {
        OP_NumericParameter np;
        np.name = "Scalefactor";
        np.label = "Scale Factor";
        
        np.minSliders[0] = 1.0001;
        np.maxSliders[0] = 2.0;
        
        np.minValues[0] = 1.0001;
        np.maxValues[0] = 2.0;
        
        np.clampMins[0] = true;
        np.clampMaxes[0] = true;
        
        np.defaultValues[0] = 1.1;
        
        OP_ParAppendResult res = manager->appendFloat(np);
        assert(res == OP_ParAppendResult::Success);
    }
    
    //Nearest Neighbor
    {
        OP_NumericParameter np;
        np.name = "Nearestneigh";
        np.label = "Nearest Neighbor";
        np.defaultValues[0] = 3;
        
        np.minSliders[0] = 1;
        np.maxSliders[0] = 10;
        
        np.minValues[0] = 1;
        np.maxValues[0] = 100;
        
        np.clampMins[0] = true;
        np.clampMaxes[0] = true;
        
        OP_ParAppendResult res = manager->appendInt(np);
        assert(res == OP_ParAppendResult::Success);
    }

	//Eyes Cascade Off and On Toggle
	/*
	{
		OP_NumericParameter np;
		np.name = "Detecteyes";
		np.label = "Detect Eyes";
		np.defaultValues[0] = 0;


		OP_ParAppendResult res = manager->appendToggle(np);
		assert(res == OP_ParAppendResult::Success);
	}

	*/

	//Turn On or Off
	{
		OP_NumericParameter np;
		np.name = "Equalizehist";
		np.label = "Equalize Histogram";
		np.defaultValues[0] = 0;


		OP_ParAppendResult res = manager->appendToggle(np);
		assert(res == OP_ParAppendResult::Success);
	}
    
#ifdef WIN32
	//Sanity Check Toggle. Sanity check draws rectangle around found faces
	{

		OP_NumericParameter np;
		np.name = "Sanitycheck";
		np.label = "Sanity Check";
		np.defaultValues[0] = 0;

		OP_ParAppendResult res = manager->appendToggle(np);
		assert(res == OP_ParAppendResult::Success);

	}
#endif

	// pulse
	/*
	{
		OP_NumericParameter	np;

		np.name = "Reset";
		np.label = "Reset";
		
		OP_ParAppendResult res = manager->appendPulse(np);
		assert(res == OP_ParAppendResult::Success);
	}
	*/
}

void
OpenCV3TOP::pulsePressed(const char* name, void *reserved1)
{
	if (!strcmp(name, "Reset"))
	{
		myStep = 0.0;
	}


}

