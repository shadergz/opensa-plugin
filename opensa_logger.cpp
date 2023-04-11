#include <cstdio>
#include <cstring>

#include <sys/file.h>

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

    OpenSA_Logger::~OpenSA_Logger() {
        const auto finalTime = std::chrono::system_clock::now();
        const auto timeTData = std::chrono::system_clock::to_time_t(finalTime);

        if (m_Log_File != nullptr) {
            fprintf(m_Log_File, 
                "OpenSA has been closed!\n"
                "Log file end time: %s\n",
                
                std::ctime(&timeTData));

            // Now any other process can read all the logs!
            fflush(m_Log_File);
            flock(fileno(m_Log_File), LOCK_UN);

            fclose(m_Log_File);
        }
    }

    void OpenSA_Logger::Android_Release(LOG_Release_Info* release_info) {
        if (m_Log_File != nullptr) {
            const char* cMessage = strchr(release_info->mOutput_Buffer, ')');
            if (!cMessage) return;
            else cMessage++;

            fputs(cMessage, m_Log_File);
            fflush(m_Log_File);
        }

        if (mLog_Options.mUse_Logcat) {
            char* unusedTag = strchr(release_info->mOutput_Buffer,  '=') - 1;
            char* logMessage = strchr(release_info->mOutput_Buffer, '>') + 1;

            memmove(unusedTag, logMessage, strlen(logMessage) + 1);
            __android_log_write(release_info->mPriority_Event, release_info->mTAG_Name, 
                release_info->mOutput_Buffer);
        }

    }

    ssize_t OpenSA_Logger::Android_Produce(LOG_Release_Info* produce_info) {
        switch(produce_info->mPriority_Event) {
        default:
        case ANDROID_LOG_UNKNOWN:
        case ANDROID_LOG_DEFAULT:
        case ANDROID_LOG_VERBOSE: 
            produce_info->mStatus_Str = "Success"; break;
        case ANDROID_LOG_DEBUG:
        case ANDROID_LOG_WARN:
        case ANDROID_LOG_ERROR:
        case ANDROID_LOG_FATAL:
            produce_info->mStatus_Str = "Error";   break;
        case ANDROID_LOG_SILENT:
        case ANDROID_LOG_INFO: 
            produce_info->mStatus_Str = "Info";    break;
        }

        uint_fast64_t bfPointer = 0, remain_buffer_sz = 0;
        char* const base_buffer_ptr = produce_info->mOutput_Buffer;

        const LOG_Location* location = &produce_info->mLog_Location;

        #define OUTPUT_LOCATION\
            (base_buffer_ptr + bfPointer)
        
        #define REMAIN_SIZE\
            (FORMAT_OUTPUT_SZ - bfPointer)
        
        #define INC_BUFFER(size)\
            (bfPointer += size)
        
        #define EXPAND_BUFFER(message, ...)\
            INC_BUFFER(snprintf(OUTPUT_LOCATION, REMAIN_SIZE, message, ##__VA_ARGS__))

        /* (Tag File:Line) =Status 99999= > Message */
        if (mLog_Options.mDsp_TAG)
            EXPAND_BUFFER("(%s ", mLog_Options.mTAG);
        
        if (mLog_Options.mFile_Status)
            EXPAND_BUFFER("%s:%d) ", location->mLocal_Filename, location->mLocal_Line); 
        else
            EXPAND_BUFFER(") ");

        if (mLog_Options.mDsp_Status) {
            // 0 -> 99999
            static uint msgCount = 0;
            EXPAND_BUFFER("=%s %5d= > ", produce_info->mStatus_Str, msgCount);
            if (msgCount++ >= 99999)
                msgCount = 0;
        }

        EXPAND_BUFFER("%s", produce_info->mFormat_Buffer);
        return static_cast<ssize_t>(bfPointer);
    }

    ssize_t OpenSA_Logger::Android_Launch(const LOG_Launch_Data* launch_data, const char* format, ...) {
        va_list variable_arguments;

        va_start(variable_arguments, format);

        LOG_Release_Info loginfo = {
            .mTAG_Name = mLog_Options.mTAG,
            .mPriority_Event = launch_data->mPriority
        };

        memcpy(&loginfo.mLog_Location, launch_data->mLocation, sizeof(LOG_Location));
        vsnprintf(loginfo.mFormat_Buffer, FORMAT_BUFFER_SZ, format, variable_arguments);

        const auto produce_result = Android_Produce(&loginfo);
        Android_Release(&loginfo);
        va_end(variable_arguments);
        return produce_result;
    }

};

