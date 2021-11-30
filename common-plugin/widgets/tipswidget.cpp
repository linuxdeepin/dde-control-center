#include "tipswidget.h"

#include <QPainter>
#include <QAccessible>
#include <QTextDocument>

#define MARGIN 10

TipsWidget::TipsWidget(QWidget *parent)
    : QFrame(parent)
{
}

void TipsWidget::setContext(const QList<QPair<QString, QStringList>> &textList)
{
    m_textList = textList;

    int width = 0;
    int height = 0;
    int titleWidth = 0;
    for (QPair<QString, QStringList> textPair : m_textList)
        titleWidth = qMax(titleWidth, fontMetrics().width(textPair.first));

    for (QPair<QString, QStringList> textPair : m_textList) {
        QString key = textPair.first;
        QStringList values = textPair.second;
        if (values.size() > 0) {
            for (const QString &value : values) {
                QString text = m_spliter + value;
                width = qMax(width, fontMetrics().width(text) + MARGIN * 2);
                height += fontMetrics().boundingRect(text).height();
            }
        } else {
            height += fontMetrics().boundingRect(key).height();
            width = MARGIN * 2;
        }
    }

    width += titleWidth;
    setFixedSize(width, height);

    update();
}

void TipsWidget::setSpliter(const QString &spliter)
{
    m_spliter = spliter;
}

// 计算右侧文字的左侧位置
int TipsWidget::calcValueX()
{
    int nMaxWidth = 0;
    for (QPair<QString, QStringList> textPair : m_textList) {
        int nCurrentTextWidth = fontMetrics().boundingRect(textPair.first).width();
        nMaxWidth = qMax(nMaxWidth, nCurrentTextWidth);
    }
    return MARGIN + nMaxWidth + fontMetrics().width(m_spliter);
}

/**
 * @brief TipsWidget::paintEvent 任务栏插件提示信息绘制
 * @param event
 */
void TipsWidget::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    painter.setPen(QPen(palette().brightText(), 1));

    QTextOption option;
    option.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    int y = 0;
    int valueX = calcValueX();
    for (QPair<QString, QStringList> textPair : m_textList) {
        QString key = textPair.first;
        QStringList values = textPair.second;
        QString displayText = (values.size() > 0 ? (QString("%1%2").arg(key).arg(m_spliter)) : key);
        int lineHeight = fontMetrics().boundingRect(displayText).height();
        painter.drawText(QRect(MARGIN, y, rect().width(), lineHeight), displayText, option);
        for (const QString &text : values) {
            int lineHeight = fontMetrics().boundingRect(text).height();
            painter.drawText(QRect(valueX, y, rect().width(), lineHeight), text, option);
            y += lineHeight;
        }
    }
}

bool TipsWidget::event(QEvent *event)
{
    if (event->type() == QEvent::FontChange)
        setContext(m_textList);

    return QFrame::event(event);
}
