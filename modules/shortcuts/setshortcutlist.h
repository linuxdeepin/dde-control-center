#ifndef SETSHORTCUTLIST_H
#define SETSHORTCUTLIST_H

#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>

#include <libdui/libdui_global.h>
#include <libdui/dimagebutton.h>
#include <libdui/dshortcutedit.h>

#include "searchlist.h"

DUI_USE_NAMESPACE

class ShortcutWidget : public QFrame, public SearchItem
{
    Q_OBJECT

    Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged)

public:
    ShortcutWidget(int id, const QString &title, const QString &shortcut = "", QWidget *parent = 0);

public slots:
    void setId(int id);
    void setTitle(const QString &title);
    void setShortcut(const QString &shortcut);
    void showRemoveButton() const;
    void hideRemoveButton() const;

public:
    int id() const;
    QString title() const;
    QString shortcut() const;

    QStringList keyWords() const Q_DECL_OVERRIDE;
    void setData(const QVariant &datas) Q_DECL_OVERRIDE;
    QVariant getData() Q_DECL_OVERRIDE;
    QWidget *widget() const Q_DECL_OVERRIDE;
signals:
    void shortcutChanged(QString shortcut);
    void removeShortcut();

private:
    QHBoxLayout *m_layout;
    int m_id;
    QLabel *m_title;
    DShortcutEdit *m_shortcut;
    ShortcutWidget *m_me;
    DImageButton *m_removeButton;
    QPropertyAnimation *m_animation;
    QString m_shortcuText;
};

class SetShortcutList : public QFrame
{
    Q_OBJECT

public:
    explicit SetShortcutList(QWidget *parent = 0);

public slots:
    int addItem(int id, const QString &title, const QString &shortcut = "");
    void addItems(QList<int> ids, const QStringList &titles, const QStringList &shortcuts = QStringList());
    void insertItem(int index, int id, const QString &title, const QString &shortcut = "");
    void insertItems(int index, QList<int> ids, const QStringList &titles, const QStringList &shortcuts = QStringList());
    void setItemId(int index, int id);
    void setItemTitle(int index, const QString &title);
    void setItemShortcut(int index, const QString &value);
    void setItemSize(int w, int h);
    void clear();
    void removeItem(int index);

public:
    int count() const;
    QString getItemTitle(int index) const;
    QString getItemShortcut(int index) const;
    int getItemId(int index) const;
    int indexOfByTitle(const QString &title) const;
    int indexOfById(int id) const;
    ShortcutWidget *getItemWidget(int index) const;

signals:
    void shortcutChanged(int index, QString shortcut);
    void shortcutChanged(QString title, QString shortcut);

private slots:
    void shortcutChanged(const QString &value);

private:
    QVBoxLayout *m_layout;

    int m_itemWidth, m_itemHeight;
};

#endif // SETSHORTCUTLIST_H
