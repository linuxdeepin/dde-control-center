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
const QColor lineLightColor = QColor::fromRgbF(0,0,0,0.1);

const QString CLIENT_ID = "388340d186f311eb983b0242ac130002";
const QString REDIRECT_URI = "https://uosvip-pre.uniontech.com/account/unionid/callback/uid-managment";
const QString SCOPES = "{base,user.api:contact,user:contact:read}";

//二维码状态
enum qRStyle {
    NormalScanCode,       //正常扫码
    TimeoutScanCode,      //超时扫码模糊
    AshingScanCode,       //未阅读内容灰化
    RefreshScanCode,      //等待扫码刷新
    SuccessScanCode,      //成功扫码模糊
    QuestionScanCode      //扫码模糊问号
};

//登陆注册以及扫码登录状态
enum uIState {
    ScanLoadWidget,            //扫码登录界面
    LoadSuccessRegistWidget,   //扫码成功已注册
    LoadSuccessUnRegistWidget, //扫码成功未注册
    AccountLoadWidget,         //账号登录
    LoadingWidget,             //扫码登陆中
    ScanLoadSuccessWidget,     //扫码登录成功
    ScanRegisterWidget,        //扫码注册界面
    PhoneRegistWidget,         //手机号注册界面
    PhonePasswordWidget,       //手机密码登录界面
    RegistSuccessWidget,       //注册成功页面
    ScanSuccessedunVerifiedWidget, //扫码成功未验证
    SignUpStateWidget,         //扫码成功已验证
    SignOutWidget,             // 确认退出界面
    InitUi = 99,                    //没有界面
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
