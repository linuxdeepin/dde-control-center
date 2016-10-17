#include "contentwidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QTimer>

ContentWidget::ContentWidget(QWidget *parent)
    : QWidget(parent),

      m_content(nullptr)
{
    QPushButton *backBtn = new QPushButton;
    backBtn->setText("Back");
    backBtn->setFixedWidth(32);

    m_title = new QLabel;
    m_title->setText("Title");
    m_title->setAlignment(Qt::AlignCenter);

    m_contentArea = new QScrollArea;
    m_contentArea->installEventFilter(this);
    m_contentArea->setFrameStyle(QFrame::NoFrame);
    m_contentArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_contentArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    m_contentArea->setStyleSheet("background-color:red;");

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(backBtn);
    titleLayout->addWidget(m_title);

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addLayout(titleLayout);
    centeralLayout->addWidget(m_contentArea);
    centeralLayout->setMargin(0);
    centeralLayout->setSpacing(0);

    connect(backBtn, &QPushButton::clicked, this, &ContentWidget::back);

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
    m_content->setFixedWidth(m_contentArea->width());
    m_contentArea->setWidget(m_content);

    return lastWidget;
}

bool ContentWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (m_content && watched == m_contentArea && event->type() == QEvent::Resize)
        m_content->setFixedWidth(static_cast<QResizeEvent *>(event)->size().width());

    return false;
}
