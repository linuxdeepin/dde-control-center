#include "loadingnextpagewidget.h"

#include <QBoxLayout>

namespace dcc {
namespace widgets {

LoadingNextPageWidget::LoadingNextPageWidget(QFrame *parent)
    : NextPageWidget(parent),

      m_loadingIndicator(new DPictureSequenceView(this))
{
    m_loadingIndicator->setPictureSequence(":/frame/themes/dark/icons/dark_loading/loading_%1.png", QPair<int, int>(0, 90), 3);
    m_loadingIndicator->setFixedSize(30, 30);
    m_loadingIndicator->stop();
    m_loadingIndicator->setVisible(false);

    QHBoxLayout *mainLayout = static_cast<QHBoxLayout *>(layout());
    mainLayout->insertWidget(4, m_loadingIndicator);
    mainLayout->setAlignment(m_loadingIndicator, Qt::AlignVCenter);
}

void LoadingNextPageWidget::setLoading(const bool loading)
{
    m_loadingIndicator->setVisible(loading);

    if (loading)
        m_loadingIndicator->play();
    else
        m_loadingIndicator->stop();
}

}

}
