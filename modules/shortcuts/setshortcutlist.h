#ifndef SETSHORTCUTLIST_H
#define SETSHORTCUTLIST_H

#include <QFrame>
#include <QVBoxLayout>

class ShortcutWidget : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QString shortcutName READ shortcutName WRITE setShortcutName NOTIFY shortcutNameChanged)

public:
    ShortcutWidget(const QString &title, const QString &shortcutName = "", QWidget *parent = 0);

public slots:
    void setTitle(const QString &title);
    void setShortcutName(const QString &shortcutName);

public:
    QString title() const;
    QString shortcutName() const;

signals:
    void shortcutNameChanged(QString shortcutName);

private:
    QString m_shortcutName;
    QString m_title;
};

class SetShortcutList : public QFrame
{
    Q_OBJECT

public:
    explicit SetShortcutList(QWidget *parent = 0);

public slots:
    int addItem(const QString &title, const QString &shortcutName = "");
    void addItems(const QStringList &titles, const QStringList &shortcutNames = QStringList());
    void insertItem(int index, const QString &title, const QString &shortcutName = "");
    void insertItems(int index, const QStringList &titles, const QStringList &shortcutNames = QStringList());
    void setItemTitle(int index, const QString &title);
    void setItemShortcutName(int index, const QString &name);

public:
    int count() const;
    QString getItemTitle(int index) const;
    QString getItemShortcutName(int index) const;
    int indexOfByTitle(const QString &title) const;
    ShortcutWidget *getItemWidget(int index) const;

signals:
    void shortcutNameChanged(int index, QString name);
    void shortcutNameChanged(QString title, QString name);

private slots:
    void shortcutNameChanged(const QString &name);

private:
    QVBoxLayout *m_layout;
};

#endif // SETSHORTCUTLIST_H
