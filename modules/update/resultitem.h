#ifndef DCC_UPDATE_RESULTITEM_H
#define DCC_UPDATE_RESULTITEM_H

#include "settingsitem.h"

#include "dimagebutton.h"

namespace dcc {
namespace widgets {
class NormalLabel;
}
}

namespace dcc {
namespace update {

class ResultItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit ResultItem(QFrame* parent = 0);

    void setSuccess(bool success);

private:
    dcc::widgets::NormalLabel* m_message;
    QLabel *m_icon;
};

} // namespace update
} // namespace dcc

#endif // DCC_UPDATE_RESULTITEM_H
