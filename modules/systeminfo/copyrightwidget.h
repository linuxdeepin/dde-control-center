#ifndef COPYRIGHTWIDGET_H
#define COPYRIGHTWIDGET_H

#include "contentwidget.h"

namespace dcc {
namespace widgets {
class TipsLabel;
}
}

using namespace dcc::widgets;

namespace dcc{
namespace systeminfo{

class CopyrightWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit CopyrightWidget(QWidget* parent = 0);
    void setLicenseTitle(const QString& title);
    void setLicenseBody(const QString& body);

private:
    TipsLabel* m_title;
    TipsLabel* m_body;
};

}
}
#endif // COPYRIGHTWIDGET_H
