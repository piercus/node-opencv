#ifndef __NODE_OPTICALFLOW_H
#define __NODE_OPTICALFLOW_H

#include "OpenCV.h"

// Implementation of imgproc.hpp functions

class OpticalFlow: public node::ObjectWrap {
public:
    static void Init(Handle<Object> target);

    static NAN_METHOD(CalcOpticalFlowPyrLK);

};

#endif