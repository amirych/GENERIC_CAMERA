#ifndef ABSTRACT_CAMERA_H
#define ABSTRACT_CAMERA_H


#include <type_traits>
#include <functional>
#include <vector>
#include <map>
#include <exception>
#include <string>
#include <memory>
#include <iostream>


#ifdef _MSC_VER
#if (_MSC_VER > 1800)
    #define NOEXCEPT_DECL noexcept
#else
    #define NOEXCEPT_DECL // empty to compile with VS2013
#endif
#else
    #define NOEXCEPT_DECL noexcept
#endif

                /*  AUXIALIRY TEMPLATE RECURSION DECLARATIONS FOR TYPE CHECKING   */

template<typename T, typename... Rest>
struct is_any : std::false_type {};

template<typename T, typename First>
struct is_any<T, First> : std::is_same<T, First> {};

template<typename T, typename First, typename... Rest>
struct is_any<T, First, Rest...>
    : std::integral_constant<bool, std::is_same<T, First>::value || is_any<T, Rest...>::value>
{};



                            /************************************************
                            *                                               *
                            *        AbstractCamera CLASS DECLARATION       *
                            *                                               *
                            ************************************************/


// just forward declaration
class AbstractCameraException;


enum AbstractCameraError {Error_UnknownFeature, Error_FeatureIsReadOnly, Error_FeatureIsWriteOnly,
                          Error_FeatureGetterIsNULL, Error_FeatureSetterIsNULL, Error_FeatureRangeGetterIsNULL,
                          Error_UnknownCommand, Error_CommandExecFuncIsNULL,
                          Error_UninitializedCameraState};



template<typename FeatureTypeID,
         typename FeatureNameType,
         typename FeatureProxyType,
         typename CommandNameType,
         typename LogStreamCharType,
         typename ... FeatureTypes>
class AbstractCamera
{
    friend FeatureProxyType;
public:
    enum FeatureAccessType {ReadWrite_AccessType, ReadOnly_AccessType, WriteOnly_AccessType};

    AbstractCamera(std::basic_ostream<LogStreamCharType> *log_stream = nullptr):
        _logStreamPtr(log_stream), _cameraFeatureToAccess(nullptr),
        _cameraFeatureValue(this)
    {
    }

    virtual ~AbstractCamera() {}

    virtual bool isInitialized() = 0;

    void setLogStream(std::basic_ostream<LogStreamCharType> *log_stream = nullptr) {
        _logStreamPtr = log_stream;
    }

    template<typename ... T>
    void logToFile(T ... args) {
        if ( !_logStreamPtr ) return;
        logHelper(args ...);
    }

protected:
                /*  BASE CLASS DECLARATION FOR CAMERA FEATURES  */

    class AbstractCameraFeature {
    public:
        AbstractCameraFeature(const FeatureNameType name,
                              const FeatureTypeID type,
                              const FeatureAccessType access_type):
            _name(name), _type(type), _accessType(access_type)
        {
        }

        virtual ~AbstractCameraFeature(){}

        FeatureNameType name() const {
            return _name;
        }
        FeatureTypeID type() const {
            return _type;
        }
        FeatureAccessType accessType() const {
            return _accessType;
        }

    protected:
        FeatureNameType _name;
        FeatureTypeID _type;
        FeatureAccessType _accessType;
    };

                            /*  TYPEDEF TO HOLD FEATURE OBJECTS  */

    typedef std::map<FeatureNameType,std::unique_ptr<AbstractCameraFeature>> camera_feature_map_t;

                /*  WORKING TEMPLATE CLASS DECLARATION FOR CAMERA FEATURES  */

    template<typename T, typename = typename std::enable_if< is_any<T,FeatureTypes ...>::value >::type>
    class CameraFeature: public AbstractCameraFeature {
    public:
        CameraFeature(const FeatureNameType name,
                      const FeatureTypeID type,
                      const FeatureAccessType access_type,
                      const std::vector<T> &range,
                      const std::function<T()> getter = nullptr,
                      const std::function<void(const T)> setter = nullptr,
                      const std::function<std::vector<T>()> range_getter = nullptr):
            AbstractCameraFeature(name,type,access_type), _range(range),
            _getter(getter), _setter(setter), _range_getter(range_getter)
        {
        }

        virtual ~CameraFeature(){}


        T get() {
            if ( this->_accessType == WriteOnly_AccessType ) {
                throw AbstractCameraException(Error_FeatureIsWriteOnly, "Feature is write only");
            }
            if ( _getter ) return _getter();
            throw AbstractCameraException(Error_FeatureGetterIsNULL, "Feature getter is NULL");
        }

