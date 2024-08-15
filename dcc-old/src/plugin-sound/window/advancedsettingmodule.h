//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ADVANCEDSETTINGMODULE_H
#define ADVANCEDSETTINGMODULE_H

#include "interface/pagemodule.h"

#include "interface/namespace.h"
#include "soundmodel.h"
#include "itemmodule.h"

#include <DStandardItem>

#include <QWidget>


QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
class ItemModule;
class PageModule;
}

class SoundModel;
class SoundWorker;
/**
 * 该界面类对应 控制中心-声音模块-高级设置 界面
 */

class AdvancedSettingModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT

public:
    explicit AdvancedSettingModule(SoundModel *model, SoundWorker *work, QObject *parent);
    ~AdvancedSettingModule();
    void deactive() override;

Q_SIGNALS:
    void setCurAudioServer(const QString &curAudioServer);  // 设置音频框架时触发信号

public Q_SLOTS:
    void setAudioServerByName(const QString &curAudioServer); // 通过框架名设置选中状态
    void onAudioServerChecked(const QModelIndex &index);      // 当音频框架被选中时触发

private:
    void initUI();

private:
    // model类， 为后端数据来源及数据变化信号来源
    SoundModel *m_model{nullptr};
    SoundWorker *m_work{nullptr};

    // 音频服务选项
    QStandardItemModel *m_audioItemModel{nullptr};
    DCC_NAMESPACE::ItemModule *m_audioListModule{nullptr};

};

#endif // ADVANCEDSETTINGMODULE_H
