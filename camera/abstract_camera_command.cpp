#include "abstract_camera.h"

ABS_CAM_MACRO()::CameraCommand::CameraCommand(const CommandNameType name,
                                                               const std::function<void()> exec_func):
    _name(name), _exec_func(exec_func)
{

}


ABS_CAM_MACRO(CommandNameType)::CameraCommand::name() const
{
    return _name;
}

ABS_CAM_MACRO(void)::CameraCommand::exec()
{
    if ( _exec_func ) {
        _exec_func();
    } else {
        throw AbstractCameraException(Error_CommandExecFuncIsNULL, "Command exec function is NULL");
    }
}
