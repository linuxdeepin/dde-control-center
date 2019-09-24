/*
 * Copyright (C) 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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
#include "systemlanguagewidget.h"
#include "checkitem.h"
#include "systemlanguagesettingwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/translucentframe.h"
#include "widgets/settingshead.h"
#include "widgets/comboxwidget.h"
#include "modules/keyboard/checkitem.h"
#include "modules/keyboard/keylabel.h"
#include "modules/keyboard/keyboardmodel.h"

#include <DFloatingButton>
#include <DAnchors>

#include <QStringList>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDebug>
#include <QList>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;
using namespace dcc;
using namespace dcc::keyboard;
using namespace dcc::widgets;

SystemLanguageWidget::SystemLanguageWidget(KeyboardModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_model(model)
    , m_settingWidget(nullptr)
{
    TranslucentFrame *content = new TranslucentFrame();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(20);
    layout->setMargin(0);

    m_group = new SettingsGroup();
    m_head = new SettingsHead();
    //~ contents_path /keyboard/Language List
    m_head->setTitle(tr("Language List"));
    m_head->setEditEnable(false);
    m_group->appendItem(m_head);
    layout->addSpacing(10);
    layout->addWidget(m_group);
    content->setLayout(layout);
    setContent(content);

    DFloatingButton *addSystemLanguage = new DFloatingButton(DStyle::SP_IncreaseElement, this);
    DAnchors<DFloatingButton> anchors(addSystemLanguage);
    anchors.setAnchor(Qt::AnchorBottom, this, Qt::AnchorBottom);
    anchors.setBottomMargin(2);
    anchors.setAnchor(Qt::AnchorHorizontalCenter, this, Qt::AnchorHorizontalCenter);

    connect(addSystemLanguage, &DFloatingButton::clicked, this, &SystemLanguageWidget::onSystemLanguageAdded);
    connect(m_head, &SettingsHead::editChanged, this, &SystemLanguageWidget::onEdit);

    connect(m_model, &KeyboardModel::curLocalLangChanged, this, [this](const QStringList &curLocalLang) {
        for (int i = 0; i < curLocalLang.size(); i++) {
            onAddLanguage(curLocalLang[i]);
        }
    });
    connect(m_model, &KeyboardModel::curLangChanged, this, &SystemLanguageWidget::onDefault);
    QStringList localLangList = m_model->localLang();
    for (int i = 0; i < localLangList.size(); i++) {
        onAddLanguage(localLangList[i]);
    }
    onDefault(m_model->curLang());
}

void SystemLanguageWidget::onEdit(bool value)
{
    m_bEdit = value;
}

void SystemLanguageWidget::onAddLanguage(const QString &localeLang)
{
    if (m_localMaps.contains(localeLang))
        return;

    CheckItem *checkItem = new CheckItem();
    connect(m_head, &SettingsHead::editChanged, checkItem, &CheckItem::onEditMode);
    connect(checkItem, &CheckItem::checkedChanged, this, &SystemLanguageWidget::setCurLangChecked);
    connect(checkItem, &CheckItem::destroySelf, this, &SystemLanguageWidget::onRemoveLang);

    checkItem->setTitle(localeLang);
    checkItem->onEditMode(m_bEdit);
    checkItem->setMultipleMode(false);
    m_group->appendItem(checkItem);

    m_localMaps[localeLang] = checkItem;
    m_head->setEditEnable(m_localMaps.size() > 1);
}

void SystemLanguageWidget::setCurLangChecked(const QString &curLang)
{
    for (auto i(m_localMaps.begin()); i != m_localMaps.end(); ++i) {
        CheckItem *item = i.value();
        item->setChecked(item->title() == curLang);
    }
    QString langKey = m_model->langFromText(curLang);
    Q_EMIT setCurLang(langKey);
}

void SystemLanguageWidget::onDefault(const QString &curLang)
{
    qDebug() << "curLang is " << curLang;
    for (auto i(m_localMaps.begin()); i != m_localMaps.end(); ++i) {
        CheckItem *item = i.value();
        item->setChecked(item->title() == curLang);
    }
}

void SystemLanguageWidget::onRemoveLang(CheckItem *item)
{
    if (item) {
        m_group->removeItem(item);
        Q_EMIT delLocalLang(item->title());
        m_localMaps.remove(item->title());
        item->deleteLater();
    }

    if (m_localMaps.size() < 2) {
        m_head->setEditEnable(false);
    }
}
