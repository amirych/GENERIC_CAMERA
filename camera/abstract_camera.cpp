#include "abstract_camera.h"

ABS_CAM_MACRO()::AbstractCamera(const std::basic_ostream<LogStreamCharType> *log_stream):
    _logStreamPtr(log_stream)
{
}



ABS_CAM_MACRO(void)::defineFeature(AbstractCamera::AbstractCameraFeature *feature)
{
    _cameraFeatures[feature->name()] = std::unique_ptr<AbstractCamera::AbstractCameraFeature>(feature);
}

ABS_CAM_MACRO(void)::defineCommand(AbstractCamera::CameraCommand *command)
{
    _cameraCommands[command->name()] = std::unique_ptr<AbstractCamera::CameraCommand>(command);
}





AbstractCameraException::AbstractCameraException(const AbstractCameraError error,
                                                 const std::string &context):
    exception(), _error(error), _context(context)
{
}


AbstractCameraException::AbstractCameraException(const AbstractCameraError error, const char *context):
    AbstractCameraException(error, std::string(context))
{
}


AbstractCameraError AbstractCameraException::error() const
{
    return _error;
}


const char* AbstractCameraException::what() const NOEXCEPT_DECL
{
    return _context.c_str();
}
