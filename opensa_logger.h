#ifndef OPENSA_LOGGER_H
#define OPENSA_LOGGER_H

#include <android/log.h>

#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <cstdint>

#include <chrono>
#include <string_view>

namespace OpenSA {
    constexpr short FORMAT_BUFFER_SZ = 0x13f;
    constexpr short FORMAT_OUTPUT_SZ = FORMAT_BUFFER_SZ * 2;

    struct LOG_Location {
    public:
        const char* mLocal_Filename;
        uint32_t mLocal_Line;
    };

    struct LOG_Release_Info {
    public:
        const char* mTAG_Name;
        const char* mStatus_Str;

        const android_LogPriority mPriority_Event;
        LOG_Location mLog_Location;

        char mFormat_Buffer[FORMAT_BUFFER_SZ];
        char mOutput_Buffer[FORMAT_OUTPUT_SZ];
    };

    struct LOG_Launch_Data {
    public:
        const android_LogPriority mPriority;
        const LOG_Location* mLocation;
    };

    struct LOG_Options {
    public:
        LOG_Options() {}
        LOG_Options(const LOG_Options& copy_options) {
            memcpy(this, &copy_options, sizeof(copy_options));
        }
        bool mDsp_TAG = true;
        bool mDsp_Status = true;
        /* Display information like the file and line where the log originate from */
        bool mFile_Status = true;

        const char* mTAG = nullptr;
        /* -> When enable by default, the log system will display the message into the 
         * Android logcat message system, otherwise, only the file descriptor will be used into
         * log release operations <-
        */
        bool mUse_Logcat = true;
    };

    class OpenSA_Logger {
    public:
        /*
        OpenSA_Logger(LOG_Options* log_options);
        OpenSA_Logger(int opened_fd, const char* log_filename = nullptr);
        */
        OpenSA_Logger() noexcept;
        ~OpenSA_Logger();

        #define Android_Logger_DO(logger, status, format, ...)\
            do \
            {\
                OpenSA::LOG_Location __actual_location {\
                    .mLocal_Filename = strrchr(__FILE__, '/') + 1,\
                    .mLocal_Line = __LINE__\
                };\
                const OpenSA::LOG_Launch_Data __current_log_launch {\
                    .mPriority = status,\
                    .mLocation = &__actual_location\
                };\
                logger.android_Launch(&__current_log_launch, format, ##__VA_ARGS__);\
            }\
            while(0)

        #define Android_Success(logger, format, ...)\
            Android_Logger_DO(logger, ANDROID_LOG_VERBOSE, format, ##__VA_ARGS__)
        #define Android_Info(logger, format, ...)\
            Android_Logger_DO(logger, ANDROID_LOG_INFO, format, ##__VA_ARGS__)
        #define Android_Error(logger, format, ...)\
            Android_Logger_DO(logger, ANDROID_LOG_ERROR, format, ##__VA_ARGS__)
        
        ssize_t android_Launch(const LOG_Launch_Data* launch_data, const char* format, ...);
        bool setups_logFile(const std::string_view fullPath, const std::string_view logFilename);

    private:
        ssize_t android_Produce(LOG_Release_Info* produce_info);
        void android_Release(LOG_Release_Info* release_info);

        bool write_logHeader();
        /* std::unique_ptr<LOG_Options> mLog_Options; */
        LOG_Options mLog_Options;

        FILE* mLogFile = {};
        std::chrono::time_point<std::chrono::system_clock> mOpenDate = {};
    };

};

#endif

