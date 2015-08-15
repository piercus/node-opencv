#ifndef __NODE_IMGPROC_H
#define __NODE_IMGPROC_H

#include "OpenCV.h"

// Implementation of imgproc.hpp functions

class ImgProc: public node::ObjectWrap {
public:
	cv::Mat mask;
	
    static void Init(Handle<Object> target);

    static NAN_METHOD(Undistort);

    static NAN_METHOD(InitUndistortRectifyMap);

    static NAN_METHOD(Remap);

    static NAN_METHOD(CalcHist);

    static NAN_METHOD(Normalize);
};

#endif
