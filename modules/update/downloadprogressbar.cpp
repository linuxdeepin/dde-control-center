#include "downloadprogressbar.h"
#include <QPainter>

DownloadProgressBar::DownloadProgressBar(QWidget* parent)
    :QProgressBar(parent),
      m_type(CheckUpdate)
{
    setMinimum(0);
    setMaximum(100);

    setValue(100);

    this->setTextVisible(false);
}

void DownloadProgressBar::setCurState(UpdateType type)
{
    if(m_type == type)
        return;

    m_type = type;
    update();
}

void DownloadProgressBar::paintEvent(QPaintEvent *e)
{
    QProgressBar::paintEvent(e);
    QPainter painter(this);

    if(m_type == UpdateType::CheckUpdate)
    {
        m_state = tr("Download Update");
    }
    else if(m_type == UpdateType::StartDownload)
    {
        m_state = tr("Downloaded ") + text()  + tr(" (Click to pause)");
    }
    else if(m_type == UpdateType::StopDownload)
    {
        m_state = tr("Downloaded ") + text()  + tr(" (Click to continue)");
    }
    else
    {
        m_state = tr("Restart to install updates");
    }
    painter.drawText(this->rect(), Qt::AlignCenter, m_state);
}

void DownloadProgressBar::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_type == UpdateType::CheckUpdate)
        setValue(0);

    emit action(m_type);
    QProgressBar::mouseReleaseEvent(e);
}
