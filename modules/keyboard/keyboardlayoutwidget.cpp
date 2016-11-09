#include "keyboardlayoutwidget.h"
#include "indexdelegate.h"
#include "indexmodel.h"
#include "indexview.h"
#include "indexframe.h"
#include "settingsgroup.h"
#include "settingsitem.h"

#include <QHBoxLayout>

using namespace dcc;

KeyboardLayoutWidget::KeyboardLayoutWidget(QWidget *parent)
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

    m_view->setFixedHeight(600);
    m_indexframe = new IndexFrame();

    hlayout->addWidget(m_view);
    hlayout->addWidget(m_indexframe);
    indexItem->setLayout(hlayout);

    connect(m_indexframe, SIGNAL(click(QString)), m_view, SLOT(onClick(QString)));
    group->appendItem(indexItem);
    layout->addWidget(group);
    widget->setLayout(layout);

    setContent(widget);
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
