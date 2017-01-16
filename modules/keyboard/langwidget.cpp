#include "langwidget.h"
#include "settingsgroup.h"
#include "settingsitem.h"
#include "indexdelegate.h"
#include "indexframe.h"
#include "indexview.h"
#include "indexmodel.h"
#include "translucentframe.h"
#include "keyboardmodel.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QDebug>

using namespace dcc;

namespace dcc {
namespace keyboard{
LangWidget::LangWidget(KeyboardModel *model, QWidget *parent)
    :ContentWidget(parent),
      m_keyboardModel(model)
{
    setTitle(tr("System Language"));

    TranslucentFrame* widget = new TranslucentFrame();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    m_searchModel = new IndexModel();

    m_model = new IndexModel();
    m_view = new IndexView();
    m_delegate = new IndexDelegate();

    m_search = new SearchInput();
    m_contentTopLayout->addWidget(m_search);
    m_contentTopLayout->addSpacing(10);

    layout->addWidget(m_view);
    widget->setLayout(layout);

    setContent(widget);

    connect(m_search, SIGNAL(textChanged(QString)), this, SLOT(onSearch(QString)));
    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SIGNAL(click(QModelIndex)));

    setCurLang(m_keyboardModel->curLang());
    setModelData(m_keyboardModel->langLists());
}

void LangWidget::setModelData(const QList<MetaData> &datas)
{
    QList<MetaData>::const_iterator it = datas.begin();
    int index = 0;
    for(; it != datas.end(); ++it)
    {
        if((*it).text() == m_curLang)
        {
            break;
        }
        index++;
    }
    m_model->setMetaData(datas);
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setFixedHeight(datas.count()*36);

    m_view->setCurrentIndex(m_model->index(index));}

void LangWidget::setCurLang(const QString &lang)
{
    m_curLang = lang;
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
}
}
