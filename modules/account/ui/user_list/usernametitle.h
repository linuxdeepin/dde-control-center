#ifndef ACCOUNTNAMETITLE_H
#define ACCOUNTNAMETITLE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class HeaderNameLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(bool isCurrentUser READ isCurrentUser WRITE setIsCurrentUser)
public:
    explicit HeaderNameLabel(QWidget * = 0);

    void setIsCurrentUser(bool value);
    bool isCurrentUser() const;

private:
    bool m_isCurrentUser = false;
};

class UserNameTitle : public QWidget
{
    Q_OBJECT
public:
    explicit UserNameTitle(QWidget *parent = 0);
    void setUserName(const QString &name);
    void setUserType(const QString &type);
    void setIsCurrentUser(bool v);

private:
    QVBoxLayout *m_mainLayout = NULL;
    HeaderNameLabel *m_nameLabel = NULL;
    QLabel *m_typeLabel = NULL;
};

#endif // ACCOUNTNAMETITLE_H
