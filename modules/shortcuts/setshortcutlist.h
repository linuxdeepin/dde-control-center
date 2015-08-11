#ifndef SETSHORTCUTLIST_H
#define SETSHORTCUTLIST_H

#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>

#include <libdui/libdui_global.h>

class ShortcutWidget : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged)

public:
    ShortcutWidget(const QString &title, const QString &shortcut = "", QWidget *parent = 0);

public slots:
    void setTitle(const QString &title);
    void setShortcut(const QString &shortcut);

public:
    QString title() const;
    QString shortcut() const;

signals:
    void shortcutChanged(QString shortcut);

private:
    QHBoxLayout *m_layout;
    QLabel *m_title;
    QLabel *m_shortcut;
};

class SetShortcutList : public QFrame
{
    Q_OBJECT

public:
    explicit SetShortcutList(QWidget *parent = 0);

public slots:
    int addItem(const QString &title, const QString &shortcut = "");
    void addItems(const QStringList &titles, const QStringList &shortcuts = QStringList());
    void insertItem(int index, const QString &title, const QString &shortcut = "");
    void insertItems(int index, const QStringList &titles, const QStringList &shortcuts = QStringList());
    void setItemTitle(int index, const QString &title);
    void setItemShortcut(int index, const QString &value);
    void setItemSize(int w, int h);

public:
    int count() const;
    QString getItemTitle(int index) const;
    QString getItemShortcut(int index) const;
    int indexOfByTitle(const QString &title) const;
    ShortcutWidget *getItemWidget(int index) const;

signals:
    void shortcutChanged(int index, QString name);
    void shortcutChanged(QString title, QString name);

private slots:
    void shortcutChanged(const QString &value);

private:
    QVBoxLayout *m_layout;

    int m_itemWidth, m_itemHeight;
};

#endif // SETSHORTCUTLIST_H
