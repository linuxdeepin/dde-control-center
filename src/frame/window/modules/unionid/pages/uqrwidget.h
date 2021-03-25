#ifndef UQRFRAME_H
#define UQRFRAME_H
#include "QrCode.h"
#include "QImage"
#include <string>
#include <vector>
#include <QTimer>

// dtk
#include <DFrame>
#include <DLabel>
#include <DIconButton>

// qt
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGraphicsEffect>

DWIDGET_USE_NAMESPACE
using namespace qrcodegen;

//二维码状态
enum qRStyle {
    NormalScanCode,       //正常扫码
    TimeoutScanCode,      //超时扫码模糊
    RefreshScanCode,      //等待扫码刷新
    SuccessScanCode       //成功扫码模糊
};

class RequestService;
class UQrFrame : public DFrame
{
    Q_OBJECT
public:
    UQrFrame(QWidget *parent = nullptr);
    // 切换窗口类型
    void setWidgetType(qRStyle type);
    // 二维码图片显示添加
    void showQRcodePicture(QString url);
    // 刷新按钮不可用
    DIconButton * getFreshButton();
    // 设置阴影
    void setOpacity(qreal opacity);

Q_SIGNALS:
    void refreshsignal();

protected:
    void paintEvent(QPaintEvent *event);

private:
    //信号槽初始化
    void initConnect();
    void initUI();
    void initUIType(qRStyle tppe);
    // 正常显示
    void initUINormal();
    // 超时模糊
    void initUITimeout();
    // 等待刷新
    void initUIRefresh();
    // 扫码成功
    void initUISuccesss();
    // 二维码图片
    DLabel *m_qrLabel = nullptr;
    // 边框布局
    QWidget *m_borderwidget = nullptr;
    // 刷新按钮
    DIconButton *m_refreshbutton = nullptr;
    // 整体网格布局
    QGridLayout *m_mainLayout = nullptr;
    // 窗口类型
    qRStyle m_type;
    QGraphicsOpacityEffect *opacityEffect = nullptr;
    RequestService *requestQrCode =nullptr;

};

#endif // UQRFRAME_H
