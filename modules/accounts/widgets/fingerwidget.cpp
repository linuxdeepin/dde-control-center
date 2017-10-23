#include "fingerwidget.h"

#include <DHiDPIHelper>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::accounts;

FingerWidget::FingerWidget(QWidget *parent)
    : QWidget(parent)
    , m_view(new DPictureSequenceView)
{
    QStringList lists;
    for(uint i = 0; i < 39; i++)
    {
        QString path = QString(":/accounts/themes/dark/icons/finger/fingerprint_%2.png").arg(i, 2, 10);
        lists << path;
    }

    m_view->setPictureSequence(lists);
    m_view->play();

    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(m_view);
    setLayout(layout);
}
