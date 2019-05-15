/**************************************************************************
  log.h - Class declaration for log messages.

  Author(s): Juha Ojanpera
  Copyright (c) 1999, 2019 Juha Ojanpera.
  *************************************************************************/

#ifndef LOG_H_
#define LOG_H_

/*-- System Headers. --*/
#include <stdio.h>

class LogFile
{
public:
    LogFile();
    LogFile(const char *szLogFileArg);
    ~LogFile();

    // Open logger for writing
    bool open();

    // Close logger
    bool close();

    // Writes messages to logfile
    void write(const char *format, ...);

    // Number of entities using the logger
    inline int getOpenCount() const { return m_openCount; }

private:
    FILE *m_fpLog;
    char *m_szLogFile;
    int m_openCount;
};

#endif /* LOG_H_ */
