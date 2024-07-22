
#include <cerrno>
#include <ctime>
#include <clocale>

#include <sys/file.h>

#include "opensa_logger.h"
#include "opensa_objects.h"
#include "address_map.h"
#include "game/game_hooks.h"

namespace OpenSA {

    bool OpenSA_Logger::write_logHeader() {

        const auto openDate = std::chrono::system_clock::to_time_t(mOpenDate);
        struct tm timeDate[1] = {};
        localtime_r(&openDate, &timeDate[0]);

        const bool isNew = ftell(mLogFile) == 0;
        constexpr uint16_t DATE_AND_HOUR_BSZ = 0x100;
        char dateAndHour[DATE_AND_HOUR_BSZ];
        strftime(dateAndHour, sizeof dateAndHour, "%c", &timeDate[0]);

        if (isNew) {
            fprintf(mLogFile, 
                "OPENSA LOG (CLIENT V0.0.4)\n"
                "Full Storage Path: [%s] (Read/Write Access)\n"
                "Log file created at: %s\n",

                Address_IO_A64::gExtStorage_path,
                dateAndHour);
        }

        if (!isNew)
            fprintf(mLogFile, "Log file reopened to write at: %s\n", dateAndHour);

        fprintf(mLogFile, "Current libGTASA.so base ptr: %p\n", 
            reinterpret_cast<void*>(gGTASA_SO.get_Native_Addr()));
        
        fflush(mLogFile);

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
            Android_Error(gSA_logger, "Can't allocate the desired log file full pathname!\n");
            std::terminate();
        }

        Android_Info(gSA_logger, "Will open a new Log file in %s\n", fullPLLog);

        mLogFile = fopen(fullPLLog, "a");

        if (!mLogFile) {
            Android_Error(gSA_logger, "Couldn't open the new log file because of %s\n", 
                // https://pubs.opengroup.org/onlinepubs/9699919799/functions/uselocale.html
                strerror_l(errno, uselocale(static_cast<locale_t>(0))));
            
            free(fullPLLog);
            return false;
        }

        flock(fileno(mLogFile), LOCK_EX);
        mOpenDate = std::chrono::system_clock::now();

        free(fullPLLog);
        const auto bWrote = write_logHeader();

        return mLogFile && bWrote;
    }
}
