/**************************************************************************
  errmsg.h - Class declaration for retrieving and holding error messages.

  Author(s): Juha Ojanpera
  Copyright (c) 1999, 2019 Juha Ojanpera.
  *************************************************************************/

#ifndef ERRORMSG_H_
#define ERRORMSG_H_

/*-- System Headers. --*/
#include <stdlib.h>
#include <string>


class ErrorMsg
{
public:
    /**
     * @param errorMessage Error message
     * @param title Error message title (optional)
     * @param extendedMessage Further information about the error (optional)
     */
    ErrorMsg(char *errorMessage, char *title = NULL, char *extendedMessage = NULL) :
        m_title(title),
        m_errorMessage(errorMessage),
        m_extendedMessage(extendedMessage)
    {}

    ~ErrorMsg() {}

    /*-- Public methods. --*/
    const char *getMessage() const { return m_errorMessage.c_str(); }
    const char *getTitle() const { return m_title.c_str(); }
    const char *getExtendedMessage() const { return m_extendedMessage.c_str(); }

private:
    std::string m_title;
    std::string m_errorMessage;
    std::string m_extendedMessage;
};

#endif /* ERRORMSG_H_ */
