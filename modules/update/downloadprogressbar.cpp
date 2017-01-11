#include "downloadprogressbar.h"

#include <QMouseEvent>
#include <QHBoxLayout>

#include "labels/normallabel.h"

namespace dcc{
namespace update{

DownloadProgressBar::DownloadProgressBar(QWidget* parent)
    :QProgressBar(parent),
      m_message(new dcc::widgets::NormalLabel)
{
    setRange(0, 100);
    setValue(maximum());
    setTextVisible(false);
    setFixedHeight(36);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addStretch();
    layout->addWidget(m_message, 0, Qt::AlignVCenter);
    layout->addStretch();

    setLayout(layout);
}

void DownloadProgressBar::setMessage(const QString &message)
{
    m_message->setText(message);
}

void DownloadProgressBar::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();
    emit clicked();

    QProgressBar::mouseReleaseEvent(e);
}

}
}
