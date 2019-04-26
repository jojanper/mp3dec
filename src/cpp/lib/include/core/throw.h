/**************************************************************************
  throw.h - Class declaration for reporting errors during execution.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

#ifndef EXCEPTION_THROW_H_
#define EXCEPTION_THROW_H_

/*-- System Headers. --*/
#include <stdlib.h>
#include <string.h>

/*-- Project Headers. --*/
#include "core/defines.h"

class AdvanceExcpt
{
public:
    /*-- Constructor --*/
    // AdvanceExcpt(Error_Msg *ErrMsg) { emsg = ErrMsg; };
    AdvanceExcpt() {}

    /*-- Deconstructor --*/
    ~AdvanceExcpt(void)
    {
#if 0
      if(emsg != NULL)
        delete emsg;
      emsg = NULL;
#endif
    }

#if 0
    inline Error_Msg *GetErrorMSG(void) { return (emsg); }

    void __EXPORT_TYPE __stdcall ShowMessage(char *LastMsgStr = NULL)
    {
      if(GetErrorMSG()->UseAssertMessage())
        MessageBox(GetActiveWindow(), GetErrorMSG()->GetAssertMessage(),
                   GetErrorMSG()->GetTitleErrorMessage(),
                   MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
      else
      {
        MessageBox(GetActiveWindow(), GetErrorMSG()->GetMessage(),
                   GetErrorMSG()->GetTitleErrorMessage(),
                   MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
        if(GetErrorMSG()->HasExtensionMessage())
          MessageBox(GetActiveWindow(), GetErrorMSG()->GetExtendedMessage(),
                     "Additional information about the error",
                     MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
      }

      char buf[1024];
      wsprintf(buf, _T("Message from Error Handler"));
      if(LastMsgStr)
        MessageBox(GetActiveWindow(), LastMsgStr, buf,
                   MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
#if 0
      else
        MessageBox(GetActiveWindow(), "Unable to initialize the decoder.",
                   buf, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
#endif
    }
#endif

private:
    // Error_Msg *emsg;
};

#endif /* EXCEPTION_THROW_H_ */
