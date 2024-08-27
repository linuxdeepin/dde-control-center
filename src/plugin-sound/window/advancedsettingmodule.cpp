// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "advancedsettingmodule.h"

#include "widgets/dcclistview.h"
#include "widgets/widgetmodule.h"

#include <dtiplabel.h>

#include <QStandardItem>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

AdvancedSettingModule::AdvancedSettingModule(SoundModel *model, SoundWorker *work, QObject *parent)
    : PageModule("advancedSetting", tr("Advanced Setting"), parent)
    , m_model(model)
    , m_work(work)
{
    initUI();
}

AdvancedSettingModule::~AdvancedSettingModule() { }

void AdvancedSettingModule::deactive() { }

void AdvancedSettingModule::initUI()
{
    // 音频选项标题
    appendChild(new ItemModule("audioFrameworkTitle", tr("Audio Framework")));

    // 音频框架列表
    m_audioItemModel = new QStandardItemModel(this);
    DStandardItem *m_pulseAudio = new DStandardItem("PulseAudio");
    m_pulseAudio->setData("PulseAudio", Dtk::UserRole);
    m_audioItemModel->appendRow(m_pulseAudio);

    DStandardItem *m_pipeWire = new DStandardItem("PipeWire");
    m_pipeWire->setData("PipeWire", Dtk::UserRole);
    m_audioItemModel->appendRow(m_pipeWire);

    m_audioListModule = new ItemModule(
            "selectFramework",
            QString(),
            [this](ModuleObject *) -> QWidget * {
                DCCListView *audiorListView = new DCCListView();
                audiorListView->setModel(m_audioItemModel);
                setAudioServerByName(m_model->audioServer());

                connect(audiorListView,
                        &DListView::clicked,
                        this,
                        &AdvancedSettingModule::onAudioServerChecked);

                return audiorListView;
            },
            false);
    m_audioListModule->setEnabled(m_model->audioServerChangedState());
    // 等待切换结束的信号，才允许操作
    connect(m_model,
            &SoundModel::onSetAudioServerFinish,
            m_audioListModule,
            &ItemModule::setEnabled);
    appendChild(m_audioListModule);

    // 音频框架提示
    appendChild(new WidgetModule<DTipLabel>("framework", QString(), [](DTipLabel *dTipLabel) {
        dTipLabel->setWordWrap(true);
        dTipLabel->setAlignment(Qt::AlignLeft);
        dTipLabel->setContentsMargins(10, 0, 10, 0);
        dTipLabel->setText(
                tr("Different audio frameworks have their own advantages and disadvantages, and "
                   "you can choose the one that best matches you to use"));
    }));
}

void AdvancedSettingModule::setAudioServerByName(const QString &curAudioServer)
{
    qDebug() << "current AudioFrame is " << curAudioServer;
    int row_count = m_audioItemModel->rowCount();
    for (int i = 0; i < row_count; i++) {
        QStandardItem *item = m_audioItemModel->item(i, 0);
        if (item && (item->text().toLower() == curAudioServer)) {
            item->setCheckState(Qt::Checked);
        } else if (item) { // 如果不加此判断，item会出现空指针
            item->setCheckState(Qt::Unchecked);
        }
    }
}

void AdvancedSettingModule::onAudioServerChecked(const QModelIndex &index)
{
    int row_count = m_audioItemModel->rowCount();
    for (int i = 0; i < row_count; i++) {
        QStandardItem *item = m_audioItemModel->item(i, 0);
        if (item && (index.row() == i)) {
            if (item->checkState() != Qt::Checked) {
                // 被点击时设置为不可选中状态，直到切换结束
                m_audioListModule->setDisabled(true);
                qDebug() << "switch AudioFrame " << item->text();
                item->setCheckState(Qt::Checked);
                Q_EMIT setCurAudioServer(item->text().toLower());
            }
        } else if (item) { // 如果不加此判断，item会出现空指针
            item->setCheckState(Qt::Unchecked);
        }
    }
}
