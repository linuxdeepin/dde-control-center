#ifndef INPUTITEM_H
#define INPUTITEM_H

#include "settingsitem.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

using namespace dcc;

class InputItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit InputItem(QFrame *parent = 0);

    void setTitle(const QString& title);
    void setValue(const QString& value);

    void setChooseVisible(bool visible);

    QString value() const { return m_input->text(); }

public slots:
    void onClick();

private:
    QLabel *m_title;
    QLineEdit *m_input;
    QPushButton* m_choose;
};

class TitleValueItem : public SettingsItem
{
    Q_OBJECT
public:
    explicit TitleValueItem(QFrame* parent = 0);
    void setTitle(const QString& title);
    void setValue(const QString& value);

private:
    QLabel *m_title;
    QLabel *m_value;
};

#endif // INPUTITEM_H
