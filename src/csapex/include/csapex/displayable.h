#ifndef DISPLAYABLE_H
#define DISPLAYABLE_H

/// SYSTEM
#include <string>
#include <QWidget>

namespace csapex {

class Box;

class Displayable : public QWidget
{
public:
    enum ErrorLevel {
        EL_ERROR, EL_WARNING
    };

public:
    virtual void setError(bool e, const std::string& msg = "", ErrorLevel level = EL_ERROR);
    bool isError() const;
    ErrorLevel errorLevel() const;

    virtual void setBox(Box* box);
    Box* getBox() const;

protected:
    Displayable();

    virtual void errorEvent(bool error, ErrorLevel level);

protected:
    Box* box_;
    bool error_;
    ErrorLevel level_;
};

}

#endif // DISPLAYABLE_H