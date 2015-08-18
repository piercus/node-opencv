#include "OpenCV.h"

class TCriteria: public node::ObjectWrap {
  public:

    cv::TermCriteria tcriteria;
    static Persistent<FunctionTemplate> constructor;
    static void Init(Handle<Object> target);
    static NAN_METHOD(New);
    TCriteria();
    TCriteria(int type, int maxCount, double epsilon);

};
