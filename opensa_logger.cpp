#include <cstdio>
#include <cstring>

#include "opensa_logger.h"

constexpr const char* const LOGD_TAG = "OpenSA";

static OpenSA::LOG_Options gsDefault_LogConf;

namespace OpenSA {
    OpenSA_Logger::OpenSA_Logger() noexcept {
        gsDefault_LogConf.mTAG = LOGD_TAG;
        /* Copy and initialize the default logger options structure data */
        /* mLog_Options = std::make_unique<LOG_Options>(gsDefault_LogConf); */
        mLog_Options = gsDefault_LogConf;
    }

    void OpenSA_Logger::Android_Release(const LOG_Release_Info* release_info) {
        if (mLog_Options.mUse_Logcat) {
            __android_log_write(release_info->mPriority_Event, release_info->mTAG_Name, 
                release_info->mOutput_Buffer);
        }
    }

    ssize_t OpenSA_Logger::Android_Produce(LOG_Release_Info* produce_info) {
        switch(produce_info->mPriority_Event) {
        default:
        case ANDROID_LOG_UNKNOWN:
        case ANDROID_LOG_DEFAULT:
        case ANDROID_LOG_VERBOSE: produce_info->mStatus_Str = "Success"; break;
        case ANDROID_LOG_DEBUG:
        case ANDROID_LOG_WARN:
        case ANDROID_LOG_ERROR: produce_info->mStatus_Str = "Error"; break;
        case ANDROID_LOG_FATAL:
        case ANDROID_LOG_SILENT:
        case ANDROID_LOG_INFO: produce_info->mStatus_Str = "Info"; break;
        }

        uint_fast64_t buffer_ptr_location = 0, remain_buffer_sz = 0;
        char* const base_buffer_ptr = produce_info->mOutput_Buffer;

        const LOG_Location* location = &produce_info->mLog_Location;

        #define OUTPUT_LOCATION\
            (base_buffer_ptr + buffer_ptr_location)
        
        #define REMAIN_SIZE\
            (FORMAT_OUTPUT_SZ - buffer_ptr_location)
        
        #define INC_BUFFER(size)\
            (buffer_ptr_location += size)
        
        #define EXPAND_BUFFER(message, ...)\
            INC_BUFFER(snprintf(OUTPUT_LOCATION, REMAIN_SIZE, message, ##__VA_ARGS__))

        /* (TAG FILE:LINE) <Status> -> MESSAGE */
        
        if (mLog_Options.mDsp_TAG) {
            EXPAND_BUFFER("(%s ", mLog_Options.mTAG);
        }
        if (mLog_Options.mFile_Status) {
            EXPAND_BUFFER("%s:%d) ", location->mLocal_Filename, location->mLocal_Line); 
        } else { 
            EXPAND_BUFFER(") "); 
        }

        if (mLog_Options.mDsp_Status) {
            EXPAND_BUFFER("<%s> ", produce_info->mStatus_Str);
        }

        EXPAND_BUFFER("%s", produce_info->mFormat_Buffer);
        return static_cast<ssize_t>(buffer_ptr_location);
    }

    ssize_t OpenSA_Logger::Android_Launch(const LOG_Launch_Data* launch_data, const char* format, ...) {
        va_list variable_arguments;

        va_start(variable_arguments, format);

        LOG_Release_Info stack_based_re = {
            .mTAG_Name = mLog_Options.mTAG,
            .mPriority_Event = launch_data->mPriority
        };

        memcpy(&stack_based_re.mLog_Location, launch_data->mLocation, sizeof(LOG_Location));
        vsnprintf(stack_based_re.mFormat_Buffer, FORMAT_BUFFER_SZ, format, variable_arguments);

        const auto produce_result = Android_Produce(&stack_based_re);

        Android_Release(&stack_based_re);

        va_end(variable_arguments);

        return produce_result;
    }

};

