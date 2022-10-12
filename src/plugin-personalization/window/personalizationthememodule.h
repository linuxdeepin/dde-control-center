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
#ifndef PERSONALIZATIONTHEMEMODULE_H
#define PERSONALIZATIONTHEMEMODULE_H
#include "interface/pagemodule.h"

#include <QJsonObject>

namespace DCC_NAMESPACE {
class DCCListView;
};
class QStandardItemModel;
class QComboBox;

class PersonalizationModel;
class PersonalizationWorker;
class FontModel;

class PersonalizationThemeModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT
public:
    explicit PersonalizationThemeModule(PersonalizationModel *model, PersonalizationWorker *work, QObject *parent = nullptr);

    void active() override;
private Q_SLOTS:
    void onActiveColorClicked();

    void setStandList(const QList<QJsonObject> &list);
    void setMonoList(const QList<QJsonObject> &list);
    void setList(const QList<QJsonObject> &list, QStandardItemModel *model);

    void setIconTheme(QWidget *widget);
    void setCursorTheme(QWidget *widget);

private:
    QWidget *initThemeList(DCC_NAMESPACE::ModuleObject *module);
    QWidget *initThemeSwitch(DCC_NAMESPACE::ModuleObject *module);
    QWidget *initAccentColor(DCC_NAMESPACE::ModuleObject *module);
    QWidget *initIconTheme(DCC_NAMESPACE::ModuleObject *module);
    QWidget *initCursorTheme(DCC_NAMESPACE::ModuleObject *module);
    QWidget *initFontSize(DCC_NAMESPACE::ModuleObject *module);
    QWidget *initStandardFont(DCC_NAMESPACE::ModuleObject *module);
    QWidget *initMonospacedFont(DCC_NAMESPACE::ModuleObject *module);

    void initFontWidget(QComboBox *combox, FontModel *fontModel, QStandardItemModel *model);
    QString getGlobalThemeId(const QString &themeId, QString &mode);

private:
    QStandardItemModel *m_standardModel;
    QStandardItemModel *m_monospacedModel;

    PersonalizationModel *m_model;
    PersonalizationWorker *m_work;

    QMap<QString, QJsonObject> m_jsonMap;
};

#endif // PERSONALIZATIONTHEMEMODULE_H
