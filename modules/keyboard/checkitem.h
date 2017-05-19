#ifndef CHECKITEM_H
#define CHECKITEM_H

#include "settingsitem.h"
#include <dimagebutton.h>
#include <QPushButton>
#include <QLabel>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace keyboard{
class CheckItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit CheckItem(QFrame *parent = 0);
    void setTitle(const QString& title);
    QString title() const;

    void setChecked(bool checked);
    void setMultipleMode(bool multiple);
    bool checked() const;

signals:
    void checkedChanged(const QString &title);
    void destroySelf(CheckItem* item);

public slots:
    void onEditMode(bool value);
    void onClick();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    bool m_checked;
    bool m_bEdit;
    QLabel* m_title;
    DImageButton* m_checkBtn;
    int m_circle;
    bool m_multiple;
};
}
}
#endif // CHECKITEM_H
