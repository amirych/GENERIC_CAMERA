#include "camera/abstract_camera.h"


#include <iostream>
#include <string>

using namespace std;

class FP;
enum ft {INTTYPE, FLOATTYPE, STRTYPE};

//class CAM;
typedef AbstractCamera<ft, std::string, FP, std::string, char, int, double, std::string> AC;


class FP
{
//    friend class CAM;
    template<typename,typename,typename,typename,typename,typename ...> friend class AbstractCamera;
protected:
//    FP() = delete;
//public:
//    FP(CAM *cam = nullptr):_cam(cam) {}
    FP(AC *cam = nullptr):_cam(cam) {}

public:
    template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    operator T() {
        switch ( _cam->_cameraFeatureToAccess->type() ) {
        case INTTYPE: {
//            CAM::CameraFeature<int> *f = dynamic_cast<CAM::CameraFeature<int>*>(_cam->_cameraFeatureToAccess);
            AC::CameraFeature<int> *f = dynamic_cast<AC::CameraFeature<int>*>(_cam->_cameraFeatureToAccess);
            return f->get();
        }
        case FLOATTYPE: {
//            CAM::CameraFeature<double> *f = dynamic_cast<CAM::CameraFeature<double>*>(_cam->_cameraFeatureToAccess);
            AC::CameraFeature<double> *f = dynamic_cast<AC::CameraFeature<double>*>(_cam->_cameraFeatureToAccess);
            return f->get();
        }
        default: {
            cerr << "ERROR!!!\n";
        }
        }
    }

    operator std::string() {
        AC::CameraFeature<std::string> *f = dynamic_cast<AC::CameraFeature<std::string>*>(_cam->_cameraFeatureToAccess);
        return f->get();
    }

private:
//    CAM *_cam;
    AC *_cam;
};

//typedef AbstractCamera<ft, std::string, std::string, FP, std::string, std::string, char, int, double, std::string> AC;
class CAM: public AC
{
public:
    enum LID {cam, api, bl};

    CAM(ostream* ls): AbstractCamera(ls), _aa(10), _ss("AAA") {
//        _cameraFeatureValue = new FP(this);
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

        defineFeature( new CameraFeature<std::string>("S",STRTYPE,ReadWrite_AccessType,{"A","B","C"},
                                                      [this](){return _ss;},
                                                      [this](const std::string s){_ss = s;})
                     );
    }

//    ~CAM() {delete _cameraFeatureValue;}

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
    std::string _ss;
};

int main()
{
    CAM cam(&std::cout);

    cam("STARTEXP");

    int a = cam["A"];

//    cam["A"] = 22;

    long aa = cam["A"];

    std::string ss = cam["S"];

    cam.logToFile(CAM::api, "ZZZZZ: SSSSS\n");


////    double f = cam["A"];

//    int b;
//    b = cam["A"];

//    cout << "a = " << a << "\n";
    cout << "aa = " << aa << "\n";
    cout << "ss = " << ss << "\n";

////    cout << "f = " << f << "\n";
}
