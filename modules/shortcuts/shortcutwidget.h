#ifndef SHORTCUTWIDGET_H
#define SHORTCUTWIDGET_H

#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>

#include <libdui/libdui_global.h>

#include "searchlist.h"
#include "shortcutedit.h"

DUI_USE_NAMESPACE

class ImageNameButton;
class ShortcutWidget : public QFrame, public SearchItem
{
    Q_OBJECT

    Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged)

public:
    ShortcutWidget(ShortcutDbus *dbus, int id, const QString &title, const QString &shortcut = "", QWidget *parent = 0);

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
    void shortcutChanged(QString flag, QString shortcut);
    void removeShortcut();

private slots:
    void shortcutKeyFinished(const QString &str);

private:
    QHBoxLayout *m_hlayout;
    QVBoxLayout *m_vlayout;
    int m_id;
    QLabel *m_title;
    ShortcutEdit *m_shortcut;
    ShortcutWidget *m_me;
    ImageNameButton *m_removeButton;
    QPropertyAnimation *m_animation;
    QString m_shortcuText;
};

#endif // SHORTCUTWIDGET_H
