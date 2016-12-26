#ifndef TITLEVALUEITEM_H
#define TITLEVALUEITEM_H

#include "settingsitem.h"
#include <QLabel>

using namespace dcc::widgets;

namespace dcc{
namespace systeminfo{

class TitleValueItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit TitleValueItem(QFrame *parent = 0);
    void setTitle(const QString& title);
    void setValue(const QString& value);

private:
    QLabel* m_title;
    QLabel* m_value;
};


class LogoItem : public SettingsItem
{
    Q_OBJECT

    Q_PROPERTY(QString logo WRITE setLogo)
public:
    explicit LogoItem(QFrame* parent = 0);
    void setDescription(const QString& des);
    void setLogo(const QString& logo);

private:
    QLabel* m_logo;
    QLabel* m_description;
};

}
}
#endif // TITLEVALUEITEM_H