        void set(const T val) {
            if ( this->_accessType == ReadOnly_AccessType ) {
                throw AbstractCameraException(Error_FeatureIsReadOnly, "Feature is read only");
            }
            if ( _setter ) _setter(val);
            else throw AbstractCameraException(Error_FeatureSetterIsNULL, "Feature setter is NULL");
        }

        std::vector<T> range() {
            if ( _range_getter ) {  // get range and return it
                _range = _range_getter();
                return _range;
            } else {                // return 'static' range
                return _range;
            }
        }

        void set_range(std::vector<T> range) {
            _range = range;
        }

        void set_range() {
            if ( _range_getter ) _range = _range_getter();
            else throw AbstractCameraException(Error_FeatureRangeGetterIsNULL,
                                               "Feature range getter is NULL");
        }

    protected:
        std::vector<T> _range;
        std::function<T()> _getter;
        std::function<void(const T)> _setter;
        std::function<std::vector<T>()> _range_getter;
    };


                    /*  CameraCommand CLASS DECLARATION  */

    class CameraCommand {
    public:
        CameraCommand(const CommandNameType name,
                      const std::function<void()> exec_func):
            _name(name), _exec_func(exec_func)
        {
        }

        CommandNameType name() const {
            return _name;
        }

        void exec() {
            if ( _exec_func ) {
                _exec_func();
            } else {
                throw AbstractCameraException(Error_CommandExecFuncIsNULL, "Command exec function is NULL");
            }
        }

    protected:
        CommandNameType _name;
        std::function<void()> _exec_func;
    };


                        /*  TYPEDEF TO HOLD COMMAND OBJECTS  */

    typedef std::map<CommandNameType,std::unique_ptr<CameraCommand>> camera_command_map_t;


public:     /*  PUBLIC MEMBERS AND METHODS OF AbstractCamera CLASS (continue) */

    // operator to execute camera command

    void operator()(const CommandNameType cmd_name) {
        if ( !isInitialized() ) {
            throw AbstractCameraException(Error_UninitializedCameraState,
                                          "Try to execute comand for uninitialized camera");
        }

        auto search_result = _cameraCommands.find(cmd_name);

        if ( search_result == _cameraCommands.end() ) {
            throw AbstractCameraException(Error_UnknownCommand, "Unknown command name");
        }

        search_result->second.get()->exec();
    }

    // operator to access camera features

    FeatureProxyType & operator[](const FeatureNameType feature_name) {
        if ( !isInitialized() ) {
            throw AbstractCameraException(Error_UninitializedCameraState,
                                          "Try to access feature of uninitialized camera");
        }

        auto search_result = _cameraFeatures.find(feature_name);

        if ( search_result == _cameraFeatures.end() ) {
            throw AbstractCameraException(Error_UnknownFeature, "Unknown feature name");
        }

        _cameraFeatureToAccess = search_result->second.get();

        return _cameraFeatureValue;
    }



protected:  /*  PROTECTED MEMBERS AND METHODS OF AbstractCamera CLASS */

    std::basic_ostream<LogStreamCharType>* _logStreamPtr;


        /*  features control members  */

    AbstractCameraFeature* _cameraFeatureToAccess;
    FeatureProxyType _cameraFeatureValue;

    camera_feature_map_t _cameraFeatures;
    camera_command_map_t _cameraCommands;

    void defineFeature(AbstractCameraFeature *feature) {
        _cameraFeatures[feature->name()] = std::unique_ptr<AbstractCamera::AbstractCameraFeature>(feature);
    }

    void defineCommand(CameraCommand *command) {
        _cameraCommands[command->name()] = std::unique_ptr<AbstractCamera::CameraCommand>(command);
    }


        /*  helper methods for logging facility  */

    template<typename T1, typename ... T2>
    void logHelper(T1 first, T2 ... rest) {
        logHelper(first);
        logHelper(rest ...);
    }

    template<typename T>
    void logHelper(T arg) {
        *_logStreamPtr << arg << std::flush;
    }

    void logHelper(){}

};


                        /*********************************************************
                        *                                                        *
                        *        AbstractCameraException CLASS DECLARATION       *
                        *                                                        *
                        *********************************************************/

class AbstractCameraException: public std::exception
{
public:
    AbstractCameraException(const AbstractCameraError error,
                            const std::string &context):
        exception(), _error(error), _context(context)
    {
    }

    AbstractCameraException(const AbstractCameraError error,
                            const char *context):
        AbstractCameraException(error, std::string(context))
    {
    }

    AbstractCameraError error() const {
        return _error;
    }

    const char* what() const NOEXCEPT_DECL {
        return _context.c_str();
    }

protected:
    AbstractCameraError _error;
    std::string _context;
};


#endif // ABSTRACT_CAMERA_H
