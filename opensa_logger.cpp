#include <cstdio>
#include <cstring>

#include <opensa_logger.h>

constexpr const char* const LOGD_TAG = "OpenSA";

static OpenSA::LOG_Options gs_default_log;

namespace OpenSA {
    OpenSA_Logger::OpenSA_Logger() noexcept {
        gs_default_log.m_TAG = LOGD_TAG;
        /* Copy and initialize the default logger options structure data */
        /* m_log_options = std::make_unique<LOG_Options>(gs_default_log); */
        m_log_options = gs_default_log;
    }

    void OpenSA_Logger::Android_Release(const LOG_Release_Info* release_info) {
        if (m_log_options.m_use_logcat) {
            __android_log_write(release_info->m_prior, release_info->m_TAG_name, 
                release_info->m_output_buffer);
        }
    }

    ssize_t OpenSA_Logger::Android_Produce(LOG_Release_Info* produce_info) {
        switch(produce_info->m_prior) {
        default:
        case ANDROID_LOG_UNKNOWN:
        case ANDROID_LOG_DEFAULT:
        case ANDROID_LOG_VERBOSE: produce_info->m_status_str = "Success"; break;
        case ANDROID_LOG_DEBUG:
        case ANDROID_LOG_WARN:
        case ANDROID_LOG_ERROR: produce_info->m_status_str = "Error"; break;
        case ANDROID_LOG_FATAL:
        case ANDROID_LOG_SILENT:
        case ANDROID_LOG_INFO: produce_info->m_status_str = "Info"; break;
        }

        uintptr_t buffer_ptr_location = 0, remain_buffer_sz = 0;
        char* const base_buffer_ptr = produce_info->m_output_buffer;

        const LOG_Location* location = &produce_info->log_local;

        #define OUTPUT_LOCATION\
            (base_buffer_ptr + buffer_ptr_location)
        
        #define REMAIN_SIZE\
            (FORMAT_OUTPUT_SZ - buffer_ptr_location)
        
        #define INC_BUFFER(size)\
            (buffer_ptr_location += size)
        
        #define EXPAND_BUFFER(message, ...)\
            INC_BUFFER(snprintf(OUTPUT_LOCATION, REMAIN_SIZE, message, ##__VA_ARGS__))

        /* (TAG FILE:LINE) <Status> -> MESSAGE */
        
        if (m_log_options.m_dsp_TAG) {
            EXPAND_BUFFER("(%s ", m_log_options.m_TAG);
        }
        if (m_log_options.m_file_status) {
            EXPAND_BUFFER("%s:%d) ", location->m_filename, location->m_line); 
        } else { 
            EXPAND_BUFFER(") "); 
        }

        if (m_log_options.m_dsp_status) {
            EXPAND_BUFFER("<%s> ", produce_info->m_status_str);
        }

        EXPAND_BUFFER("%s", produce_info->m_format_buffer);
        return static_cast<ssize_t>(buffer_ptr_location);
    }

    ssize_t OpenSA_Logger::Android_Launch(const LOG_Launch_Data* launch_data, const char* format, ...) {
        va_list variable_arguments;

        va_start(variable_arguments, format);

        LOG_Release_Info stack_based_re = {
            .m_TAG_name = m_log_options.m_TAG,
            .m_prior = launch_data->m_priority
        };

        memcpy(&stack_based_re.log_local, launch_data->m_location, sizeof(LOG_Location));
        vsnprintf(stack_based_re.m_format_buffer, FORMAT_BUFFER_SZ, format, variable_arguments);

        const auto produce_result = Android_Produce(&stack_based_re);

        Android_Release(&stack_based_re);

        va_end(variable_arguments);

        return produce_result;
    }

};

