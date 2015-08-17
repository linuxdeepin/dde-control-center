#ifndef ACCOUNTNAMETITLE_H
#define ACCOUNTNAMETITLE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class AccountNameTitle : public QWidget
{
    Q_OBJECT
public:
    explicit AccountNameTitle(QWidget *parent = 0);
    void setUserName(const QString &name);
    void setUserType(const QString &type);
    void setIsCurrentUser(bool v);

private:
    QVBoxLayout *m_mainLayout = NULL;
    QLabel *m_nameLabel = NULL;
    QLabel *m_typeLabel = NULL;
    bool m_isCurrentUser = false;
};

#endif // ACCOUNTNAMETITLE_H
