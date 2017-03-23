#include "camera/abstract_camera.h"


#include <iostream>
#include <string>

using namespace std;

class CAM;
//enum CAM::ft;
//class CAM::FP;
enum ft {IBTTYPE, FLOATTYPE, STRTYPE};
class FP
{
    friend class CAM;
protected:
//    FP() = delete;
//public:
    FP(CAM *cam = nullptr):_cam(cam) {}
private:
    CAM *_cam;
};

typedef AbstractCamera<ft, std::string, std::string, FP, std::string, std::string, char, int, double, std::string> AC;
class CAM: public AC
{
public:
    enum LID {cam, api, bl};

    CAM(ostream* ls): AbstractCamera(ls) {
        defineCommand(new CameraCommand("STARTEXP",
                                        std::bind(static_cast<void(CAM::*)()>
                                                  (&CAM::startExp), this) )
                      );
    }

    void startExp() {}

    bool isInitialized() { return true;}

    void logToFile( const LID lid, const std::string &ls, const int t = 0) {
        *_logStreamPtr << "AAA: ";
        AC::logHelper(ls);
    }

protected:
};

int main()
{
    CAM cam(&std::cout);
}
