#ifndef FINGERITEM_H
#define FINGERITEM_H

#include "settingsitem.h"
#include <QWidget>
#include <QHBoxLayout>
#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace accounts {
class FingerItem : public widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit FingerItem(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setEditMode(const bool editmode);

private:
    QHBoxLayout *m_layout;
    bool m_editMode;
    DImageButton *m_editBtn;
    DImageButton *m_removeBtn;
    QLabel *m_title;
};
}
}

#endif // FINGERITEM_H
