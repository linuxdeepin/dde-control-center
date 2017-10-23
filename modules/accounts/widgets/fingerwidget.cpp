#include "fingerwidget.h"

#include <DHiDPIHelper>
#include <QVBoxLayout>
#include <QDebug>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::accounts;

FingerWidget::FingerWidget(QWidget *parent)
    : QWidget(parent)
    , m_view(new DPictureSequenceView)
    , m_tipLbl(new QLabel(this))
{
    QStringList lists;
    for(uint i = 0; i != 40; i++)
    {
        QString path = QString(":/accounts/themes/dark/icons/finger/fingerprint_%2.png").arg(i, 2, 10, QChar('0'));
        lists << path;
    }

    m_view->setPictureSequence(lists);
    m_view->play();

    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(m_view, 0, Qt::AlignCenter);
    layout->addWidget(m_tipLbl, 0, Qt::AlignHCenter);

    setLayout(layout);
}

void FingerWidget::setFrequency(const QString &value)
{
    m_tipLbl->setText(value);
}

void FingerWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    QPainterPath path;

    path.addRoundedRect(rect(), 5, 5);
    painter.fillPath(path, QColor(255, 255, 255, 0.2 * 255));
}
