/**************************************************************************
  throw.h - Class declaration for reporting errors during execution.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

#ifndef EXCEPTION_THROW_H_
#define EXCEPTION_THROW_H_

/*-- Project Headers. --*/
#include "core/errmsg.h"

class AdvanceExcpt
{
public:
    AdvanceExcpt(ErrorMsg *err) : m_err(err) {}

    ~AdvanceExcpt()
    {
        if (m_err != NULL)
            delete m_err;
        m_err = NULL;
    }

    const ErrorMsg *getError() const { return m_err; }

private:
    ErrorMsg *m_err;
};

#endif /* EXCEPTION_THROW_H_ */
