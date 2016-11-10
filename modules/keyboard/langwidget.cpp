#include "langwidget.h"
#include "settingsgroup.h"
#include "settingsitem.h"
#include "indexdelegate.h"
#include "indexframe.h"
#include "indexview.h"
#include "indexmodel.h"

#include <QVBoxLayout>

using namespace dcc;

LangWidget::LangWidget(QWidget *parent)
    :ContentWidget(parent)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();

    SettingsGroup* group = new SettingsGroup();
    SettingsItem* indexItem = new SettingsItem();
//    indexItem->setPalette(QPalette(QColor(19, 89, 177)));

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->setSpacing(0);

    m_model = new IndexModel();
    m_view = new IndexView();

    m_delegate = new IndexDelegate();

    m_view->setFixedHeight(1000);

    hlayout->addWidget(m_view);
    indexItem->setLayout(hlayout);

    group->appendItem(indexItem);
    layout->addWidget(group);
    widget->setLayout(layout);

    setContent(widget);
}

void LangWidget::setModelData(const QList<MetaData> &datas)
{
    m_model->setMetaData(datas);
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
}
