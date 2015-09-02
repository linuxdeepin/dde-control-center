#include <QLabel>
#include <QDebug>

#include <libdui/libdui_global.h>
#include <libdui/dthememanager.h>

#include "bootmenulist.h"

DUI_USE_NAMESPACE

BootMenuList::BootMenuList(QWidget *parent) :
    QFrame(parent),
    m_layout(new QVBoxLayout(this))
{
    D_THEME_INIT_WIDGET(BootMenuList);

    m_layout->setMargin(0);
    m_layout->setSpacing(0);
}

QColor BootMenuList::normalColor() const
{
    return m_normalColor;
}

QColor BootMenuList::enabledColor() const
{
    return m_enabledColor;
}

void BootMenuList::addButtons(const QStringList &listLabels)
{
    foreach (const QString &title, listLabels) {
        QLabel *label = new QLabel(title);
        label->setEnabled(false);
        label->setFixedWidth(210);
        m_layout->addWidget(label);
    }
}

void BootMenuList::checkButtonByIndex(int index)
{
    qDebug()<<index;

    QWidget *w = m_layout->itemAt(index)->widget();
    if(w){
        for(int i=0;i<m_layout->count();++i){
            QWidget *ww = m_layout->itemAt(i)->widget();
            if(ww)
                ww->setEnabled(false);
        }
        w->setEnabled(true);
    }
}

void BootMenuList::setNormalColor(QColor normalColor)
{
    if (m_normalColor == normalColor)
        return;

    QString str = styleSheet();
    str.replace(QString("QLabel{color: %1}").arg(m_normalColor.toRgb().name()),
                QString("QLabel{color: %1}").arg(normalColor.toRgb().name()));
    setStyleSheet(str);

    m_normalColor = normalColor;
    emit normalColorChanged(normalColor);
}

void BootMenuList::setEnabledColor(QColor enabledColor)
{
    if(m_enabledColor == enabledColor)
        return;

    QString str = styleSheet();
    str.replace(QString("QLabel:enabled{color: %1}").arg(m_enabledColor.toRgb().name()),
                QString("QLabel:enabled{color: %1}").arg(enabledColor.toRgb().name()));
    setStyleSheet(str);

    m_enabledColor = enabledColor;
    emit enabledColorChanged(enabledColor);
}
