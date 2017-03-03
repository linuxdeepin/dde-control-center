#ifndef SUMMARYITEM_H
#define SUMMARYITEM_H

#include "settingsitem.h"

namespace dcc {
namespace widgets {
class NormalLabel;
class TipsLabel;
}
}

namespace dcc{
namespace update{

class SummaryItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit SummaryItem(QFrame * parent = 0);

    void setTitle(const QString& title);
    void setDetails(const QString& details);

private:
    dcc::widgets::NormalLabel *m_title;
    dcc::widgets::TipsLabel *m_details;
};

}
}
#endif // SUMMARYITEM_H
