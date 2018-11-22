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

#include "defappviewer.h"
#include "defcategorywidget.h"
#include "widgets/switchwidget.h"
#include "defappmodel.h"
#include "widgets/translucentframe.h"
#include "widgets/settingsgroup.h"

using namespace dcc;
using namespace dcc::defapp;
using namespace dcc::widgets;

DefAppViewer::DefAppViewer(QWidget *parent)
    : ContentWidget(parent)
{
    QWidget *m_defAppViewer = defappDetail();
    setTitle(tr("Default Applications"));
    m_defAppViewer->setObjectName("DefAppViewer");
    setContent(m_defAppViewer);
}

//下级菜单，该函数会初始化界面
QWidget *DefAppViewer::defappDetail()
{
    QVBoxLayout *mainlayout = new QVBoxLayout;
    QWidget     *mainwidget = new QWidget;
    mainwidget->setLayout(mainlayout);
    mainlayout->setMargin(0);
    mainlayout->setSpacing(20);

    m_modBrowser = new DefCategoryWidget(tr("Webpage"), this);
    m_modMail = new DefCategoryWidget(tr("Mail"), this);
    m_modText = new DefCategoryWidget(tr("Text"), this);
    m_modMusic = new DefCategoryWidget(tr("Music"), this);
    m_modVideo = new DefCategoryWidget(tr("Video"), this);
    m_modPicture = new DefCategoryWidget(tr("Picture"), this);
    m_modTerminal = new DefCategoryWidget(tr("Terminal"), this);

    mainlayout->addSpacing(10);
    mainlayout->addWidget(m_modBrowser);
    mainlayout->addWidget(m_modMail);
    mainlayout->addWidget(m_modText);
    mainlayout->addWidget(m_modMusic);
    mainlayout->addWidget(m_modVideo);
    mainlayout->addWidget(m_modPicture);
    mainlayout->addWidget(m_modTerminal);

    QList<DefCategoryWidget*> list;
    list << m_modBrowser << m_modMail << m_modText << m_modMusic << m_modVideo << m_modPicture << m_modTerminal;

    for (DefCategoryWidget *def : list) {
        connect(def, &DefCategoryWidget::requestSetDefaultApp, this, &DefAppViewer::requestSetDefaultApp);
        connect(def, &DefCategoryWidget::requestDelUserApp, this, &DefAppViewer::requestDelUserApp);
        connect(def, &DefCategoryWidget::requestFrameAutoHide, this, &DefAppViewer::requestFrameAutoHide);
        connect(def, &DefCategoryWidget::requestCreateFile, this, &DefAppViewer::requestCreateFile);
    }

    return mainwidget;
}

void DefAppViewer::setModel(DefAppModel *const model)
{
    m_modBrowser->setCategory(model->getModBrowser());
    m_modMail->setCategory(model->getModMail());
    m_modText->setCategory(model->getModText());
    m_modMusic->setCategory(model->getModMusic());
    m_modVideo->setCategory(model->getModVideo());
    m_modPicture->setCategory(model->getModPicture());
    m_modTerminal->setCategory(model->getModTerminal());
}
