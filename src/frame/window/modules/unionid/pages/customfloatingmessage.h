#ifndef CUSTOMFLOATINGMESSAGE_H
#define CUSTOMFLOATINGMESSAGE_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QIcon>

#include <DGuiApplicationHelper>
#include <DLabel>
#include <DPushButton>
#include <DWidget>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

class CustomFloatingMessage : public DWidget
{
    Q_OBJECT

public:
enum MessageType {
    TransientType,  //临时的消息,
    ResidentType    //常驻的消息
};
    explicit CustomFloatingMessage(MessageType type, DWidget *parent = nullptr);
    ~CustomFloatingMessage();
    void setDuration(const int m);
    void setMessage(const QString &error);
    void setIcon(const QString &path);
protected:
    void paintEvent(QPaintEvent *event);

private Q_SLOTS:
    void onThemeTypeChanged(DGuiApplicationHelper::ColorType themeType);
    void onTimer();

Q_SIGNALS:
    void signal_setFlag();

private:
    DGuiApplicationHelper::ColorType m_type;
    DLabel *m_icon;
    DLabel *m_toast;
    QTimer *m_show;
    MessageType m_MessageType;
};

#endif // CUSTOMFLOATINGMESSAGE_H
