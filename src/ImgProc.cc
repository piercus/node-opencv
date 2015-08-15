#include "ImgProc.h"
#include "Matrix.h"
#include <iostream>                                                                                                                                 
#include "v8.h"
#include "node.h"
#include <string>
#include <vector>
using namespace std;
using namespace v8;
using namespace node;

void ImgProc::Init(Handle<Object> target)
{
    Persistent<Object> inner;
    Local<Object> obj = NanNew<Object>();
    NanAssignPersistent(inner, obj);

    NODE_SET_METHOD(obj, "undistort", Undistort);
    NODE_SET_METHOD(obj, "initUndistortRectifyMap", InitUndistortRectifyMap);
    NODE_SET_METHOD(obj, "remap", Remap);
    NODE_SET_METHOD(obj, "calcHist", CalcHist);
    NODE_SET_METHOD(obj, "normalize", Normalize);

    target->Set(NanNew("imgproc"), obj);
}

// cv::undistort
NAN_METHOD(ImgProc::Undistort)
{
    NanEscapableScope();

    try {
        // Get the arguments

        // Arg 0 is the image
        Matrix* m0 = ObjectWrap::Unwrap<Matrix>(args[0]->ToObject());
        cv::Mat inputImage = m0->mat;

        // Arg 1 is the camera matrix
        Matrix* m1 = ObjectWrap::Unwrap<Matrix>(args[1]->ToObject());
        cv::Mat K = m1->mat;

        // Arg 2 is the distortion coefficents
        Matrix* m2 = ObjectWrap::Unwrap<Matrix>(args[2]->ToObject());
        cv::Mat dist = m2->mat;

        // Make an mat to hold the result image
        cv::Mat outputImage;

        // Undistort
        cv::undistort(inputImage, outputImage, K, dist);

        // Wrap the output image
        Local<Object> outMatrixWrap = NanNew(Matrix::constructor)->GetFunction()->NewInstance();
        Matrix *outMatrix = ObjectWrap::Unwrap<Matrix>(outMatrixWrap);
        outMatrix->mat = outputImage;

        // Return the output image
        NanReturnValue(outMatrixWrap);

    } catch (cv::Exception &e) {
        const char *err_msg = e.what();
        NanThrowError(err_msg);
        NanReturnUndefined();
    }
}

// cv::initUndistortRectifyMap
NAN_METHOD(ImgProc::InitUndistortRectifyMap)
{
    NanEscapableScope();

    try {

        // Arg 0 is the camera matrix
        Matrix* m0 = ObjectWrap::Unwrap<Matrix>(args[0]->ToObject());
        cv::Mat K = m0->mat;

        // Arg 1 is the distortion coefficents
        Matrix* m1 = ObjectWrap::Unwrap<Matrix>(args[1]->ToObject());
        cv::Mat dist = m1->mat;

        // Arg 2 is the recification transformation
        Matrix* m2 = ObjectWrap::Unwrap<Matrix>(args[2]->ToObject());
        cv::Mat R = m2->mat;

        // Arg 3 is the new camera matrix
        Matrix* m3 = ObjectWrap::Unwrap<Matrix>(args[3]->ToObject());
        cv::Mat newK = m3->mat;

        // Arg 4 is the image size
        cv::Size imageSize;
        if (args[4]->IsArray()) {
            Local<Object> v8sz = args[4]->ToObject();

            imageSize = cv::Size(v8sz->Get(1)->IntegerValue(), v8sz->Get(0)->IntegerValue());
        } else {
            JSTHROW_TYPE("Must pass image size");
        }

        // Arg 5 is the first map type, skip for now
        int m1type = args[5]->IntegerValue();

        // Make matrices to hold the output maps
        cv::Mat map1, map2;

        // Compute the rectification map
        cv::initUndistortRectifyMap(K, dist, R, newK, imageSize, m1type, map1, map2);

        // Wrap the output maps
        Local<Object> map1Wrap = NanNew(Matrix::constructor)->GetFunction()->NewInstance();
        Matrix *map1Matrix = ObjectWrap::Unwrap<Matrix>(map1Wrap);
        map1Matrix->mat = map1;

        Local<Object> map2Wrap = NanNew(Matrix::constructor)->GetFunction()->NewInstance();
        Matrix *map2Matrix = ObjectWrap::Unwrap<Matrix>(map2Wrap);
        map2Matrix->mat = map2;

        // Make a return object with the two maps
        Local<Object> ret = NanNew<Object>();
        ret->Set(NanNew<String>("map1"), map1Wrap);
        ret->Set(NanNew<String>("map2"), map2Wrap);

        // Return the maps
        NanReturnValue(ret);


    } catch (cv::Exception &e) {
        const char *err_msg = e.what();
        NanThrowError(err_msg);
        NanReturnUndefined();
    }
}

// cv::remap
NAN_METHOD(ImgProc::Remap)
{
    NanEscapableScope();

    try {
        // Get the arguments

        // Arg 0 is the image
        Matrix* m0 = ObjectWrap::Unwrap<Matrix>(args[0]->ToObject());
        cv::Mat inputImage = m0->mat;

        // Arg 1 is the first map
        Matrix* m1 = ObjectWrap::Unwrap<Matrix>(args[1]->ToObject());
        cv::Mat map1 = m1->mat;

        // Arg 2 is the second map
        Matrix* m2 = ObjectWrap::Unwrap<Matrix>(args[2]->ToObject());
        cv::Mat map2 = m2->mat;

        // Arg 3 is the interpolation mode
        int interpolation = args[3]->IntegerValue();

        // Args 4, 5 border settings, skipping for now

        // Output image
        cv::Mat outputImage;

        // Remap
        cv::remap(inputImage, outputImage, map1, map2, interpolation);

        // Wrap the output image
        Local<Object> outMatrixWrap = NanNew(Matrix::constructor)->GetFunction()->NewInstance();
        Matrix *outMatrix = ObjectWrap::Unwrap<Matrix>(outMatrixWrap);
        outMatrix->mat = outputImage;

        // Return the image
        NanReturnValue(outMatrixWrap);

    } catch (cv::Exception &e) {
        const char *err_msg = e.what();
        NanThrowError(err_msg);
        NanReturnUndefined();
    }
}

