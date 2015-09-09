#include "inputwarningframe.h"

InputWarningFrame::InputWarningFrame(QWidget *sourceInput) :
    DArrowRectangle(DArrowRectangle::ArrowTop, NULL),m_sourceInput(sourceInput)
{
    D_THEME_INIT_WIDGET(InputWarningFrame);

    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint);
    setArrowX(ARROW_LEFT_MARGIN);
    setMaximumWidth(sourceInput->maximumWidth());

    m_label = new QLabel;
    m_label->setObjectName("WarningMessageLabel");
    m_label->setWordWrap(true);
    m_label->setMaximumWidth(sourceInput->maximumWidth());

    QFrame *frame = new QFrame;
    frame->setMaximumWidth(sourceInput->maximumWidth());
    QHBoxLayout *frameLayout = new QHBoxLayout(frame);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    frameLayout->addWidget(m_label);
    setContent(frame);
}

void InputWarningFrame::showWarning(const QString &msg)
{
    m_label->setText(msg);
    QFontMetrics metrics(m_label->font());
    int msgWidth = metrics.width(msg);
    m_label->setMinimumWidth((msgWidth < m_sourceInput->maximumWidth() ? msgWidth : m_sourceInput->maximumWidth()) - 10);
    QPoint sourcePoint = m_sourceInput->mapToGlobal(QPoint(0, 0));
    DArrowRectangle::show(sourcePoint.x() + width() / 2, sourcePoint.y() + m_sourceInput->height());
}
