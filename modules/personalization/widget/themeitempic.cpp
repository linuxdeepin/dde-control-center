#include "themeitempic.h"
using namespace dcc;
using namespace dcc::personalization;
using namespace dcc::widgets;
ThemeItemPic::ThemeItemPic(const QString &path, QWidget *parent)
    :TranslucentFrame(parent)
{
    m_show = new QLabel(this);
    m_mainLayout = new QVBoxLayout;

    QPixmap pix(path);
    QPixmap mask(":/personalization/themes/icon/mask.png");

    m_show->setMask(mask.mask());
    m_show->setPixmap(pix);

    m_mainLayout->addWidget(m_show);
    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);
    setObjectName("TiemeItemPic");
}
