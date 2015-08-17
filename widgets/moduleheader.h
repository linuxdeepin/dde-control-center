#ifndef MODULEHEADER_H
#define MODULEHEADER_H

#include <libdui/dbaseline.h>

DUI_BEGIN_NAMESPACE
class DTextButton;
DUI_END_NAMESPACE

DUI_USE_NAMESPACE

class QLabel;
class ModuleHeader : public DBaseLine
{
    Q_OBJECT
public:
    ModuleHeader(QString title, bool addResetButton = true, QWidget * parent = 0);

signals:
    void resetButtonClicked();

private:
    QLabel * m_title;
    DTextButton * m_reset;
};

#endif // MODULEHEADER_H
