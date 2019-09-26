/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mirrorswidget.h"
#include "widgets/translucentframe.h"
#include "modules/update/updatemodel.h"
#include "modules/update/mirroritem.h"
#include "widgets/settingsgroup.h"
#include "loadingitem.h"
#include "window/utils.h"

#include <QPushButton>
#include <QVBoxLayout>

using namespace dcc;
using namespace dcc::update;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

MirrorsWidget::MirrorsWidget(UpdateModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_curItem(nullptr)
    , m_testProgress(NotStarted)
    , m_testButton(new QPushButton)
    , m_mirrorListGroup(new SettingsGroup)
    , m_layout(new QVBoxLayout)
{
    setTitle(tr("Mirror List"));

    //~ contents_path /update/Update Settings/Mirror List
    m_testButton->setText(tr("Test Speed"));

    TranslucentFrame *widget = new TranslucentFrame();

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setMargin(0);
    mainlayout->setSpacing(0);
    mainlayout->addSpacing(10);

    QLabel *title = new QLabel;
    QFont font;
    font.setPointSizeF(16);
    title->setFont(font);
    //~ contents_path /update/Update Settings/Mirror List
    title->setText(tr("Mirror List"));

    m_testButton->setFixedSize(120, 48);
    m_testButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    m_layout->addWidget(title, 0, Qt::AlignHCenter);
    m_layout->addSpacing(5);
    m_layout->addWidget(m_testButton, 0, Qt::AlignCenter);
    m_layout->addSpacing(5);
    m_layout->addWidget(m_mirrorListGroup);

    m_mirrorListGroup->setSpacing(List_Interval);
    m_mirrorListGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    mainlayout->addLayout(m_layout);

    widget->setLayout(mainlayout);
    setContent(widget);

    setModel(model);

    connect(m_testButton, &QPushButton::clicked,
            this, &MirrorsWidget::testButtonClicked);
}

void MirrorsWidget::setModel(UpdateModel *model)
{
    if (!model->mirrorInfos().isEmpty()) {
        setDefaultMirror(model->defaultMirror());
    }

    setMirrorInfoList(model->mirrorInfos());

    connect(model, &UpdateModel::defaultMirrorChanged, this, &MirrorsWidget::setDefaultMirror);
    connect(model, &UpdateModel::mirrorSpeedInfoAvaiable, this, &MirrorsWidget::onSpeedInfoAvailable);
    connect(model, &UpdateModel::netselectExistChanged, m_testButton, &QPushButton::setVisible);
    m_testButton->setVisible(model->netselectExist());
}

//close the window can do it
void MirrorsWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)

    qDebug() << Q_FUNC_INFO;
}

void MirrorsWidget::setDefaultMirror(const MirrorInfo &mirror)
{
    if (mirror.m_id != m_defaultMirror.m_id) {
        m_defaultMirror = mirror;

        QList<MirrorItem *> items = findChildren<MirrorItem *>();
        for (MirrorItem *item : items) {
            if (item->mirrorInfo().m_id == m_defaultMirror.m_id) {
                blockSignals(true);
                setCurItem(item);
                blockSignals(false);
            }
        }
    }
}

void MirrorsWidget::setMirrorInfoList(const MirrorInfoList &list)
{
    QList<MirrorInfo>::const_iterator it = list.begin();
    for (; it != list.end(); ++it) {
        MirrorItem *item = new MirrorItem;
        item->setMirrorInfo((*it));

        if ((*it).m_id == m_defaultMirror.m_id) {
            item->setSelected(true);
            m_curItem = item;
        }
        m_mirrorListGroup->appendItem(item);

        connect(item, &MirrorItem::clicked, this, &MirrorsWidget::setCurItem);
    }
}

void MirrorsWidget::setCurItem(MirrorItem *item)
{
    if (item) {
        if (m_curItem) {
            m_curItem->setSelected(false);
        }
        item->setSelected(true);
        m_curItem = item;

        MirrorInfo info = item->mirrorInfo();
        Q_EMIT requestSetDefaultMirror(info);
    }
}

void MirrorsWidget::onSpeedInfoAvailable(const QMap<QString, int> &info)
{
    m_testProgress = Done;
    //~ contents_path /update/Update Settings/Mirror List
    m_testButton->setText(tr("Retest"));

    QList<MirrorItem *> items = findChildren<MirrorItem *>();
    for (MirrorItem *item : items) {
        const QString id = item->mirrorInfo().m_id;

        if (info.contains(id))
            item->setSpeed(info.value(id, -1));
    }

    if (info.keys().length() == items.length()) {
        sortMirrorsBySpeed();
    }
}

void MirrorsWidget::testButtonClicked()
{
    if (m_testProgress == Running)
        return;

    Q_EMIT requestTestMirrorSpeed();

    m_testProgress = Running;

    QList<MirrorItem *> items = findChildren<MirrorItem *>();
    for (MirrorItem *item : items) {
        item->setTesting();
    }
}

void MirrorsWidget::sortMirrorsBySpeed()
{
    QList<MirrorItem *> items = findChildren<MirrorItem *>();
    qSort(items.begin(), items.end(), [](const MirrorItem * one, const MirrorItem * two) {
        return one->speed() > two->speed();
    });

    for (MirrorItem *item : items) {
        m_mirrorListGroup->moveItem(item, 0);
    }
}
