#ifndef SETTINGSHEAD_H
#define SETTINGSHEAD_H

#include "settingsitem.h"
#include <QPushButton>
#include <QLabel>

namespace dcc {

class SettingsHead : public SettingsItem
{
    Q_OBJECT

public:
    explicit SettingsHead(QFrame *parent = 0);
    void setTitle(const QString& title);

signals:
    void editChanged(bool edit);

public slots:
    void initStatus();

protected:
    void mouseMoveEvent(QMouseEvent* e);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QLabel *m_title;
    QLabel *m_edit;
};

class TimeButton : public SettingsItem
{
    Q_OBJECT

public:
    explicit TimeButton(QFrame *parent = 0);

signals:
    void confirm();
    void cancel();

private:
    QPushButton* m_confirm;
    QPushButton* m_cancel;
};
}
#endif // SETTINGSHEAD_H
