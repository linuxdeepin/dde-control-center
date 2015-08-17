#include "tooltip.h"

ToolTip::ToolTip(QWidget *parent) :
    QLabel(parent)
{
    m_widget = this;

    setWordWrap(true);
}

QStringList ToolTip::keyWords() const
{
    return QStringList();
}

void ToolTip::setData(const QVariant &datas)
{
    if(datas.type() == QVariant::String){
        setText(datas.toString());
    }
}

QVariant ToolTip::getData()
{
    return text();
}

QWidget *ToolTip::widget() const
{
    return m_widget;
}

