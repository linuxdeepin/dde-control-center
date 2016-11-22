#include "searchinput.h"
#include <QPainter>
#include <QApplication>

using namespace dcc;

SearchInput::SearchInput(QWidget* parent)
    :QLineEdit(parent)
{
    setFocusPolicy(Qt::ClickFocus);
    m_search = tr("Search");
    m_icon = QPixmap(":/widgets/icons/search_press.png");
}

void SearchInput::setSearchText(const QString &text)
{
    m_search = text;
}

void SearchInput::paintEvent(QPaintEvent *e)
{
    QLineEdit::paintEvent(e);

    if(!hasFocus())
    {
        QRect rect = this->rect();
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(0.5);
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
}
