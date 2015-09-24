#include "inputwarningframe.h"

InputWarningFrame::InputWarningFrame(QWidget *sourceInput) :
    DArrowRectangle(DArrowRectangle::ArrowTop, NULL),m_sourceInput(sourceInput)
{
    D_THEME_INIT_WIDGET(InputWarningFrame);

    setArrowX(ARROW_LEFT_MARGIN);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::ToolTip) ;
    setMaximumWidth(sourceInput->maximumWidth());
    setShadowBlurRadius(0);
    setShadowDistance(0);

    m_label = new QLabel;
    m_label->setObjectName("WarningMessageLabel");
    m_label->setWordWrap(true);

    m_frame = new QFrame;
    QHBoxLayout *frameLayout = new QHBoxLayout(m_frame);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    frameLayout->addWidget(m_label);
    setContent(m_frame);
}

void InputWarningFrame::showWarning(const QString &msg)
{
    m_label->setText(msg);
    QFontMetrics metrics(m_label->font());
    int msgWidth = metrics.width(msg);
    m_label->setFixedWidth((msgWidth < m_sourceInput->maximumWidth() ? msgWidth : m_sourceInput->maximumWidth()) - 10);
    m_frame->adjustSize();
    resizeWithContent();
    QPoint sourcePoint = m_sourceInput->mapToGlobal(QPoint(0, 0));

    show(sourcePoint.x() + m_sourceInput->width() / 2, sourcePoint.y() + m_sourceInput->height());
}
