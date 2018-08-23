/* Shared Use License: This file is owned by Derivative Inc. (Derivative) and
 * can only be used, and/or modified for use, in conjunction with 
 * Derivative's TouchDesigner software, and only if you are a licensee who has
 * accepted Derivative's TouchDesigner license or assignment agreement (which
 * also govern the use of this file).  You may share a modified version of this
 * file with another authorized licensee of Derivative's TouchDesigner software.
 * Otherwise, no redistribution or sharing of this file, with or without
 * modification, is permitted.
 */

#include "TOP_CPlusPlusBase.h"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"


class OpenCV3TOP : public TOP_CPlusPlusBase
{
public:
    OpenCV3TOP(const OP_NodeInfo *info);
    virtual ~OpenCV3TOP();

    virtual void		getGeneralInfo(TOP_GeneralInfo *) override;
    virtual bool		getOutputFormat(TOP_OutputFormat*) override;


    virtual void		execute(const TOP_OutputFormatSpecs*,
							OP_Inputs*,
							TOP_Context* context) override;


    virtual int32_t		getNumInfoCHOPChans() override;
    virtual void		getInfoCHOPChan(int32_t index,
								OP_InfoCHOPChan *chan) override;

    virtual bool		getInfoDATSize(OP_InfoDATSize *infoSize) override;
    virtual void		getInfoDATEntries(int32_t index,
									int32_t nEntries,
									OP_InfoDATEntries *entries) override;

	virtual void		setupParameters(OP_ParameterManager *manager) override;
	virtual void		pulsePressed(const char *name) override;

	virtual const char* getErrorString() override;

private:

    // We don't need to store this pointer, but we do for the example.
    // The OP_NodeInfo class store information about the node that's using
    // this instance of the class (like its name).
    const OP_NodeInfo*	myNodeInfo;

    // In this example this value will be incremented each time the execute()
    // function is called, then passes back to the TOP 
    int					myExecuteCount;

	double				myStep;


	// My Variables

	size_t				faceDetected;
	int					myErrors;  
	int					minScaledSize;
	int					maxScaledSize;
	bool				b_DetectEyes;
	bool				b_SanityCheck;
	bool				b_EqualizeHist;

	std::vector<cv::Rect>	v_Faces;



	/*
	0: No Error
	1: Face Cascade File Param Missing
	2: Eyes Cascade File Param Missing
	3: Face Cascade File Load Error
	4: Eyes Cascade File Load Error
	*/

	cv::CascadeClassifier   faceCascade;
	//cv::CascadeClassifier	eyesCascade;
	std::string				faceCascadeFName;
	//std::string				eyesCascadeFName;
	std::string				emptyString = "";

};
