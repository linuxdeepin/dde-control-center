#include "langwidget.h"
#include "settingsgroup.h"
#include "settingsitem.h"
#include "indexdelegate.h"
#include "indexframe.h"
#include "indexview.h"
#include "indexmodel.h"
#include "translucentframe.h"

#include <QVBoxLayout>
#include <QLineEdit>

using namespace dcc;

LangWidget::LangWidget(QWidget *parent)
    :ContentWidget(parent)
{
    TranslucentFrame* widget = new TranslucentFrame();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);

    SettingsGroup* group = new SettingsGroup();
    group->setMargin(3,0,0,0);
    SettingsItem* indexItem = new SettingsItem();
//    indexItem->setPalette(QPalette(QColor(19, 89, 177)));

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->setSpacing(0);

    m_model = new IndexModel();
    m_searchModel = new IndexModel();
    m_view = new IndexView();

    m_delegate = new IndexDelegate();

    hlayout->addWidget(m_view);
    indexItem->setLayout(hlayout);

    group->appendItem(indexItem);
    m_search = new SearchInput();
    layout->addWidget(m_search);
    layout->addWidget(group);
    widget->setLayout(layout);

    setContent(widget);

    connect(m_search, SIGNAL(textChanged(QString)), this, SLOT(onSearch(QString)));
    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SIGNAL(click(QModelIndex)));
}

void LangWidget::setModelData(const QList<MetaData> &datas)
{
    m_model->setMetaData(datas);
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);

    m_view->setFixedHeight(datas.count()*36);
}

void LangWidget::onSearch(const QString &text)
{
    if(text.length() == 0)
    {
        m_view->setModel(m_model);
    }
    else
    {
        QList<MetaData> datas = m_model->metaData();
        QList<MetaData>::iterator it = datas.begin();
        QList<MetaData> sdatas;
        for(; it != datas.end(); ++it)
        {
            if((*it).text().contains(text, Qt::CaseInsensitive))
            {
                sdatas.append(*it);
            }
        }
        m_searchModel->setMetaData(sdatas);
        m_view->setModel(m_searchModel);
    }
}
