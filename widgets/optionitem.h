#ifndef OPTIONITEM_H
#define OPTIONITEM_H

#include "settingsitem.h"
#include "labels/normallabel.h"

#include <QHBoxLayout>

namespace dcc
{

class OptionItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit OptionItem(QWidget *parent = 0);

    void setTitle(const QString &title);

signals:
    void clicked() const;

protected:
    void mouseReleaseEvent(QMouseEvent *e);

protected:
    QHBoxLayout *m_mainLayout;
    NormalLabel *m_title;
    QLabel *m_selectedIcon;
};

} // namespace dcc;

#endif // OPTIONITEM_H
