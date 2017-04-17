#include "customitem.h"

#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QFont>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::keyboard;

CustomItem::CustomItem(KeyboardWork *work, QWidget *parent)
    :SettingsItem(parent),
      m_work(work),
      m_contain(false),
      m_info(nullptr)
{
    setMouseTracking(true);
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(20,0,10,0);
    layout->setSpacing(2);

    m_title = new QLabel();
    m_title->setText(tr("Shortcut"));
    m_title->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_title);
    layout->setAlignment(m_title, Qt::AlignLeft);

    m_shortcutEdit = new QLineEdit(this);
    m_shortcutEdit->setReadOnly(true);
    m_shortcutEdit->hide();

    layout->addStretch();

    setLayout(layout);
    setFixedHeight(36);

    connect(m_work, &KeyboardWork::KeyEvent, this, &CustomItem::onKeyEvent);
}

void CustomItem::setInfo(ShortcutInfo *info)
{
    m_info = info;
    m_accels = info->accels;

    update();
}

QString CustomItem::text() const
{
    return m_accels;
}

void CustomItem::onKeyEvent(bool press, const QString &keylist)
{
    if(!press)
    {
        if(!m_shortcutEdit->isVisible())
            return;

        if (keylist == "BackSpace" || keylist == "escape") {
            m_shortcutEdit->hide();
        } else {
            QString in = keylist;
            in.replace("<", "");
            in.replace(">", "-");
            in.replace("_L", "");
            QStringList value = in.split("-");
            m_accels = keylist;
            QMap<QString, bool> list;
            for (QString key : ModelKeylist) {
                QStringList t;
                t << value << key;
                list.insert(key, m_work->keyOccupy(t));
            }

            m_shortcutEdit->hide();

            if (!keylist.isEmpty()){
                emit shortcut(keylist);
            }
        }
    }

    update ();
}

void CustomItem::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);

    if(!m_shortcutEdit->isVisible() && m_rect.contains(e->pos()))
    {
        QRect r = QRect(0,m_rect.y(),width()/2,m_rect.height());
        r.moveRight(m_rect.right());
        m_shortcutEdit->setGeometry(r);
        m_shortcutEdit->clear();
        m_work->grabScreen();
        m_shortcutEdit->setFocus();
        m_shortcutEdit->show();
        m_shortcutEdit->setPlaceholderText(tr("Please enter a shortcut"));
    }
    else
    {
        m_shortcutEdit->hide();
    }
    update();
}

void CustomItem::paintEvent(QPaintEvent *e)
{
    SettingsItem::paintEvent(e);
    if(m_shortcutEdit->isVisible())
    {
       return;
    }

    if (m_accels.isEmpty()) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QFont font = qApp->font();
        QFontMetrics fm(font);

        int right = this->rect().right() - 10;
        int w = fm.width(tr("Please enter a shortcut"));
        int h = (height() - fm.height()-6)/2;
        QRect r(0,0,w+8,height());
        r.moveRight(right);

        painter.drawText(r, Qt::AlignCenter, tr("Please enter a shortcut"));

        m_rect = QRect(r.topLeft(),this->rect().bottomRight());
        m_rect = m_rect.adjusted(0,h,0,-h);

        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QFont font = qApp->font();
    QFontMetrics fm(font);
    QString accels;
    accels = m_accels;
    accels = accels.replace("Control", "Ctrl");
    accels = accels.replace("<", "");
    accels = accels.replace(">", "-");
    accels = accels.replace("Above_Tab", "`");
    accels = accels.replace("Super_L", "Super");
    accels = accels.replace("Up", "↑");
    accels = accels.replace("Down", "↓");
    accels = accels.replace("Left", "←");
    accels = accels.replace("Right", "→");

    QString conflict(tr("Conflict"));
    QStringList splits = accels.split("-");
    int right = this->rect().right() - 10;
    for(int i = splits.count() - 1; i>=0; --i)
    {

        QString str = splits.at(i);
        int w;
        if (str == "null") {
            w = fm.width(conflict);
        } else {
            w = fm.width(str);
        }
        int h = (height() - fm.height()-6)/2;
        QRect r(0,0,w+8,height());
        r.moveRight(right);
        right = right - w - 12;
        painter.setBrush(palette().color(QPalette::Window));
        painter.save();
        painter.setPen(Qt::NoPen);
        painter.drawRoundRect(r.adjusted(0,h,0,-h));
        painter.restore();
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(m_contain ? 1.0 : 0.85);
        if(str == "null")
        {
            QPen pen = painter.pen();
            QColor col = pen.color();
            pen.setColor(Qt::red);
            painter.setPen(pen);
            painter.drawText(r,Qt::AlignCenter,tr("Conflict"));
            pen.setColor(col);
            painter.setPen(pen);
        }
        else
            painter.drawText(r,Qt::AlignCenter,str);

        painter.restore();
        if(i == 0)
        {
            m_rect = QRect(r.topLeft(),this->rect().bottomRight());
            m_rect = m_rect.adjusted(0,h,0,-h);
        }
    }
}

void CustomItem::mouseMoveEvent(QMouseEvent *e)
{
    m_contain = m_rect.contains(e->pos());

    update();
}

void CustomItem::leaveEvent(QEvent *e)
{
    Q_UNUSED(e)

    m_contain = false;

    update();
}
