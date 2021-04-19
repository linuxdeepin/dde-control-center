/*
 * Copyright (C) 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     liuxing <liuxing@uniontech.com>
 *
 * Maintainer: liuxing <liuxing@uniontech.com>
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

#include "papersettingwidget.h"
#include "wallpaperview.h"
#include "paperdetailwidget.h"
#include "modules/personalization/personalizationmodel.h"
#include "modules/personalization/personalizationwork.h"

#include <DLabel>

#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE
using namespace dcc::personalization;
using namespace DCC_NAMESPACE::personalization;

const int ViewIndex = 0;

PaperSettingWidget::PaperSettingWidget(PersonalizationWork *work, PersonalizationModel *model, QWidget *parent)
    : DWidget(parent)
    , m_mainLayout(new QVBoxLayout(this))
    , m_settingPage(nullptr)
    , m_work(work)
    , m_model(model)
{
    // 壁纸列表主窗口
    QWidget *viewPage = new QWidget;
    QVBoxLayout *viewLayout = new QVBoxLayout(viewPage);

    // 壁纸列表标题
    DLabel *paperTitle = new DLabel(tr("Wallpaper"));
    paperTitle->setAlignment(Qt::AlignCenter);
    viewLayout->addWidget(paperTitle);

    // 壁纸列表
    m_paperView = new WallpaperView;
    QStringList paths = QDir(WallPaperPath).entryList(QStringList() << "*.jpg", QDir::Files);
    m_paperView->resetModel(paths, WallPaperPath, m_model->desktopPaper(), m_model->lockPaper());
    viewLayout->addWidget(m_paperView);

    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->insertWidget(ViewIndex, viewPage);

    initConnect();
}

PaperSettingWidget::~PaperSettingWidget()
{
    if (m_settingPage)
        delete m_settingPage;
}


void PaperSettingWidget::initConnect()
{
    connect(m_paperView, &WallpaperView::clicked, this, &PaperSettingWidget::onItemClicked);
    connect(m_model, &PersonalizationModel::desktopPaperChanged, this, &PaperSettingWidget::updateView);
    connect(m_model, &PersonalizationModel::lockPaperChanged, this, &PaperSettingWidget::updateView);
}

void PaperSettingWidget::onItemClicked(const QModelIndex &index)
{
    if (!m_settingPage) {
        m_settingPage = new PaperDetailWidget();
        m_settingPage->setWindowFlags(Qt::FramelessWindowHint);
        m_settingPage->setWindowState(Qt::WindowFullScreen);
        connect(m_settingPage, &PaperDetailWidget::cancelClicked, this, &PaperSettingWidget::onCancelClicked);
        connect(m_settingPage, &PaperDetailWidget::requestSetDesktop, m_work, &PersonalizationWork::setDesktopWallpaper);
        connect(m_settingPage, &PaperDetailWidget::requestSetLock, m_work, &PersonalizationWork::setLockWallpaper);
        connect(m_settingPage, &PaperDetailWidget::finishSetWallpaper, this, &PaperSettingWidget::requestExit);
    }
    m_settingPage->setPixmapIndex(index.row());
    m_settingPage->show();
}


void PaperSettingWidget::onCancelClicked()
{
    if (m_settingPage)
     m_settingPage->hide();
}

void PaperSettingWidget::updateView()
{
    QStringList paths = QDir(WallPaperPath).entryList(QStringList() << "*.jpg", QDir::Files);
    m_paperView->resetModel(paths, WallPaperPath, m_model->desktopPaper(), m_model->lockPaper());
}
