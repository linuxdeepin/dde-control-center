#include "keyboardlayoutwidget.h"
#include "indexdelegate.h"
#include "indexmodel.h"
#include "indexview.h"
#include "indexframe.h"
#include "settingsgroup.h"
#include "settingsitem.h"

#include <QHBoxLayout>
#include <QLineEdit>

using namespace dcc;

KeyboardLayoutWidget::KeyboardLayoutWidget(QWidget *parent)
    :ContentWidget(parent),
      textLength(0)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);

    SettingsGroup* group = new SettingsGroup();
    SettingsItem* indexItem = new SettingsItem();
//    indexItem->setPalette(QPalette(QColor(19, 89, 177)));

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->setSpacing(0);

    m_model = new IndexModel();
    m_searchModel = new IndexModel();

    m_view = new IndexView();
    m_delegate = new IndexDelegate();

    m_indexframe = new IndexFrame();

    hlayout->addWidget(m_view);
    hlayout->addWidget(m_indexframe);
    indexItem->setLayout(hlayout);

    connect(m_indexframe, SIGNAL(click(QString)), m_view, SLOT(onClick(QString)));
    group->appendItem(indexItem);
    m_search = new SearchInput();
    layout->addWidget(m_search);
    layout->addWidget(group);
    widget->setLayout(layout);

    setContent(widget);

    connect(m_search, SIGNAL(textChanged(QString)), this, SLOT(onSearch(QString)));
}

KeyboardLayoutWidget::~KeyboardLayoutWidget()
{
    m_searchModel->deleteLater();
    m_model->deleteLater();
    m_delegate->deleteLater();
}

void KeyboardLayoutWidget::setMetaData(const QList<MetaData> &datas)
{
    m_model->setMetaData(datas);
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
}

void KeyboardLayoutWidget::setLetters(QList<QString> letters)
{
    m_model->setLetters(letters);
    m_indexframe->setLetters(letters);
}

QList<MetaData> KeyboardLayoutWidget::selectData() const
{
    QModelIndex index = m_view->currentIndex();
    QList<MetaData> datas;

    if(index.isValid())
    {
        QVariant var = index.data();
        MetaData md = var.value<MetaData>();
        datas.append(md);
    }

    return datas;
}

void KeyboardLayoutWidget::onSearch(const QString &text)
{
    if(text.length() == 0)
    {
        m_view->setModel(m_model);
        m_indexframe->show();
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
        m_indexframe->hide();
    }
}
