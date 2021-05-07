#ifndef DEFIND_H
#define DEFIND_H

#include <QColor>

const QColor textTitleLightColor = QColor::fromRgbF(0,0,0,0.85);
const QColor textTipLightColor = QColor::fromRgbF(0,0,0,0.45);
const QColor textTitleDarkColor = QColor(192,198,212);
const QColor textTiptDarkColor = QColor(121,129,144);
const QColor buttonNormalBorderLightColor = QColor::fromRgbF(0,0,0,0.15);
const QColor buttonHoverBorderLightColor = QColor::fromRgbF(0,0,0,0.25);
const QColor buttonPressBorderLightColor = QColor::fromRgbF(0,0,0,0.15);
const QColor buttonNormalBorderDarkColor = QColor::fromRgbF(1,1,1,0.15);
const QColor buttonHoverBorderDarkColor = QColor::fromRgbF(1,1,1,0.25);
const QColor buttonPressBorderDarkColor = QColor::fromRgbF(1,1,1,0.15);
const QColor lineLightColor = QColor::fromRgbF(0,0,0,0.1);

const QString SCOPES = "{base,user.api:contact,user:contact:read}";


//默认头像
const QString AvaterPath = ":/themes/light/icons/avatar_24px.png";

//二维码状态
enum qRStyle {
    NormalScanCode,       //正常扫码
    TimeoutScanCode,      //超时扫码模糊
    AshingScanCode,       //未阅读内容灰化
    RefreshScanCode,      //等待扫码刷新
    SuccessScanCode,      //成功扫码模糊
    QuestionScanCode,     //扫码模糊问号
    BlankScanCode         //加载状态
};

enum ScanStatus {
    Waitscancode,       //待扫码
    BeginDeal,          //开始处理
    Bindings,           //已绑定
    UnBindings,         //未绑定
    Registered,         //已注册
    UnRegistered,       //未注册
    CancelRegistered,   //取消注册
    Login,              //已登录
    CancelLogin,        //取消登录
    Activate,           //已激活
    UnActivate,         //未激活
    CancelActivate,     //取消激活
    ScanComplete,       //完成扫码
    ScanCreated,        //扫码完成已创建
    ScanCancelComplete, //扫码完成取消创建
    cancelBindings      //取消绑定
};

#endif // DEFIND_H
