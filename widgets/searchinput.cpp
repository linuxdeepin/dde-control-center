#include "searchinput.h"
#include <QPainter>
#include <QApplication>

namespace dcc {
namespace widgets {


SearchInput::SearchInput(QWidget* parent)
    :QLineEdit(parent),
      m_iconVisible(true)
{
    setFocusPolicy(Qt::ClickFocus);
    m_search = tr("Search");
}

void SearchInput::setSearchText(const QString &text)
{
    m_search = text;
}

void SearchInput::setIconVisible(bool visible)
{
    m_iconVisible = visible;
}

QPixmap SearchInput::pixmap() const
{
    return m_icon;
}

void SearchInput::setPixmap(const QPixmap &pixmap)
{
    m_icon = pixmap;
}

void SearchInput::paintEvent(QPaintEvent *e)
{
    QLineEdit::paintEvent(e);

    if(!hasFocus() && text().isEmpty())
    {
        QRect rect = this->rect();
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(0.5);
        if(m_iconVisible)
        {
            QFontMetrics fm(qApp->font());
            int w = fm.width(m_search);
            int iw = m_icon.width();
            int x = (rect.width() - w - iw -8)/2;
            QRect iconRect(x, 0, iw, rect.height());
            QRect tmp(QPoint(0,0),m_icon.size());
            tmp.moveCenter(iconRect.center());
            QRect searchRect(iconRect.right() + 2, 0, w, rect.height());
            painter.drawPixmap(tmp, m_icon);
            painter.drawText(searchRect, Qt::AlignCenter, m_search);
        }
        else
        {
            painter.drawText(rect, Qt::AlignCenter, m_search);
        }
    }
}

}
}
