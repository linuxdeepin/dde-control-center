#include "shortcutitem.h"
#include "shortcutmodel.h"
#include <QLabel>
#include <QMouseEvent>
#include <QLineEdit>
#include <QApplication>
#include <QPainter>

using namespace dcc;

namespace dcc {
namespace keyboard{

ShortcutItem::ShortcutItem(QFrame *parent)
    :SettingsItem(parent),
      m_contain(false),
      m_display(false),
      m_info(NULL)
{
    setMouseTracking(true);
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(20,0,10,0);
    layout->setSpacing(2);

    m_title = new QLabel();
    m_title->setText("");
    m_title->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_title);
    layout->setAlignment(m_title, Qt::AlignLeft);

    m_editBtn = new DImageButton;
    m_editBtn->setNormalPic(":/keyboard/themes/common/icons/edit_normal.png");
    m_editBtn->setPressPic(":/keyboard/themes/common/icons/edit_press.png");
    m_editBtn->setHoverPic(":/keyboard/themes/common/icons/edit_hover.png");
    m_editBtn->hide();
    layout->addWidget(m_editBtn, 1, Qt::AlignLeft);

    m_shortcutEdit = new QLineEdit(this);
    m_shortcutEdit->setReadOnly(true);
    m_shortcutEdit->hide();

    m_checkBtn = new DImageButton();
    m_checkBtn->setNormalPic(":/keyboard/themes/dark/icons/list_delete_normal.png");
    m_checkBtn->setHoverPic(":/keyboard/themes/dark/icons/list_delete_hover.png");
    m_checkBtn->setPressPic(":/keyboard/themes/dark/icons/list_delete_press.png");

    layout->addStretch();
    layout->addWidget(m_checkBtn);
    layout->setAlignment(m_checkBtn, Qt::AlignVCenter);
    m_checkBtn->hide();

    setLayout(layout);
    setFixedHeight(36);

    m_inter = new KeybingdingInter("com.deepin.daemon.Keybinding",
                                          "/com/deepin/daemon/Keybinding",
                                          QDBusConnection::sessionBus(), this);

    connect(m_inter, &KeybingdingInter::KeyEvent, this, &ShortcutItem::onKeyEvent);
    connect(m_checkBtn, SIGNAL(clicked()), this, SLOT(onChecked()));
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChanged(QWidget*,QWidget*)));
    connect(m_editBtn, &DImageButton::clicked, this, &ShortcutItem::onShortcutEdit);
}

void ShortcutItem::setShortcutInfo(ShortcutInfo *info)
{
    m_info = info;
}

void ShortcutItem::displayConflict(bool display)
{
    m_display = display;
}

ShortcutInfo *ShortcutItem::curInfo()
{
    return m_info;
}

void ShortcutItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void ShortcutItem::onFocusChanged(QWidget *old, QWidget *now)
{
    if(!old && now != m_shortcutEdit)
        m_shortcutEdit->hide();
}

void ShortcutItem::onKeyEvent(bool press, QString shortcut)
{
    if(!press)
    {
        if(!m_shortcutEdit->isVisible())
            return;

        if(shortcut.isEmpty() || shortcut.toLower() == "escape"){
            m_shortcutEdit->hide();
            return;
        }

        if(shortcut == "BackSpace" || shortcut == "delete"){
            m_shortcutEdit->hide();
            emit requestDisableShortcut(m_info);
        }else{
            m_shortcutEdit->hide();
            if(m_info->accels != shortcut)
            {
                bool result = m_inter->CheckAvaliable(shortcut);
                emit shortcutChangd(result, m_info, shortcut);
            }
        }
    }
    else
    {
        if(shortcut.isEmpty())
            m_shortcutEdit->hide();
    }
    update();
}

void ShortcutItem::onEditMode(bool value)
{
    if(value)
    {
        m_checkBtn->show();
        m_editBtn->show();
    }
    else
    {
        m_checkBtn->hide();
        m_editBtn->hide();
    }
    update();
}

void ShortcutItem::onChecked()
{
    emit destroyed();
}

void ShortcutItem::onShortcutEdit()
{
    emit shortcutEditChanged(m_info);
}

void ShortcutItem::paintEvent(QPaintEvent *e)
{
    SettingsItem::paintEvent(e);
    if(m_shortcutEdit->isVisible() || m_checkBtn->isVisible())
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
        if (str == "null" && m_display) {
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
        if(str == "null" && m_display)
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
        m_shortcutEdit->setPlaceholderText(tr("Please enter a new shortcut"));
        m_info->item = this;
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
}
}
