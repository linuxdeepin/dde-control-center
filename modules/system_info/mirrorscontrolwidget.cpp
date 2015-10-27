#include "mirrorscontrolwidget.h"
#include "constants.h"
#include "mirroritemwidget.h"

#include <QGridLayout>
#include <QVBoxLayout>

MirrorsControlWidget::MirrorsControlWidget(QWidget *parent)
    : QWidget(parent)
{
    m_updateSwitchBtn = new DSwitchButton;
    m_changeMirrorBtn = new DTextButton(tr("Change"));
    m_cancelBtn = new DTextButton(tr("Cancel"));
    m_cancelBtn->hide();
    m_applyBtn = new DTextButton(tr("Apply"));
    m_applyBtn->hide();
    m_mirrorsList = new QListWidget;
    m_mirrorsList->setFrameStyle(QFrame::NoFrame);
    m_mirrorsList->setFixedWidth(DCC::ModuleContentWidth);
//    m_mirrorsList->setFixedHeight(50);
//    m_mirrorsList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    m_mirrorsList->setStyleSheet(QString("background-color:#252627;"));
    m_mirrorsList->hide();
    m_mirrorItems = new QMap<QListWidgetItem *, MirrorItemWidget *>;

    loadMirrorList();
    m_mirrorsList->setFixedHeight(50 * m_mirrorItems->count());

    QLabel *autoCheckUpdate = new QLabel(tr("Auto check update"));
    autoCheckUpdate->setStyleSheet(QString("color:%1;").arg(DCC::TextNormalColor.name()));
    autoCheckUpdate->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QLabel *updateServer = new QLabel(tr("Update server"));
    updateServer->setStyleSheet(QString("color:%1;").arg(DCC::TextNormalColor.name()));
    updateServer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(autoCheckUpdate, 0, 0);
    gridLayout->addWidget(m_updateSwitchBtn, 0, 1);
    gridLayout->setAlignment(m_updateSwitchBtn, Qt::AlignRight);
    gridLayout->addWidget(updateServer, 1, 0);
    gridLayout->addWidget(m_changeMirrorBtn, 1, 1);
    gridLayout->setSpacing(10);
    gridLayout->setContentsMargins(15, 15, 10, 15);

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addStretch();
    btnsLayout->addWidget(m_cancelBtn);
    btnsLayout->addWidget(m_applyBtn);
    btnsLayout->setSpacing(10);
    btnsLayout->setContentsMargins(0, 10, 10, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(gridLayout);
    mainLayout->addWidget(m_mirrorsList);
    mainLayout->addLayout(btnsLayout);
    mainLayout->addStretch(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 8);

    setLayout(mainLayout);
    setFixedWidth(DCC::ModuleContentWidth);
    setFixedHeight(500);

    connect(m_applyBtn, &DTextButton::clicked, m_cancelBtn, &DTextButton::click);
    connect(m_changeMirrorBtn, &DTextButton::clicked, [this, updateServer] {
        updateServer->hide();
        m_changeMirrorBtn->hide();

        m_mirrorsList->show();
        m_cancelBtn->show();
        m_applyBtn->show();
    });
    connect(m_cancelBtn, &DTextButton::clicked, [this, updateServer] {
        m_mirrorsList->hide();
        m_cancelBtn->hide();
        m_applyBtn->hide();

        updateServer->show();
        m_changeMirrorBtn->show();
    });
}

MirrorsControlWidget::~MirrorsControlWidget()
{
    // TODO: free m_mirrorsItems
}

void MirrorsControlWidget::loadMirrorList()
{
    MirrorItemWidget *mirrorItem = new MirrorItemWidget;
    QListWidgetItem *widgetItem = new QListWidgetItem;

    m_mirrorsList->addItem(widgetItem);
    m_mirrorsList->setItemWidget(widgetItem, mirrorItem);

    m_mirrorItems->insert(widgetItem, mirrorItem);
}

