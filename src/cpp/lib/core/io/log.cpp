/**************************************************************************
  log.cpp - Logfile implementations.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

/*-- System Headers. --*/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-- Project Headers. --*/
#include "core/io/log.h"
#include "core/throw.h"

/*
   Purpose:     Maximum length of a log message.
   Explanation: - */
const int MAXLOGLINELEN = 2048;


LogFile::LogFile() : m_fpLog(stdout), m_szLogFile(NULL), m_openCount(0) {}

LogFile::LogFile(const char *m_szLogFileArg) :
    m_fpLog(NULL),
    m_szLogFile(strdup(m_szLogFileArg)),
    m_openCount(0)
{}

LogFile::~LogFile()
{
    this->close();

    if (m_szLogFile)
        free(m_szLogFile);
}


bool
LogFile::open()
{
    m_openCount++;

    if (m_fpLog)
        return true;

    m_fpLog = fopen(m_szLogFile, "a");
    if (m_fpLog == NULL) {
        char buf[256];

        sprintf(buf, "Unable to open logfile %s", m_szLogFile);
        throw new AdvanceExcpt(new ErrorMsg(buf));
    }

    return (m_fpLog != NULL);
}

bool
LogFile::close()
{
    if (m_fpLog == NULL)
        return false;

    if (m_szLogFile)
        fclose(m_fpLog);
    m_fpLog = NULL;

    return true;
}

void
LogFile::write(const char *format, ...)
{
    va_list argptr;

    if (m_fpLog == NULL)
        return;

    char *szBuffer = new char[MAXLOGLINELEN];

    va_start(argptr, format);
    vsprintf(szBuffer, format, argptr);
    va_end(argptr);

    fprintf(m_fpLog, "%s", szBuffer);
    fflush(m_fpLog);

    delete szBuffer;
}
