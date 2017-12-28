#ifndef ADDIALOG_H
#define ADDIALOG_H

#include "../usermodel.h"

#include <ddialog.h>
#include <QWidget>
#include <dlineedit.h>
#include <dpasswordedit.h>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace accounts {

class ADDialog : public DDialog
{
    Q_OBJECT
public:
    explicit ADDialog(QWidget *parent = nullptr);

    void setUserModel(UserModel *model);

signals:
    void requestInfos(const QString &server, const QString& account, const QString &password);

private:
    void initUI();

    enum ButtonType {
        Cancle = 0,
        Submit = 1
    };

private:
    DLineEdit *m_server;
    DLineEdit *m_username;
    DPasswordEdit *m_password;
    UserModel *m_model;
};
}
}

#endif // ADDIALOG_H