// by piercus
//cv::calcHist
NAN_METHOD(ImgProc::CalcHist){
    NanEscapableScope();

    try {
        // Get the arguments

        // Arg 0 is the image
        Matrix* m0 = ObjectWrap::Unwrap<Matrix>(args[0]->ToObject());
        cv::Mat inputImage = m0->mat;

        // Arg 1 is the nimages
        int nimages = args[1]->IntegerValue();

        // Arg 2 List of the dims channels used to compute the histogram. 
        //The first array channels are numerated from 0 to images[0].channels()-1 , 
        //the second array channels are counted from images[0].channels() to images[0].channels() + images[1].channels()-1, and so on.
        unsigned int L0;
        if(args[2]->IsArray()){
            v8::Handle<v8::Array> jsChannels = v8::Handle<v8::Array>::Cast(args[2]);
            L0 = jsChannels->Length();
        } else {
            L0 = 1;
        }

        int channels[L0];

        if(args[2]->IsArray()){
            v8::Handle<v8::Array> jsChannels = v8::Handle<v8::Array>::Cast(args[2]);
            for (unsigned int i = 0; i < L0; i++) {
               channels[i] = jsChannels->Get(i)->Uint32Value();
            } 
        } else {
            channels[0] = args[2]->Uint32Value();
        }

        // Arg 3 is the second map for mask
        Matrix* maskIn = ObjectWrap::Unwrap<Matrix>(args[3]->ToObject());
        cv::Mat inputMask = cv::Mat(maskIn->mat);

        //Arg 4 is dims
        int dims = args[4]->IntegerValue();

        // Arg 5 is histSizes array
        
        
        unsigned int L1;
        
        if(args[5]->IsArray()){    
            v8::Handle<v8::Array> jsHistSizes = v8::Handle<v8::Array>::Cast(args[5]);
            L1 = jsHistSizes->Length();
        } else {
            L1 = 1;
        }

        int vHistSizes[L1];
        

        if(args[5]->IsArray()){
            v8::Handle<v8::Array> jsHistSizes = v8::Handle<v8::Array>::Cast(args[5]);
            for (unsigned int i = 0; i < L1; i++) {
               vHistSizes[i] = jsHistSizes->Get(i)->Uint32Value();
            }   
        } else {
            vHistSizes[0] = args[5]->IntegerValue();
        }


        // args 6 is hardcoded
        v8::Handle<v8::Array> jsRanges = v8::Handle<v8::Array>::Cast(args[6]);

        unsigned int L2 = jsRanges->Length();
        const float* ranges[L2];
        float sranges[L2][2];
        for (unsigned int i = 0; i < L2; i++) {
            sranges[i][0] = jsRanges->Get(i)->ToObject()->Get(0)->NumberValue();
            sranges[i][1]= jsRanges->Get(i)->ToObject()->Get(1)->NumberValue(); 
            ranges[i] = sranges[i];
        }

        // Args 7 uniform 
        bool uniform = (args.Length() >= 8) ? args[7]->BooleanValue() : true;

        // Args 8 accumulated doesn't mean anything from js
        bool accumulated = false;

        // Output hist
        cv::Mat hist;

        cv::calcHist(&inputImage, nimages, channels, inputMask, hist, dims, vHistSizes, ranges, uniform, accumulated);   

        // Wrap the output hist
        Local<Object> outHistWrap = NanNew(Matrix::constructor)->GetFunction()->NewInstance();
        Matrix *outHist = ObjectWrap::Unwrap<Matrix>(outHistWrap);
        outHist->mat = hist;

        // Return the image
        NanReturnValue(outHistWrap);

    } catch (cv::Exception &e) {
        const char *err_msg = e.what();
        NanThrowError(err_msg);
        NanReturnUndefined();
    }

}


NAN_METHOD(ImgProc::Normalize){
    NanEscapableScope();

    try {
        // Get the arguments

        // Arg 0 is the image
        Matrix* m0 = ObjectWrap::Unwrap<Matrix>(args[0]->ToObject());
        cv::Mat src = m0->mat;

        // Arg 1 is the nimages
        double alpha = args[1]->IntegerValue();
        double beta = args[2]->IntegerValue();
        // for normType see values in http://docs.opencv.org/java/constant-values.html#org.opencv.core.Core.NORM_MINMAX
        int normType = args[3]->IntegerValue();
        int dtype = args[4]->IntegerValue();

        Matrix* m5 = ObjectWrap::Unwrap<Matrix>(args[5]->ToObject());
        cv::Mat mask = m5->mat;        

        // Output hist
        cv::Mat dst;

        cv::normalize(src, dst, alpha, beta, normType, dtype, mask);

        // Wrap the output hist
        Local<Object> outDstWrap = NanNew(Matrix::constructor)->GetFunction()->NewInstance();
        Matrix *outDst = ObjectWrap::Unwrap<Matrix>(outDstWrap);
        outDst->mat = dst;



        // Return the image
        NanReturnValue(outDstWrap);

    } catch (cv::Exception &e) {
        const char *err_msg = e.what();
        NanThrowError(err_msg);
        NanReturnUndefined();
    }

}