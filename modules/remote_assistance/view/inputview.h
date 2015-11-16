#ifndef INPUTVIEW_H
#define INPUTVIEW_H

#include <QString>
#include "abstractview.h"
#include <libdui/libdui_global.h>

class QLineEdit;
class QLabel;
class QRegExpValidator;

DUI_BEGIN_NAMESPACE
class DTextButton;
DUI_END_NAMESPACE

class InputView : public AbstractView
{
    Q_OBJECT
public:
    InputView(QWidget* p=nullptr);
    void focus();

signals:
    void cancel();
    void connect(QString);

private slots:
    void emitConnect();

private:
    QWidget* createMainWidget() Q_DECL_OVERRIDE;
    QLineEdit* m_tokenEdit;
    QLabel* m_tip;
    QRegExpValidator* m_validator;
    DUI_NAMESPACE::DTextButton* m_connectButton;
};

#endif // INPUTVIEW_H
