
#include <cerrno>
#include <ctime>
#include <clocale>

#include <sys/file.h>

#include "opensa_logger.h"
#include "opensa_objects.h"
#include "Address_Map.h"
#include "game/Game_Hooks.h"

static int32_t logRet = 0;

namespace OpenSA {

    bool OpenSA_Logger::write_logHeader() {

        const auto openDate = std::chrono::system_clock::to_time_t(m_openDate);
        struct tm timeDate[1];
        localtime_r(&openDate, &timeDate[0]);

        const bool isNew = ftell(m_Log_File) == 0;
        constexpr uint16_t DATE_AND_HOUR_BSZ = 0x100;
        char dateAndHour[DATE_AND_HOUR_BSZ];
        strftime(dateAndHour, sizeof dateAndHour, "%c", &timeDate[0]);

        if (isNew) {
            fprintf(m_Log_File, 
                "OPENSA LOG (Client v0.0.4)\n"
                "Full Storage Path: %s (Read/Write Access)\n"
                "Log file created at: %s\n",

                gAddrMap_IO_extstorage_path,
                dateAndHour);
        }

        if (!isNew)
            fprintf(m_Log_File, "Log file reopened to write at: %s\n", dateAndHour);

        fprintf(m_Log_File, "Current libGTASA.so base ptr: %p\n\n", 
            reinterpret_cast<void*>(gLib_GTASA_Native.get_Native_Addr()));
        
        fflush(m_Log_File);

        return true;
    }

    bool OpenSA_Logger::setups_logFile(
        const std::string_view fullPath, 
        const std::string_view logFilename) {

        char* fullPLLog = nullptr;
        asprintf(&fullPLLog, "%s%s", 
            fullPath.data(), 
            logFilename.data());
        
        if (fullPLLog == nullptr) {
            Android_Error(gMAIN_SA_Logger, logRet, "Can't allocate the desired log file full pathname!\n");
            std::terminate();
        }

        Android_Info(gMAIN_SA_Logger, logRet, "Will open a new Log file in %s\n", fullPLLog);

        m_Log_File = fopen(fullPLLog, "a");

        if (!m_Log_File) {
            Android_Error(gMAIN_SA_Logger, logRet, "Couldn't open the new log file because of %s\n", 
                // https://pubs.opengroup.org/onlinepubs/9699919799/functions/uselocale.html
                strerror_l(errno, uselocale(static_cast<locale_t>(0))));
            
            free(fullPLLog);
            return false;
        }

        flock(fileno(m_Log_File), LOCK_EX);
        m_openDate = std::chrono::system_clock::now();

        free(fullPLLog);
        return write_logHeader();
    }
}
