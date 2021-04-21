#ifndef CUSTOMFLOATINGMESSAGE_H
#define CUSTOMFLOATINGMESSAGE_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QIcon>

#include <DGuiApplicationHelper>
#include <DLabel>
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
    void setDuration(const int m);
    void setMessage(const QString &error);
    void setIcon(const QString &path);
    ~ CustomFloatingMessage();
protected:
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);

private Q_SLOTS:
    void onThemeTypeChanged(DGuiApplicationHelper::ColorType themeType);
    void onTimer();

private:
    DGuiApplicationHelper::ColorType m_type;
    DLabel *m_icon;
    DLabel *m_toast;
    QTimer *m_show = nullptr;
    MessageType m_MessageType;
};
#endif // CUSTOMFLOATINGMESSAGE_H
