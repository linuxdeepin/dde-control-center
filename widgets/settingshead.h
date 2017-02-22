#ifndef SETTINGSHEAD_H
#define SETTINGSHEAD_H

#include "settingsitem.h"
#include <QPushButton>

namespace dcc {
namespace widgets {

class NormalLabel;
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

public slots:
    void toEdit();
    void toCancel();

signals:
    void editChanged(bool edit);

private slots:
    void refershButton();

private:
    NormalLabel *m_title;
    QPushButton *m_edit;
    QPushButton *m_cancel;

    State m_state;
};

}
}

#endif // SETTINGSHEAD_H
