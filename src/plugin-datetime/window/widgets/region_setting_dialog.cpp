#include "region_setting_dialog.h"

#include <QRegularExpression>

RegionDialog::RegionDialog(QMap<QString, QString> regions, QWidget *parent)
    : Dtk::Widget::DDialog(parent)
    , m_edit(new QTextEdit(this))
    , m_listWidget(new QListWidget(this))
    , m_regions(regions)
{
    m_listWidget->addItems(regions.keys());
}

void RegionDialog::onFilterChanged(const QString &filter)
{
    auto re = QRegularExpression(filter);
    m_listWidget->clear();
    m_listWidget->addItems(m_regions.keys().filter(re));
}
