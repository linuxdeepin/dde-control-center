#include <QVariant>

#include <libdui/libdui_global.h>
#include <libdui/dthememanager.h>
#include <libdui/dconstants.h>

#include "genericlistitem.h"
#include "imagenamebutton.h"

DUI_USE_NAMESPACE

GenericListItem::GenericListItem(bool showRmButton, QWidget *parent) :
    QFrame(parent),
    m_widget(this),
    m_label(new QLabel),
    m_checked(false),
    m_layout(new QHBoxLayout),
    m_showBgColor(false)
{
    D_THEME_INIT_WIDGET(GenericListItem, checked, showBgColor);

    m_deleteButton = new ImageNameButton("list_remove", this);
    m_deleteButton->setHidden(!showRmButton);

    connect(m_deleteButton, &ImageNameButton::clicked, this, &GenericListItem::removeButtonClicked);
    connect(this, &GenericListItem::showRemoveButton, [=]{
        if(!m_checked){
            m_deleteButton->show();
        }
    });
    connect(this, &GenericListItem::hideRemoveButton, m_deleteButton, &ImageNameButton::hide);

    m_layout->setMargin(0);
    m_layout->setContentsMargins(22, 0, 0, 0);
    m_layout->addWidget(m_label);
    setLayout(m_layout);
}

QStringList GenericListItem::keyWords() const
{
    return m_keyWords;
}

void GenericListItem::setData(const QVariant &datas)
{
    if(datas.type() == QVariant::String){
        m_label->setText(datas.toString());
    }
}

QVariant GenericListItem::getData()
{
    return m_label->text();
}

QWidget *GenericListItem::widget() const
{
    return m_widget;
}

void GenericListItem::setListWidget(SearchList *list)
{
    SearchItem::setListWidget(list);

    connect(list, &SearchList::countChanged, this, [&]{
        if(m_list)
            setShowBgColor(m_list->count()>1);
    }, Qt::QueuedConnection);

    if(m_list)
        setShowBgColor(m_list->count()>1);
}

bool GenericListItem::checked() const
{
    return m_checked;
}

QString GenericListItem::title() const
{
    return m_label->text();
}

QString GenericListItem::imagePress() const
{
    return m_imagePress;
}

QString GenericListItem::imageHover() const
{
    return m_imageHover;
}

QString GenericListItem::imageNormal() const
{
    return m_imageNormal;
}

QString GenericListItem::imageChecked() const
{
    return m_imageChecked;
}

bool GenericListItem::showBgColor() const
{
    return m_showBgColor;
}

void GenericListItem::setChecked(bool checked)
{
    if (m_checked == checked)
        return;

    m_checked = checked;

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

void GenericListItem::setTitle(QString title)
{
    m_label->setText(title);
}

void GenericListItem::setImagePress(QString imagePress)
{
    m_imagePress = imagePress;
}

void GenericListItem::setImageHover(QString imageHover)
{
    m_imageHover = imageHover;
}

void GenericListItem::setImageNormal(QString imageNormal)
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

void GenericListItem::setImageChecked(QString imageChecked)
{
    m_imageChecked = imageChecked;
    if(!imageChecked.isEmpty()&&m_checked){
        m_label->setPixmap(QPixmap(imageChecked));
    }
}

void GenericListItem::setKeyWords(QStringList keyWords)
{
    m_keyWords = keyWords;
}

void GenericListItem::setShowBgColor(bool showBgColor)
{
    if (m_showBgColor == showBgColor)
        return;

    m_showBgColor = showBgColor;

    emit showBgColorChanged(showBgColor);
}

bool GenericListItem::eventFilter(QObject *obj, QEvent *e)
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

void GenericListItem::resizeEvent(QResizeEvent *e)
{
    m_deleteButton->move(8, height()/2-m_deleteButton->height()/2);

    QFrame::resizeEvent(e);
}

bool GenericListItem::event(QEvent *e)
{
    if(e->type() == QEvent::MouseButtonRelease) {
        emit clicked();
    } else if(e->type() == QEvent::Enter) {
        emit mouseEnter();
    } else if(e->type() == QEvent::Leave) {
        emit mouseLeave();
    }

    return QFrame::event(e);
}

