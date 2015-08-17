#include "selectdialog.h"

SelectDialog::SelectDialog(QWidget *parent):
    QMessageBox(parent)
{
    m_widget = this;
}

QStringList SelectDialog::keyWords() const
{
    return QStringList();
}

void SelectDialog::setData(const QVariant &datas)
{
    if(datas.type() == QVariant::String){
        setText(datas.toString());
    }
}

QVariant SelectDialog::getData()
{
    return text();
}

QWidget *SelectDialog::widget() const
{
    return m_widget;
}

