#include "TCriteria.h"
#include "OpenCV.h"
#include <nan.h>

v8::Persistent<FunctionTemplate> TCriteria::constructor;

void
TCriteria::Init(Handle<Object> target) {
    NanScope();

    //Class
    Local<FunctionTemplate> ctor = NanNew<FunctionTemplate>(TCriteria::New);
    NanAssignPersistent(constructor, ctor);
    ctor->InstanceTemplate()->SetInternalFieldCount(1);
    ctor->SetClassName(NanNew("TCriteria"));

    target->Set(NanNew("TCriteria"), ctor->GetFunction());
};

NAN_METHOD(TCriteria::New) {
    NanScope();

    if (args.This()->InternalFieldCount() == 0)
        NanThrowTypeError("Cannot instantiate without new");

    TCriteria *term;

    if (args.Length() == 0){
        term = new TCriteria;
    } else { //if (args.Length() == 3){
        int type = args[0]->IntegerValue();
        int maxCount = args[1]->IntegerValue();
        double epsilon = args[2]->NumberValue();
        term = new TCriteria(type, maxCount, epsilon);
    }

    term->Wrap(args.Holder());
    NanReturnValue(args.Holder());
}


TCriteria::TCriteria(): ObjectWrap() {
    tcriteria = cv::TermCriteria();
}

TCriteria::TCriteria(int type, int maxCount, double epsilon): ObjectWrap() {
    tcriteria = cv::TermCriteria(type, maxCount, epsilon);
}
