#include "camera/abstract_camera.h"


#include <iostream>
#include <string>

using namespace std;

class CAM;
//enum CAM::ft;
//class CAM::FP;
enum ft {INTTYPE, FLOATTYPE, STRTYPE};
class FP
{
    friend class CAM;
protected:
//    FP() = delete;
//public:
    FP(CAM *cam = nullptr):_cam(cam) {}

public:
    template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    operator T() {
        switch ( _cam->_cameraFeatureToAccess->type() ) {

        }
    }

private:
    CAM *_cam;
};

//typedef AbstractCamera<ft, std::string, std::string, FP, std::string, std::string, char, int, double, std::string> AC;
typedef AbstractCamera<ft, std::string, FP, std::string, char, int, double, std::string> AC;
class CAM: public AC
{
public:
    enum LID {cam, api, bl};

    CAM(ostream* ls): AbstractCamera(ls), _aa(10) {
        _cameraFeatureValue = new FP(this);
        defineCommand(new CameraCommand("STARTEXP",
                                        std::bind(static_cast<void(CAM::*)()>
                                                  (&CAM::startExp), this) )
                      );

        defineFeature(new CameraFeature<int>("A",INTTYPE,ReadWrite_AccessType,{0,10},
                                             std::bind(static_cast<int(CAM::*)()>
                                                       (&CAM::get_a), this),
                                             std::bind(static_cast<void(CAM::*)(const int)>
                                                       (&CAM::set_a), this, std::placeholders::_1),
                                             nullptr)
                      );
    }

    ~CAM() {delete _cameraFeatureValue;}

    void startExp() {}

    bool isInitialized() { return true;}

    void logToFile( const LID lid, const std::string &ls, const int t = 0) {
        *_logStreamPtr << "AAA: ";
        AC::logToFile(ls);
//        AC::logHelper(ls);
    }

protected:
    int get_a() {return _aa;}
    void set_a(const int a) {_aa = a;}

    int _aa;
};

int main()
{
    CAM cam(&std::cout);

    cam("STARTEXP");

//    int a = cam["A"];

//    cam["A"] = 22;

//    long aa = (int)cam["A"];

////    double f = cam["A"];

//    int b;
//    b = cam["A"];

//    cout << "a = " << a << "\n";
//    cout << "aa = " << aa << "\n";
////    cout << "f = " << f << "\n";
}
