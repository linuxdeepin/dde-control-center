#include "shortcutitem.h"
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QApplication>
#include <QPainter>

using namespace dcc;

ShortcutItem::ShortcutItem(QFrame *parent)
    :SettingsItem(parent),
      m_hlayout(new QHBoxLayout()),
      m_contain(false)
{
    setMouseTracking(true);
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(2);
    m_hlayout->setMargin(0);
    m_hlayout->setSpacing(5);

    m_title = new QLabel();
    m_title->setText("");
    m_title->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_title);
    layout->setAlignment(m_title, Qt::AlignLeft);
    layout->addLayout(m_hlayout);    
    layout->setAlignment(m_hlayout, Qt::AlignRight);

    m_shortcutEdit = new QLineEdit(this);
    m_shortcutEdit->setReadOnly(true);
    m_shortcutEdit->hide();

    setLayout(layout);
    setFixedHeight(36);

    m_inter = new KeybingdingInter("com.deepin.daemon.Keybinding",
                                          "/com/deepin/daemon/Keybinding",
                                          QDBusConnection::sessionBus(), this);

    connect(m_inter, SIGNAL(KeyEvent(bool,QString)), this, SLOT(onKeyEvent(bool,QString)));
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChanged(QWidget*,QWidget*)));
}

void ShortcutItem::setShortcutInfo(ShortcutInfo *info)
{
    m_info = info;
    m_info->item = this;
    m_title->setText(info->name);
}

void ShortcutItem::onFocusChanged(QWidget *old, QWidget *now)
{
    if(!old && now != m_shortcutEdit)
        m_shortcutEdit->hide();
}

void ShortcutItem::onKeyEvent(bool in0, QString key)
{
    if(!in0)
    {
        if(!m_shortcutEdit->isVisible())
            return;

        if(key.isEmpty() || key.toLower() == "escape"){
            m_shortcutEdit->hide();
            return;
        }

        if(key.toLower() == "backspace" || key.toLower() == "delete"){
            m_shortcutEdit->hide();
        }else{
            m_shortcutEdit->hide();
            if(m_info->accels != key)
            {
                bool result = m_inter->CheckAvaliable(key);
                emit shortcutChangd(result, m_info, key);
            }
        }
    }
    else
    {
        if(key.isEmpty())
            m_shortcutEdit->hide();
    }
}

void ShortcutItem::paintEvent(QPaintEvent *e)
{
    SettingsItem::paintEvent(e);
    if(m_shortcutEdit->isVisible())
    {
       return;
    }
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QFont font = qApp->font();
    QFontMetrics fm(font);
    QString accels;
    accels = m_info->accels;
    accels = accels.replace("Control", "Ctrl");
    accels = accels.replace("<", "");
    accels = accels.replace(">", "-");
    QStringList splits = accels.split("-");
    int right = this->rect().right() - 2;
    for(int i = splits.count() - 1; i>=0; --i)
    {
        QString str = splits.at(i);
        int w = fm.width(str);
        int h = (height() - fm.height()-6)/2;
        QRect r(0,0,w+8,height());
        r.moveRight(right);
        right = right - w - 12;
        painter.setBrush(palette().color(QPalette::Window));
        painter.drawRoundRect(r.adjusted(0,h,0,-h));
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(m_contain ? 1.0 : 0.6);
        painter.drawText(r,Qt::AlignCenter,str);
        painter.restore();
        if(i == 0)
        {
            m_rect = QRect(r.topLeft(),this->rect().bottomRight());
            m_rect = m_rect.adjusted(0,h,0,-h);
        }
    }
}

void ShortcutItem::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);

    if(!m_shortcutEdit->isVisible() && m_rect.contains(e->pos()))
    {
        QRect r = QRect(0,m_rect.y(),width()/2,m_rect.height());
        r.moveRight(m_rect.right());
        m_shortcutEdit->setGeometry(r);
        m_shortcutEdit->clear();
        m_inter->GrabScreen();
        m_shortcutEdit->setFocus();
        m_shortcutEdit->show();
    }
    else
    {
        m_shortcutEdit->hide();
    }
    update();
}

void ShortcutItem::mouseMoveEvent(QMouseEvent *e)
{
    m_contain = m_rect.contains(e->pos());
    update();
}

void ShortcutItem::leaveEvent(QEvent *)
{
    m_contain = false;

    update();
}
