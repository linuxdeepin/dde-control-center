#ifndef TIMEZONECTRLWIDGET_H
#define TIMEZONECTRLWIDGET_H

#include <QWidget>

#include "imagenamebutton.h"
#include "dynamiclabel.h"

#include <libdui/dtextbutton.h>

DUI_USE_NAMESPACE

class TimezoneCtrlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimezoneCtrlWidget(QWidget *parent = 0);

signals:
    void removeTimezone();
    void removeAccept();
    void addTimezone();
    void addTimezoneAccept();
    void addTimezoneCancel();

public slots:
    void toNormalMode();
    void toDelTimezoneMode();
    void toAddTimezoneMode();
    void setAcceptOrCancel(bool accept);

private:
    DynamicLabel *m_TipsLabel;
    ImageNameButton *m_addBtn;
    ImageNameButton *m_delBtn;
    DTextButton *m_cancelBtn;
    DTextButton *m_confirmBtn;
    DTextButton *m_addTimezoneBtn;
};

#endif // TIMEZONECTRLWIDGET_H
