// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "mirrorswidget.h"
#include "updatemodel.h"
#include "widgets/settingsgroup.h"
#include "mirrorsourceitem.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollBar>

#include <DTitlebar>

using namespace DCC_NAMESPACE::update;
using namespace Dtk::Widget;

MirrorsWidget::MirrorsWidget(UpdateModel *model, QWidget *parent)
    : DAbstractDialog(false, parent)
    , m_testProgress(NotStarted)
    , m_testButton(new QPushButton)
    , m_view(nullptr)
    , m_model(nullptr)
    , m_mirrorSourceNo(0)
    , m_listWidget(new QWidget)
{
    setWindowTitle(tr("Mirror List"));

    m_testButton->setText(tr("Test Speed"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    auto titleBar = new DTitlebar(this);
    titleBar->setFrameStyle(QFrame::NoFrame);
    titleBar->setBackgroundTransparent(true);
    titleBar->setMenuVisible(false);
    titleBar->setIcon(qApp->windowIcon());
    layout->addWidget(titleBar);

    QLabel *title = new QLabel;
    QFont font;
    font.setPointSizeF(16);
    title->setFont(font);
    title->setText(tr("Mirror List"));

    m_testButton->setFixedSize(120, 36);
    m_testButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    layout->addWidget(title, 0, Qt::AlignHCenter);
    layout->addSpacing(5);
    layout->addWidget(m_testButton, 0, Qt::AlignCenter);
    layout->addSpacing(5);

    m_view = new DListView();
    m_model = new QStandardItemModel(this);
    m_view->setBackgroundType(DStyledItemDelegate::BackgroundType::RoundedBackground);
    m_view->setSelectionMode(DListView::NoSelection);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setEditTriggers(QListView::NoEditTriggers);
    m_view->setModel(m_model);
    m_view->setViewportMargins(10, 0, 10, 0);

#ifdef QT_SCROLL_WHEEL_ANI
    QScrollBar *bar = m_view->verticalScrollBar();
    bar->setSingleStep(1);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarSlideAnimationOn);
#endif

    QVBoxLayout *listLayout= new QVBoxLayout;
    listLayout->setContentsMargins(2, 0, 2, 0);
    listLayout->addWidget(m_view);
    m_listWidget->setLayout(listLayout);
    layout->addWidget(m_listWidget);
    setLayout(layout);

    setModel(model);

    connect(m_testButton, &QPushButton::clicked,
            this, &MirrorsWidget::testButtonClicked);
    resize(660, 660);
}

MirrorsWidget::~MirrorsWidget()
{
    //资源销毁，通知module，需将module中的MirrorsWidget对象赋值为空
    Q_EMIT notifyDestroy();
}

void MirrorsWidget::setModel(UpdateModel *model)
{
    m_updateModel = model;
    if (!model->mirrorInfos().isEmpty()) {
        setDefaultMirror(model->defaultMirror());
    }

    setMirrorInfoList(model->mirrorInfos());
    qRegisterMetaType<QMap<QString, int>>("QMap<QString, int>");
    connect(model, &UpdateModel::defaultMirrorChanged, this, &MirrorsWidget::setDefaultMirror);
    connect(model, &UpdateModel::netselectExistChanged, m_testButton, &QPushButton::setVisible);
    m_testButton->setVisible(model->netselectExist());
}

//close the window can do it
void MirrorsWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    disconnect(m_updateModel, &UpdateModel::mirrorSpeedInfoAvailable, this, &MirrorsWidget::onSpeedInfoAvailable);
}

void MirrorsWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    connect(m_updateModel, &UpdateModel::mirrorSpeedInfoAvailable, this, &MirrorsWidget::onSpeedInfoAvailable, Qt::QueuedConnection);
    onSpeedInfoAvailable(m_updateModel->mirrorSpeedInfo());
}

void MirrorsWidget::setDefaultMirror(const MirrorInfo &mirror)
{
    if (mirror.m_id != m_defaultMirror.m_id) {
        m_defaultMirror = mirror;
    }
}

void MirrorsWidget::setMirrorInfoList(const MirrorInfoList &list)
{
    int num = 0;
    QList<MirrorInfo>::const_iterator it = list.begin();
    for (; it != list.end(); ++it) {
        MirrorSourceItem *item = new MirrorSourceItem;

        if ((*it).m_id == m_defaultMirror.m_id) {
            item->setSelected(true);
            m_mirrorSourceNo = num;
        }
        num++;
        item->setMirrorInfo((*it), tr("Untested"));

        m_model->appendRow(item);
    }

    connect(m_view, &DListView::clicked, this, [this](const QModelIndex &index) {
        //将旧的选中item的指针地址取出,转化为(MirrorSourceItem *),赋值setSelected(false),model->setItem更新select的值
        MirrorSourceItem *item = dynamic_cast<MirrorSourceItem *>(m_model->item(m_mirrorSourceNo));
        item->setSelected(false);
        m_model->setItem(m_mirrorSourceNo, item);

        //获取新的值在list里面的位置
        m_mirrorSourceNo = index.row();

        //将新的选中item的指针地址取出,转化为(MirrorSourceItem *),赋值setSelected(true),model->setItem更新select的值
        item = dynamic_cast<MirrorSourceItem *>(m_model->item(m_mirrorSourceNo));
        item->setSelected(true);
        m_model->setItem(m_mirrorSourceNo, item);

        MirrorInfo info = item->mirrorInfo();
        Q_EMIT requestSetDefaultMirror(info);

        m_view->update();
    });
}

void MirrorsWidget::onSpeedInfoAvailable(const QMap<QString, int> &info)
{
    if (info.isEmpty()) {
        return;
    }
    m_testProgress = Done;
    m_testButton->setText(tr("Retest"));
    int count = m_model->rowCount();
    MirrorSourceItem *item;
    for (int i = 0; i < count; i++) {
        item = dynamic_cast<MirrorSourceItem *>(m_model->item(i));
        const QString id = item->mirrorInfo().m_id;

        if (info.contains(id))
            item->setSpeed(info.value(id, -1));
    }

    m_view->update();
}

void MirrorsWidget::testButtonClicked()
{
    if (m_testProgress == Running)
        return;

    Q_EMIT requestTestMirrorSpeed();

    m_testProgress = Running;

    for (int i = 0; i < m_model->rowCount(); i++) {
        dynamic_cast<MirrorSourceItem *>(m_model->item(i))->setTesting();
    }
}

void MirrorsWidget::sortMirrorsBySpeed()
{
    QList<MirrorSourceItem *> items;
    int count = m_model->rowCount();
    for (int i = 0; i < count; i++) {
        items.append(dynamic_cast<MirrorSourceItem *>(m_model->item(i)));
    }
    qSort(items.begin(), items.end(), [](const MirrorSourceItem * one, const MirrorSourceItem * two) {
        return one->speed() > two->speed();
    });
}
