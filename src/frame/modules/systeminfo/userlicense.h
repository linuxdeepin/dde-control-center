#ifndef USERLICENSE_H
#define USERLICENSE_H

#include "widgets/contentwidget.h"

namespace dcc{

namespace widgets {
class TipsLabel;
}

namespace systeminfo{
class UserLicense : public ContentWidget
{
    Q_OBJECT
public:
    UserLicense(QWidget *parent = nullptr);

private:
    widgets::TipsLabel *m_body;
};
}
}

#endif // USERLICENSE_H
