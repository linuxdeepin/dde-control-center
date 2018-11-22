#include "userlicense.h"
#include "widgets/translucentframe.h"
#include "widgets/labels/tipslabel.h"
#include "widgets/utils.h"

#include <QVBoxLayout>

using namespace dcc;
using namespace dcc::systeminfo;
using namespace dcc::widgets;

UserLicense::UserLicense(QWidget *parent)
    : ContentWidget(parent)
{
    setTitle(tr("End User License Agreement"));

    TranslucentFrame* widget = new TranslucentFrame;
    widget->setObjectName("copyrightWidget");
    QVBoxLayout *layout =new QVBoxLayout();

    m_body = new TipsLabel;
    m_body->setWordWrap(true);

    layout->addWidget(m_body);

    widget->setLayout(layout);
    setContent(widget);

    m_body->setText(getLicense(":/systeminfo/license/deepin-end-user-license-agreement_%1.txt", "title"));
}
