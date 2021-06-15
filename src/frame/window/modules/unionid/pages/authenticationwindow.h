#ifndef AUTHENTICATIONWINDOW_H
#define AUTHENTICATIONWINDOW_H
#include <DAbstractDialog>
#include <QLineEdit>
#include <QLabel>
#include <DSuggestButton>

DWIDGET_USE_NAMESPACE

class AuthenticationWindow : public DAbstractDialog
{
    Q_OBJECT
public:
    static AuthenticationWindow* instance();

    void setData(QString phoneNumber, QString weChatUnionId, QString accessToken,
                 QString userAvatar, QString nickName);

Q_SIGNALS:
    void toTellrefreshUserInfo();

private Q_SLOTS:
    void onGetCodeButtonClicked();

    void onSendSmsCodeResult();

    void onLineEditTextChanged(QString text);

    void onNextButtonClicked();

    void onVerifySmsCodeResult();

    void onTimeOut();

    void onThemeTypeChanged();

    void onClose();

private:
    AuthenticationWindow(QWidget *parent = nullptr);

    QLabel *m_tipLabel;
    QLabel *m_warningLabel;
    QString m_phoneNumber;
    QString m_accessToken;
    QString m_weChatUnionId;
    QString m_userAvatar;
    QString m_nickName;
    DSuggestButton *m_getCodeButton;
    DSuggestButton *m_nextButton;
    QLineEdit *m_lineEdit;
    QTimer *m_timer;
    int m_second;
};

#endif // AUTHENTICATIONWINDOW_H
