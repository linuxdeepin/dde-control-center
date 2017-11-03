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
    , m_isFinished(false)
{
    for(uint i = 0; i != 58; i++)
    {
        QString path = QString(":/accounts/themes/dark/icons/finger/entering/fingerprint_%2.png").arg(i, 2, 10, QChar('0'));
        m_enteringList << path;
    }

    for(uint i = 0; i != 30; i++)
    {
        QString path = QString(":/accounts/themes/dark/icons/finger/finished/success_%2.png").arg(i, 2, 10, QChar('0'));
        m_finishedList << path;
    }

    m_view->setPictureSequence(m_enteringList);
    m_view->setSingleShot(true);

    m_tipLbl->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(m_view, 0, Qt::AlignCenter);
    layout->addWidget(m_tipLbl, 0, Qt::AlignHCenter);

    setLayout(layout);

    connect(m_view, &DPictureSequenceView::playEnd, this, [=] {
        if (m_isFinished)
            m_view->setPictureSequence(QStringList() << ":/accounts/themes/dark/icons/finger/finished/success_30.png");
        else
            emit playEnd();
    });
}

void FingerWidget::setFrequency(const QString &value)
{
    m_tipLbl->setText(value);
}

void FingerWidget::reEnter()
{
    m_isFinished = false;
    m_view->setPictureSequence(m_enteringList);
}

void FingerWidget::next()
{
    m_isFinished = false;
    m_view->setPictureSequence(m_enteringList);
    m_view->play();
}

void FingerWidget::finished()
{
    m_isFinished = true;
    m_view->setPictureSequence(m_finishedList);
    m_view->play();
}

void FingerWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    QPainterPath path;

    path.addRoundedRect(rect(), 5, 5);
    painter.fillPath(path, QColor(255, 255, 255, 0.2 * 255));
}
