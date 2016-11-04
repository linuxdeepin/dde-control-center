#include "contentwidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QTimer>
#include <QDebug>

#include <dimagebutton.h>
#include <dseparatorhorizontal.h>

DWIDGET_USE_NAMESPACE

ContentWidget::ContentWidget(QWidget *parent)
    : QWidget(parent),

      m_content(nullptr)
{
    DImageButton *backBtn = new DImageButton;
    backBtn->setFixedSize(24, 22);
    backBtn->setNormalPic(":/icon/previous_normal.png");
    backBtn->setHoverPic(":/icon/previous_hover.png");
    backBtn->setPressPic(":/icon/previous_press.png");

    m_title = new QLabel;
    m_title->setAlignment(Qt::AlignCenter);
    m_title->setStyleSheet("color: white;");

    m_contentArea = new QScrollArea;
    m_contentArea->setWidgetResizable(true);
    m_contentArea->installEventFilter(this);
    m_contentArea->setFrameStyle(QFrame::NoFrame);
    m_contentArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_contentArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(backBtn);
    titleLayout->addWidget(m_title);
    titleLayout->addSpacing(backBtn->width());

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addLayout(titleLayout);
    centeralLayout->addSpacing(15);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addSpacing(15);
    centeralLayout->addWidget(m_contentArea);
    centeralLayout->setContentsMargins(8, 8, 8, 0);
    centeralLayout->setSpacing(0);

    connect(backBtn, &DImageButton::clicked, this, &ContentWidget::back);

    setLayout(centeralLayout);
}

void ContentWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}

QWidget *ContentWidget::setContent(QWidget * const w)
{
    QWidget *lastWidget = m_content;

    m_content = w;
    m_content->installEventFilter(this);
    m_content->setFixedWidth(m_contentArea->width());
    m_contentArea->setWidget(m_content);

    if (lastWidget)
        lastWidget->removeEventFilter(this);

    return lastWidget;
}

bool ContentWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (m_content && watched == m_contentArea && event->type() == QEvent::Resize)
        m_content->setFixedWidth(static_cast<QResizeEvent *>(event)->size().width());

    if (watched == m_content && event->type() == QEvent::LayoutRequest)
        m_content->setFixedHeight(m_content->layout()->sizeHint().height());

    return false;
}
