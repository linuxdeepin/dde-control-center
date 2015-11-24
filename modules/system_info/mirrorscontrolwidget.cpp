#include "mirrorscontrolwidget.h"
#include "constants.h"
#include "mirroritemwidget.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPalette>

#include <libdui/dseparatorhorizontal.h>

MirrorsControlWidget::MirrorsControlWidget(QWidget *parent)
    : QWidget(parent)
{
    m_dbusLastoreInter = new DBusLastoreUpdater("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this);
    m_updateSwitchBtn = new DSwitchButton;
    m_updateSwitchBtn->setChecked(m_dbusLastoreInter->autoCheckUpdates());
    m_changeMirrorBtn = new DTextButton(tr("Change"));
    m_applyBtn = new DTextButton(tr("Complete"));
    m_mirrorsList = new DListWidget;
    m_mirrorsList->setEnableVerticalScroll(true);
    m_mirrorsList->setCheckable(true);
//    m_mirrorsList->setItemSize(DCC::ModuleContentWidth, 40);
    m_mirrorsList->setStyleSheet("background-color:#252627;");
    m_mirrorsList->hide();

    loadMirrorList();

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
    gridLayout->setContentsMargins(15, 15, 10, 13);

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addStretch();
    btnsLayout->addWidget(m_applyBtn);
//    btnsLayout->setSpacing(10);
    btnsLayout->setContentsMargins(0, 10, 10, 10);

    QWidget *btnsWidget = new QWidget;
    btnsWidget->setLayout(btnsLayout);
    QPalette btnsPalette(QColor("#252627"));
    btnsWidget->setPalette(btnsPalette);
    btnsWidget->setAutoFillBackground(true);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(gridLayout);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(m_mirrorsList);
    mainLayout->addWidget(btnsWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *internalWidget = new QWidget;
    internalWidget->setLayout(mainLayout);
    QPalette p(QColor("#1a1b1b"));
    internalWidget->setPalette(p);
    internalWidget->setAutoFillBackground(true);

    QVBoxLayout *mainVLayout = new QVBoxLayout;
    mainVLayout->addWidget(internalWidget);
    mainVLayout->addStretch(1);
    mainVLayout->setSpacing(0);
    mainVLayout->setMargin(0);

    setLayout(mainVLayout);
    setFixedWidth(DCC::ModuleContentWidth);

    connect(m_updateSwitchBtn, &DSwitchButton::checkedChanged, m_dbusLastoreInter, &DBusLastoreUpdater::SetAutoCheckUpdates);
    connect(m_applyBtn, &DTextButton::clicked, this, &MirrorsControlWidget::configAccept);
    connect(m_changeMirrorBtn, &DTextButton::clicked, [this, updateServer] {
        updateServer->hide();
        m_changeMirrorBtn->hide();

        m_mirrorsList->show();
    });
    connect(m_applyBtn, &DTextButton::clicked, [this, updateServer] {
        updateServer->show();
        m_changeMirrorBtn->show();

        m_mirrorsList->hide();
    });
}

MirrorsControlWidget::~MirrorsControlWidget()
{
    // TODO: free m_mirrorsItems
}

void MirrorsControlWidget::resizeEvent(QResizeEvent *e)
{
    m_mirrorsList->setMaximumHeight(e->size().height() - 95);
}

void MirrorsControlWidget::loadMirrorList()
{
    // TODO: lang
    MirrorInfoList mirrorList = m_dbusLastoreInter->ListMirrorSources("zh_CN");
    MirrorItemWidget *mirrorItem;

    const QString currentMirror = m_dbusLastoreInter->mirrorSource();

    for (MirrorInfo &mirror : mirrorList)
    {
        mirrorItem = new MirrorItemWidget;

        mirrorItem->setMirrorName(mirror.m_name);

        m_mirrorsList->addWidget(mirrorItem);
        if (mirror.m_id == currentMirror)
            m_mirrorsList->setChecked(m_mirrorsList->count() - 1, true);

        connect(mirrorItem, &MirrorItemWidget::clicked, [this, mirror] {
            m_dbusLastoreInter->SetMirrorSource(mirror.m_id);
        });
    }
}
