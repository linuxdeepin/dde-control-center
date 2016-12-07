#include "waitindicator.h"
#include <QVBoxLayout>

namespace dcc {
namespace widgets {

WaitIndicator::WaitIndicator(QFrame *parent)
    :SettingsItem(parent)
{
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0,10,0,10);
    layout->setSpacing(0);

    m_view = new DPictureSequenceView();
    QStringList lists;
    for(int i = 0; i<89; i++)
    {
        QString arg = QString().setNum(i);
        if(arg.length() == 1)
            arg = "0"+arg;
        QString path = QString(":/widgets/icons/Loading/loading_0%1").arg(arg);
        lists<<path;
    }
    QPixmap pix(lists.at(0));
    m_view->setFixedSize(pix.size());
    m_view->setPictureSequence(lists);
    layout->addWidget(m_view);
    layout->setAlignment(m_view, Qt::AlignCenter);
    m_title = new QLabel();
    m_title->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_title);

    setLayout(layout);
    setFixedHeight(80);
}

void WaitIndicator::setPictureSequence(const QStringList &sequence)
{
    m_view->setPictureSequence(sequence);
}

void WaitIndicator::setTitle(const QString &title)
{
    m_title->setText(title);
}

void WaitIndicator::setTitleDisplay(bool display)
{
    m_title->setVisible(display);
}

void WaitIndicator::play()
{
    m_view->play();
}

void WaitIndicator::pause()
{
    m_view->pause();
}

void WaitIndicator::setSpeed(int speed)
{
    m_view->setSpeed(speed);
}

}
}
