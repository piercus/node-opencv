#include "OpticalFlow.h"
#include "Contours.h"
#include "Matrix.h"
#include "OpenCV.h"
#include "TCriteria.h"
#include <nan.h>

void OpticalFlow::Init(Handle<Object> target)
{
    Persistent<Object> inner;
    Local<Object> obj = NanNew<Object>();
    NanAssignPersistent(inner, obj);

    NODE_SET_METHOD(obj, "calcOpticalFlowPyrLK", CalcOpticalFlowPyrLK);

    target->Set(NanNew("opticalFlow"), obj);
}
inline cv::Size sizeFromArray(Handle<Value> jsArray)
{
    cv::Size patternSize;

    if (jsArray->IsArray())
    {
        Local<Object> v8sz = jsArray->ToObject();

        patternSize = cv::Size(v8sz->Get(0)->IntegerValue(), v8sz->Get(1)->IntegerValue());
    }
    else
    {
        JSTHROW_TYPE("Size is not a valid array");
    }

    return patternSize;
}
NAN_METHOD(OpticalFlow::CalcOpticalFlowPyrLK)
{
    NanEscapableScope();

    try {
        // Get the arguments

        // Arg 0 is the previous image
        Matrix* m0 = ObjectWrap::Unwrap<Matrix>(args[0]->ToObject());
        cv::Mat prevImg = m0->mat;

        // Arg 1 is the next image
        Matrix* m1 = ObjectWrap::Unwrap<Matrix>(args[1]->ToObject());
        cv::Mat nextImg = m1->mat;

        // Arg 2 is the previous points
        Local<Array> prevArray = Local<Array>::Cast(args[2]->ToObject());
        const unsigned int L = prevArray->Length();
        vector<cv::Point2f> prevPts;

        for(unsigned int i = 0; i < L; i++){
            Local<Array> prevPoint = Local<Array>::Cast(prevArray->Get(i)->ToObject());
            cv::Point2f prevPt;
            prevPt.x = prevPoint->Get(0)->NumberValue();
            prevPt.y = prevPoint->Get(1)->NumberValue();
            prevPts.push_back(prevPt);
        }

        //Arg 3 is the size
        cv::Size winSize = sizeFromArray(args[3]);
        int maxLevel= args[4]->IntegerValue();

        cv::TermCriteria criteria = ObjectWrap::Unwrap<TCriteria>(args[5]->ToObject())->tcriteria;
        int flags = args[6]->IntegerValue();
        double minEigThreshold = args[7]->NumberValue();

        /*printf("nextImg[0,0] %i, nextImg[0,1] %i, nextImg[1,0] %i, nextImg[1,1] %i\n", 
            nextImg.at<char>(0,0), 
            nextImg.at<char>(0,1), 
            nextImg.at<char>(1,0),
            nextImg.at<char>(1,1)
        );
        
        printf("prevImg[0,0] %i, prevImg[0,1] %i, prevImg[1,0] %i, prevImg[1,1] %i\n", 
            prevImg.at<char>(0,0), 
            prevImg.at<char>(0,1), 
            prevImg.at<char>(1,0),
            prevImg.at<char>(1,1)
        );

        printf("x %f , y %f, size [%i,%i], criteria [%f,%i,%i], maxLevel %i, flags %i, minEigThreshold %f\n",
            prevPts[1].x, 
            prevPts[1].y, 
            winSize.width, 
            winSize.height,
            criteria.epsilon,
            criteria.type,
            criteria.maxCount,
            maxLevel,
            flags,
            minEigThreshold);*/

        // Make an mat to hold the result image
        vector<cv::Point2f> nextPts;
        vector<uchar> status;
        vector<float> err;

        // Optical Flow
        cv::calcOpticalFlowPyrLK(
        	prevImg, nextImg, prevPts, 
        	nextPts, status, err, 
        	winSize, 
        	maxLevel, 
        	criteria, 
        	flags, 
        	minEigThreshold);

        // Wrap the output image
        v8::Local<v8::Object> result = NanNew<v8::Object>();

        // Local<Object> outNextPtsWrap = NanNew(Matrix::constructor)->GetFunction()->NewInstance();
        // Matrix *outNextPts = ObjectWrap::Unwrap<Matrix>(outNextPtsWrap);
        // outNextPts->mat = nextPts;


        v8::Local<v8::Array> arrNextPts = NanNew<Array>(nextPts.size());

        for (unsigned int i=0; i < nextPts.size(); i++){
            v8::Local<v8::Array> pt = NanNew<Array>(2);
            pt->Set(0, NanNew<Number>((double) nextPts[i].x));
            pt->Set(1, NanNew<Number>((double) nextPts[i].y));
            arrNextPts->Set(i, pt);
        }

        result->Set(NanNew("nextPts"), arrNextPts);

        // Local<Object> outStatusWrap = NanNew(Matrix::constructor)->GetFunction()->NewInstance();
        // Matrix *outStatus = ObjectWrap::Unwrap<Matrix>(outStatusWrap);
        // outStatus->mat = status;

        v8::Local<v8::Array> arrStatus = NanNew<Array>(status.size());

        for (unsigned int i=0; i < status.size(); i++){
            arrStatus->Set(i, NanNew<Integer>(status[i]));
        }

        result->Set(NanNew("status"), arrStatus);

        // Local<Object> outErrWrap = NanNew(Matrix::constructor)->GetFunction()->NewInstance();
        // Matrix *outErr = ObjectWrap::Unwrap<Matrix>(outErrWrap);
        // outErr->mat = err;

        v8::Local<v8::Array> arrErr = NanNew<Array>(err.size());

        for (unsigned int i=0; i < err.size(); i++){
            arrErr->Set(i, NanNew<Number>(0));//NanNew<Number>(err[i]));
        }

		result->Set(NanNew("err"), arrErr);
  
        // Return the output image
        NanReturnValue(result);

    } catch (cv::Exception &e) {
        const char *err_msg = e.what();
        NanThrowError(err_msg);
        NanReturnUndefined();
    }
}