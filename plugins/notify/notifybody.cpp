#include "notifybody.h"
#include <QPainter>
#include <QDebug>

NotifyBody::NotifyBody(QWidget *parent) : QWidget(parent)
{
    m_td = new QTextDocument;
//    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFixedSize(272, 50);
}

void NotifyBody::setText(const QString &text)
{
    QTextOption bodyOption;
    QFont bodyfont;

    bodyOption.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    bodyOption.setWrapMode(QTextOption::WordWrap);
    bodyfont.setFamily("Noto Sans CJK SC");
    bodyfont.setPixelSize(12);

    m_td->setDefaultTextOption(bodyOption);
    m_td->setDefaultFont(bodyfont);
    m_td->setTextWidth(this->width());
    m_td->setDocumentMargin(0);

    const QFontMetrics fm(bodyfont);

    m_td->setHtml(holdTextInRect(fm, text, this->rect()));

    update();
}

void NotifyBody::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setBrush(QBrush(Qt::transparent));
    painter.setPen(Qt::black);

    m_td->drawContents(&painter);

    QWidget::paintEvent(event);
}

const QString NotifyBody::holdTextInRect(const QFontMetrics &fm, const QString &text, const QRect &rect) const
{
    const int textFlag = Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap | Qt::TextWrapAnywhere;

    if (rect.contains(fm.boundingRect(rect, textFlag, text)))
        return text;

    QString str(text + "...");

    while (true)
    {
        if (str.size() < 4)
            break;

        QRect boundingRect = fm.boundingRect(rect, textFlag, str);
        if (rect.contains(boundingRect))
            break;

        str.remove(str.size() - 4, 1);
    }

    return str;
}
