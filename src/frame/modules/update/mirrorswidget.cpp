/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include <QPushButton>
#include <QVBoxLayout>

#include "updatemodel.h"
#include "loadingitem.h"

namespace dcc{
namespace update{

MirrorsWidget::MirrorsWidget(UpdateModel *model, QWidget *parent)
    :ContentWidget(parent),
      m_curItem(nullptr),
      m_testProgress(NotStarted),
      m_testButton(new QPushButton(tr("Test Speed"))),
      m_mirrorListGroup(new SettingsGroup),
      m_layout(new QVBoxLayout)
{
    setTitle(tr("Switch Mirror"));

    TranslucentFrame* widget = new TranslucentFrame();

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setMargin(0);
    mainlayout->setSpacing(0);
    mainlayout->addSpacing(10);

    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_testButton);
    m_layout->addSpacing(5);
    m_layout->addWidget(m_mirrorListGroup);

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
    m_testButton->setVisible(model->netselectExist());

    connect(model, &UpdateModel::defaultMirrorChanged, this, &MirrorsWidget::setDefaultMirror);
    connect(model, &UpdateModel::mirrorSpeedInfoAvaiable, this, &MirrorsWidget::onSpeedInfoAvailable);

    connect(model, &UpdateModel::netselectExistChanged, m_testButton, &QPushButton::setVisible);
}

void MirrorsWidget::setDefaultMirror(const MirrorInfo &mirror)
{
    if (mirror.m_id != m_defaultMirror.m_id) {
        m_defaultMirror = mirror;

        QList<MirrorItem*> items = findChildren<MirrorItem*>();
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
    for(; it != list.end(); ++it)
    {
        MirrorItem* item = new MirrorItem;
        item->setMirrorInfo((*it));

        if((*it).m_id == m_defaultMirror.m_id)
        {
            item->setSelected(true);
            m_curItem = item;
        }
        m_mirrorListGroup->appendItem(item);

        connect(item, SIGNAL(clicked(MirrorItem*)), this, SLOT(setCurItem(MirrorItem*)));
    }
}

void MirrorsWidget::setCurItem(MirrorItem *item)
{
    if(item)
    {
        if(m_curItem)
        {
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
    m_testButton->setText(tr("Retest"));

    QList<MirrorItem*> items = findChildren<MirrorItem*>();
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

    QList<MirrorItem*> items = findChildren<MirrorItem*>();
    for (MirrorItem *item : items) {
        item->setTesting();
    }
}

void MirrorsWidget::sortMirrorsBySpeed()
{
    QList<MirrorItem*> items = findChildren<MirrorItem*>();
    qSort(items.begin(), items.end(), [](const MirrorItem *one, const MirrorItem *two) {
        return one->speed() > two->speed();
    });

    for (MirrorItem *item : items) {
        m_mirrorListGroup->moveItem(item, 0);
    }
}

}
}
