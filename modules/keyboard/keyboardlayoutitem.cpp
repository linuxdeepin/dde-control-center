#include <QVariant>

#include <libdui/libdui_global.h>
#include <libdui/dthememanager.h>
#include <libdui/dconstants.h>

#include "keyboardlayoutitem.h"
#include "imagenamebutton.h"

DUI_USE_NAMESPACE

KeyboardLayoutItem::KeyboardLayoutItem(bool showRmButton, QWidget *parent) :
    QFrame(parent),
    m_widget(this),
    m_label(new QLabel),
    m_checked(false),
    m_layout(new QHBoxLayout)
{
    D_THEME_INIT_WIDGET(KeyboardLayoutItem);

    m_deleteButton = new ImageNameButton("delete_multi", this);
    m_deleteButton->setHidden(!showRmButton);

    connect(m_deleteButton, &ImageNameButton::clicked, this, &KeyboardLayoutItem::removeButtonClicked);
    connect(this, &KeyboardLayoutItem::showRemoveButton, [=]{
        if(!m_checked){
            m_deleteButton->show();
        }
    });
    connect(this, &KeyboardLayoutItem::hideRemoveButton, m_deleteButton, &ImageNameButton::hide);

    m_layout->setMargin(0);
    m_layout->setContentsMargins(22, 0, 0, 0);
    m_layout->addWidget(m_label);
    setLayout(m_layout);
}

QStringList KeyboardLayoutItem::keyWords() const
{
    return m_keyWords;
}

void KeyboardLayoutItem::setData(const QVariant &datas)
{
    if(datas.type() == QVariant::String){
        m_label->setText(datas.toString());
    }
}

QVariant KeyboardLayoutItem::getData()
{
    return m_label->text();
}

QWidget *KeyboardLayoutItem::widget() const
{
    return m_widget;
}

bool KeyboardLayoutItem::checked() const
{
    return m_checked;
}

QString KeyboardLayoutItem::title() const
{
    return m_label->text();
}

QString KeyboardLayoutItem::imagePress() const
{
    return m_imagePress;
}

QString KeyboardLayoutItem::imageHover() const
{
    return m_imageHover;
}

QString KeyboardLayoutItem::imageNormal() const
{
    return m_imageNormal;
}

QString KeyboardLayoutItem::imageChecked() const
{
    return m_imageChecked;
}

void KeyboardLayoutItem::setChecked(bool checked)
{
    if (m_checked == checked)
        return;

    m_checked = checked;

    D_THEME_INIT_WIDGET(KeyboardLayoutItem);

    if(!m_imageChecked.isEmpty()&&!m_imageChecked.isEmpty()){
        if(checked){
            m_label->removeEventFilter(this);
            m_label->setPixmap(QPixmap(m_imageChecked));
        }else if(!m_imageNormal.isEmpty()){
            m_label->installEventFilter(this);
            m_label->setPixmap(QPixmap(m_imageNormal));
        }
    }

    emit checkedChanged(checked);
}

void KeyboardLayoutItem::setTitle(QString title)
{
    m_label->setText(title);
}

void KeyboardLayoutItem::setImagePress(QString imagePress)
{
    m_imagePress = imagePress;
}

void KeyboardLayoutItem::setImageHover(QString imageHover)
{
    m_imageHover = imageHover;
}

void KeyboardLayoutItem::setImageNormal(QString imageNormal)
{
    m_imageNormal = imageNormal;
    if(imageNormal.isEmpty()){
        m_label->removeEventFilter(this);
    }else{
        if(!m_checked)
            m_label->setPixmap(QPixmap(imageNormal));
        m_label->installEventFilter(this);
    }
}

void KeyboardLayoutItem::setImageChecked(QString imageChecked)
{
    m_imageChecked = imageChecked;
    if(!imageChecked.isEmpty()&&m_checked){
        m_label->setPixmap(QPixmap(imageChecked));
    }
}

void KeyboardLayoutItem::setKeyWords(QStringList keyWords)
{
    m_keyWords = keyWords;
}

bool KeyboardLayoutItem::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == m_label){
        switch (e->type()) {
        case QEvent::Enter:{
            QPixmap *pixmap = const_cast<QPixmap*>(m_label->pixmap());
            pixmap->load(m_imageHover);
            break;
        }
        case QEvent::Leave:{
            QPixmap *pixmap = const_cast<QPixmap*>(m_label->pixmap());
            pixmap->load(m_imageNormal);
            break;
        }
        case QEvent::MouseButtonPress:{
            QPixmap *pixmap = const_cast<QPixmap*>(m_label->pixmap());
            pixmap->load(m_imagePress);
            break;
        }
        case QEvent::MouseButtonRelease:{
            QPixmap *pixmap = const_cast<QPixmap*>(m_label->pixmap());
            pixmap->load(m_imageHover);
            break;
        }
        default:
            break;
        }
    }

    return false;
}

void KeyboardLayoutItem::resizeEvent(QResizeEvent *e)
{
    m_deleteButton->move(8, height()/2-m_deleteButton->height()/2);

    QFrame::resizeEvent(e);
}

