#ifndef INPUTITEM_H
#define INPUTITEM_H

#include "settingsitem.h"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

using namespace dcc;

namespace dcc {
namespace keyboard{

class InputItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit InputItem(QFrame *parent = 0);

    void setTitle(const QString& title);
    void setValue(const QString& value);

    void setChooseVisible(bool visible);
    void setRightText(const QString& text);

    QString value() const;

    void setReadOnly(bool read);

signals:
    void click();

public slots:
    void onClick();

private:
    QLabel *m_title;
    QLineEdit *m_input;
    QPushButton* m_choose;
};

class TitleButtonItem : public SettingsItem
{
    Q_OBJECT
public:
    explicit TitleButtonItem(QFrame* parent = 0);
    void setTitle(const QString& title);
    void setValue(const QString& value);

signals:
    void click();

private:
    QLabel *m_title;
    QPushButton *m_button;
};

}
}
#endif // INPUTITEM_H
