#ifndef OPENSA_LOGGER_H
#define OPENSA_LOGGER_H

#include <android/log.h>

/* #include <memory> */
#include <cstring>
#include <cstdarg>
#include <cstdint>

namespace Client_Log {
    constexpr short FORMAT_BUFFER_SZ = 0x6f;
    constexpr short FORMAT_OUTPUT_SZ = FORMAT_BUFFER_SZ * 2;

    struct LOG_Location {
    public:
        const char* m_filename;
        uint32_t m_line;
    };

    struct LOG_Release_Info {
    public:
        const char* m_TAG_name;
        const char* m_status_str;

        const android_LogPriority m_prior;
        LOG_Location log_local;

        char m_format_buffer[FORMAT_BUFFER_SZ];
        char m_output_buffer[FORMAT_OUTPUT_SZ];
    };

    struct LOG_Launch_Data {
    public:
        const android_LogPriority m_priority;
        const LOG_Location* m_location;
    };

    struct LOG_Options {
    public:
        LOG_Options() {}
        LOG_Options(const LOG_Options& copy_options) {
            memcpy(this, &copy_options, sizeof(copy_options));
        }
        bool m_dsp_TAG = true;
        bool m_dsp_status = true;
        /* Display information like the file and line where the log originate from */
        bool m_file_status = true;

        const char* m_TAG = nullptr;
        /* -> When enable by default, the log system will display the message into the 
         * Android logcat message system, otherwise, only the file descriptor will be used into
         * log release operations <-
        */
        bool m_use_logcat = true;
        int m_opened_FD = -1;
    };

    class OpenSA_Logger {
    public:
        /*
        OpenSA_Logger(LOG_Options* log_options);
        OpenSA_Logger(int opened_fd, const char* log_filename = nullptr);
        */
        OpenSA_Logger() noexcept;
        ~OpenSA_Logger() = default;

        #define Android_Info(logger, format, ...)\
            do \
            {\
                Client_Log::LOG_Location __actual_location {\
                    .m_filename = strrchr(__FILE__, '/') + 1,\
                    .m_line = __LINE__\
                };\
                const Client_Log::LOG_Launch_Data __current_log_launch {\
                    .m_priority = ANDROID_LOG_INFO,\
                    .m_location = &__actual_location\
                };\
                logger.Android_Launch(&__current_log_launch, format, ##__VA_ARGS__);\
            }\
            while(0)

        ssize_t Android_Launch(const LOG_Launch_Data* launch_data, const char* format, ...);
        void configure_user_output(int opened_fd, const char* opt_filename = nullptr);

    private:
        ssize_t Android_Produce(LOG_Release_Info* produce_info);
        void Android_Release(const LOG_Release_Info* release_info);
        /* std::unique_ptr<LOG_Options> m_log_options; */
        LOG_Options m_log_options;
    };

};

#endif

