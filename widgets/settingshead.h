#ifndef SETTINGSHEAD_H
#define SETTINGSHEAD_H

#include "settingsitem.h"
#include <QPushButton>
#include <QLabel>

namespace dcc {
namespace widgets {

class SettingsHead : public SettingsItem
{
    Q_OBJECT

public:
    explicit SettingsHead(QFrame *parent = 0);
    void setTitle(const QString& title);
    void setEditEnable(bool state);

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

}
}

#endif // SETTINGSHEAD_H
