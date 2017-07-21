#include "keyboardlayoutwidget.h"
#include "indexdelegate.h"
#include "indexmodel.h"
#include "indexview.h"
#include "indexframe.h"
#include "settingsgroup.h"
#include "settingsitem.h"
#include "translucentframe.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QEvent>
#include <QLocale>

using namespace dcc;

namespace dcc {
namespace keyboard{


KeyboardLayoutWidget::KeyboardLayoutWidget(QWidget *parent)
    :ContentWidget(parent),
      textLength(0)
{
    setTitle(tr("Add Keyboard Layout"));

    m_mainWidget = new TranslucentFrame();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->setSpacing(0);

    m_searchModel = new IndexModel();

    m_model = new IndexModel();
    m_view = new IndexView();
    m_delegate = new IndexDelegate();

    m_indexframe = nullptr;

    hlayout->addWidget(m_view);

    QLocale locale;
    if (locale.language() == QLocale::Chinese) {
        m_indexframe = new IndexFrame();
        hlayout->addWidget(m_indexframe);
        connect(m_indexframe, SIGNAL(click(QString)), m_view, SLOT(onClick(QString)));
    }

    m_search = new SearchInput();
    m_contentTopLayout->addSpacing(10);
    m_contentTopLayout->addWidget(m_search);
    m_contentTopLayout->addSpacing(10);

    m_mainWidget->setLayout(hlayout);


    m_clipEffectWidget = new DGraphicsClipEffect(m_mainWidget);
    m_mainWidget->installEventFilter(this);
    m_mainWidget->setGraphicsEffect(m_clipEffectWidget);

    setContent(m_mainWidget);

    m_mainWidget->setAttribute(Qt::WA_TranslucentBackground);
    m_mainWidget->setFixedWidth(344);

    connect(m_search, SIGNAL(textChanged(QString)), this, SLOT(onSearch(QString)));

    connect(m_view, &IndexView::clicked, this, &KeyboardLayoutWidget::onItemClicked);
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
    QLocale locale;
    if (locale.language() == QLocale::Chinese) {
        m_model->setLetters(letters);
        m_indexframe->setLetters(letters);
    }
}

void KeyboardLayoutWidget::onSearch(const QString &text)
{
    if(text.length() == 0)
    {
        m_view->setModel(m_model);
        if (m_indexframe)
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
        if (m_indexframe)
            m_indexframe->hide();
    }
}

void KeyboardLayoutWidget::onItemClicked(const QModelIndex &index)
{
    QVariant var = index.data();
    MetaData md = var.value<MetaData>();

    if (m_model->letters().contains(md.text()))
        return;

    emit layoutSelected(md.text());
    emit back();
}

bool KeyboardLayoutWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)

    if (event->type() != QEvent::Move && event->type() != QEvent::Resize)
        return false;

    QRect rect = m_mainWidget->rect();

    rect.moveTopLeft(-m_mainWidget->pos());
    rect.setHeight(m_mainWidget->window()->height() - m_mainWidget->mapTo(window(), rect.topLeft()).y());

    QPainterPath path;
    path.addRoundedRect(rect, 5, 5);
    m_clipEffectWidget->setClipPath(path);

    return false;
}

}
}
