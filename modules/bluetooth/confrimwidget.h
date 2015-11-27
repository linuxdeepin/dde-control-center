#ifndef CONFRIMWIDGET_H
#define CONFRIMWIDGET_H

#include <QWidget>

#include <libdui/dtextbutton.h>

DUI_USE_NAMESPACE

class ConfrimWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConfrimWidget(QWidget *parent = 0);

    void setDisconnectVisible(bool visible);

signals:
    void ignore();
    void disconnect();

private:
    DTextButton *m_disconnect;
    DTextButton *m_ignore;
};

#endif // CONFRIMWIDGET_H
