/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "timezonemodule.h"
#include "datetimemodel.h"
#include "datetimeworker.h"
#include "widgetmodule.h"
#include "timezoneitem.h"
#include "timezonechooser.h"
#include "widgets/titlelabel.h"
#include "widgets/settingshead.h"
#include "widgets/settingsgroup.h"

#include <DCommandLinkButton>

#include <QApplication>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

TimezoneModule::TimezoneModule(DatetimeModel *model, DatetimeWorker *work, QObject *parent)
    : ModuleObject("Timezone", tr("Timezone"), parent)
    , m_model(model)
    , m_work(work)
    , m_timezoneGroup(nullptr)
{
    deactive();
    setChildType(ModuleObject::ChildType::Page);
    connect(this, &TimezoneModule::requestRemoveUserTimeZone, m_work, &DatetimeWorker::removeUserTimeZone);

    appendChild(new WidgetModule<SettingsHead>("System Timezone", tr("System Timezone"), [this](SettingsHead *w) {
        w->setTitle(tr("System Timezone"));
        w->removeBackground();
        connect(w, &SettingsHead::editChanged, this, [this, w](bool) {
            w->blockSignals(true);
            w->toCancel();
            w->blockSignals(false);
            ensureZoneChooserDialog(true);
        });
    }));
    appendChild(new WidgetModule<TimezoneItem>("System Timezone", tr("System Timezone"), [this](TimezoneItem *w) {
        w->setTimeZone(m_model->currentSystemTimeZone());
        connect(m_model, &DatetimeModel::currentSystemTimeZoneChanged, w, &TimezoneItem::setTimeZone);
    }));
    appendChild(new WidgetModule<SettingsHead>("TimezoneList", tr("Timezone List"), [this](SettingsHead *w) {
        w->setTitle(tr("Timezone List"));
        connect(w, &SettingsHead::editChanged, this, &TimezoneModule::onEditClicked);
        connect(this, &TimezoneModule::exitEdit, w, &SettingsHead::toCancel);
    }));
    appendChild(new WidgetModule<SettingsGroup>("TimezoneList", tr("Timezone List"), this, &TimezoneModule::initTimezoneListGroup));
}

void TimezoneModule::initTimezoneListGroup(DCC_NAMESPACE::SettingsGroup *timezoneGroup)
{
    m_timezoneGroup = timezoneGroup;

    SettingsItem *item = new SettingsItem;
    item->addBackground();
    QVBoxLayout *layout = new QVBoxLayout;
    DCommandLinkButton *m_addTimezoneButton = new DCommandLinkButton(tr("Add Timezone ..."), m_timezoneGroup);
    m_addTimezoneButton->setAccessibleName(tr("Add Timezone ..."));
    layout->addWidget(m_addTimezoneButton, 0, Qt::AlignLeft);
    item->setLayout(layout);
    m_timezoneGroup->insertWidget(item);
    m_timezoneGroup->setSpacing(8);

    auto updateZones = [this]() {
        for (int i = m_timezoneGroup->itemCount() - 2; i >= 0; --i) {
            m_timezoneGroup->removeItem(m_timezoneGroup->getItem(i));
        }
        const QList<ZoneInfo> &userTimeZones = m_model->userTimeZones();
        for (const ZoneInfo &zoneInfo : userTimeZones) {
            TimezoneItem *timezoneitem = new TimezoneItem;
            timezoneitem->setTimeZone(zoneInfo);
            connect(timezoneitem, &TimezoneItem::removeClicked, [this, timezoneitem] {
                timezoneitem->setVisible(false);
                Q_EMIT requestRemoveUserTimeZone(timezoneitem->timeZone());
            });
            if (zoneInfo.getZoneName() != QTimeZone::systemTimeZoneId()) {
                m_timezoneGroup->insertItem(m_timezoneGroup->itemCount() - 1, timezoneitem);
            }
        }
        Q_EMIT exitEdit();
    };
    updateZones();
    connect(m_model, &DatetimeModel::systemTimeZoneIdChanged, m_timezoneGroup, updateZones);
    connect(m_model, &DatetimeModel::userTimeZoneAdded, m_timezoneGroup, updateZones);
    connect(m_model, &DatetimeModel::userTimeZoneRemoved, m_timezoneGroup, updateZones);

    connect(m_addTimezoneButton, &QPushButton::clicked, this, &TimezoneModule::ensureZoneChooserDialog);
}

void TimezoneModule::ensureZoneChooserDialog(bool setZone)
{
    TimeZoneChooser *zoneChooserdialog = new TimeZoneChooser(qApp->activeWindow());
    zoneChooserdialog->setAttribute(Qt::WA_DeleteOnClose);
    zoneChooserdialog->setIsAddZone(!setZone);
    if (setZone) {
        connect(zoneChooserdialog, &TimeZoneChooser::confirmed, m_work, &DatetimeWorker::setTimezone);
    } else {
        connect(zoneChooserdialog, &TimeZoneChooser::confirmed, m_work, &DatetimeWorker::addUserTimeZone);
    }
    Q_EMIT exitEdit();
    zoneChooserdialog->exec();
}

void TimezoneModule::onEditClicked(const bool &edit)
{
    for (int i = m_timezoneGroup->itemCount() - 2; i >= 0; --i) {
        TimezoneItem *item = qobject_cast<TimezoneItem *>(m_timezoneGroup->getItem(i));
        if (edit) {
            item->toRemoveMode();
        } else {
            item->toNormalMode();
        }
    }
}
