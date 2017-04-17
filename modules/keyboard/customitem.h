#ifndef CUSTOMITEM_H
#define CUSTOMITEM_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>

#include "shortcutitem.h"
#include "settingsitem.h"
#include "keyboardwork.h"

using namespace dcc::widgets;

namespace dcc {
namespace keyboard {
class CustomItem : public SettingsItem
{
    Q_OBJECT
public:
    explicit CustomItem(KeyboardWork *work, QWidget *parent = 0);
    void setInfo(ShortcutInfo * info);
    QString text() const;

public slots:
    void onKeyEvent(bool press, const QString &keylist);

signals:
    void shortcut(const QString& shortcut);

protected:
    void mousePressEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void leaveEvent(QEvent *e);

private:
    KeyboardWork* m_work;
    QLineEdit* m_shortcutEdit;
    QLabel* m_title;
    QRect m_rect;
    bool m_contain;
    ShortcutInfo* m_info;
    QString m_accels;
};
}
}


#endif // CUSTOMITEM_H
