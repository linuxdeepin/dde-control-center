#ifndef SETTINGSOPTIONITEM_H
#define SETTINGSOPTIONITEM_H

#include <QWidget>
#include <QLabel>

class SettingsOptionItem : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsOptionItem(QWidget *parent = 0);

    void setTitle(const QString &title);
    void setSelected(const bool checked);

private:
    QLabel *m_title;
    QLabel *m_icon;
};

#endif // SETTINGSOPTIONITEM_H
