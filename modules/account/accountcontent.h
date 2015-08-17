#ifndef ACCOUNTCONTENT_H
#define ACCOUNTCONTENT_H

#include <QWidget>

class AccountContent : public QWidget
{
    Q_OBJECT
public:
    struct ContentData {
        QStringList iconList;
        QStringList historyIconList;
        bool automaticLogin;
        bool locked;
        int accountType;
    };

    explicit AccountContent(const QString &userPath, QWidget *parent = 0);

private:
    QString m_userPath = "";
};

#endif // ACCOUNTCONTENT_H
