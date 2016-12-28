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
    enum State
    {
        Edit,
        Cancel,
    };

public:
    explicit SettingsHead(QFrame *parent = 0);

    void setTitle(const QString& title);
    void setEditEnable(bool state = true);

signals:
    void editChanged(bool edit);

private slots:
    void toEdit();
    void toCancel();
    void refershButton();

private:
    QLabel *m_title;
    QPushButton *m_edit;
    QPushButton *m_cancel;

    State m_state;
};

}
}

#endif // SETTINGSHEAD_H
