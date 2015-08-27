#ifndef MODULEHEADER_H
#define MODULEHEADER_H

#include <libdui/dbaseline.h>

#include <QLabel>
#include <QTimer>

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
    ~ModuleHeader();

signals:
    void resetButtonClicked();

private slots:
    void resetUI();

private:
    QLabel * m_title;
    QLabel * m_tipsLabel;
    DTextButton * m_reset;
    QTimer * m_timer;
};

#endif // MODULEHEADER_H
