#include "langwidget.h"
#include "settingsgroup.h"
#include "settingsitem.h"
#include "indexdelegate.h"
#include "indexframe.h"
#include "indexview.h"
#include "indexmodel.h"
#include "keyboardmodel.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QDebug>
#include <QEvent>

using namespace dcc;

namespace dcc {
namespace keyboard{
LangWidget::LangWidget(KeyboardModel *model, QWidget *parent)
    :ContentWidget(parent),
      m_keyboardModel(model)
{
    setTitle(tr("System Language"));

    m_contentWidget = new TranslucentFrame();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    m_searchModel = new IndexModel();

    m_model = new IndexModel();
    m_view = new IndexView();
    m_delegate = new IndexDelegate();

    m_search = new SearchInput();
    m_contentTopLayout->addSpacing(10);
    m_contentTopLayout->addWidget(m_search);
    m_contentTopLayout->addSpacing(10);

    layout->addWidget(m_view);
    m_contentWidget->setLayout(layout);

    // 矩形圆角
    m_clipEffectWidget = new DGraphicsClipEffect(m_contentWidget);
    m_contentWidget->installEventFilter(this);
    m_contentWidget->setGraphicsEffect(m_clipEffectWidget);

    setContent(m_contentWidget);
    m_contentWidget->setAttribute(Qt::WA_TranslucentBackground);
    m_contentWidget->setFixedWidth(344);

    connect(m_search, SIGNAL(textChanged(QString)), this, SLOT(onSearch(QString)));
    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SIGNAL(click(QModelIndex)));

    connect(m_keyboardModel, &KeyboardModel::langChanged, this, &LangWidget::setModelData);
    connect(m_keyboardModel, &KeyboardModel::curLangChanged, this, &LangWidget::setCurLang);

    setModelData(m_keyboardModel->langLists());
    setCurLang(m_keyboardModel->curLang());
}

void LangWidget::setModelData(const QList<MetaData> &datas)
{
    m_datas = datas;
    m_model->setMetaData(datas);
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
}

void LangWidget::setCurLang(const QString &lang)
{
    QList<MetaData>::const_iterator it = m_datas.begin();
    int index = 0;
    for(; it != m_datas.end(); ++it)
    {
        if((*it).text() == lang)
        {
            break;
        }
        index++;
    }

    m_view->setCurrentIndex(m_model->index(index));
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

bool LangWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)

    if (event->type() != QEvent::Move && event->type() != QEvent::Resize)
        return false;

    QRect rect = m_contentWidget->rect();

    rect.moveTopLeft(-m_contentWidget->pos());
    rect.setHeight(m_contentWidget->window()->height() - m_contentWidget->mapTo(window(), rect.topLeft()).y());

    QPainterPath path;
    path.addRoundedRect(rect, 5, 5);
    m_clipEffectWidget->setClipPath(path);

    return false;
}
}
}
