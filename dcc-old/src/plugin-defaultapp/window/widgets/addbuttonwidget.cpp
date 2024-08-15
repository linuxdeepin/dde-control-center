//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "addbuttonwidget.h"
#include "category.h"
#include "defappmodel.h"

#include <DFloatingButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

AddButtonWidget::AddButtonWidget(DefAppWorker::DefaultAppsCategory category, QWidget *parent)
    : QWidget(parent)
    , m_addBtn(new DFloatingButton(DStyle::SP_IncreaseElement))
    , m_categoryValue(category)
    , m_category(nullptr)
{
    QVBoxLayout *centralLayout = new QVBoxLayout;

    centralLayout->addWidget(m_addBtn, 0, Qt::AlignHCenter | Qt::AlignBottom);
    setLayout(centralLayout);

    connect(m_addBtn, &Dtk::Widget::DFloatingButton::clicked, this, &AddButtonWidget::onAddBtnClicked);
    m_addBtn->setToolTip(tr("Add Application"));

}

AddButtonWidget::~AddButtonWidget()
{

}

void AddButtonWidget::setModel(DefAppModel * const model)
{
    switch (m_categoryValue) {
    case DefAppWorker::Browser:
        setCategory(model->getModBrowser());
        break;
    case DefAppWorker::Mail:
        setCategory(model->getModMail());
        break;
    case DefAppWorker::Text:
        setCategory(model->getModText());
        break;
    case DefAppWorker::Music:
        setCategory(model->getModMusic());
        break;
    case DefAppWorker::Video:
        setCategory(model->getModVideo());
        break;
    case DefAppWorker::Picture:
        setCategory(model->getModPicture());
        break;
    case DefAppWorker::Terminal:
        setCategory(model->getModTerminal());
        break;
    default:
        break;
    }
}

void AddButtonWidget::setCategory(Category * const category)
{
    m_category = category;
    connect(m_category, &Category::categoryNameChanged, this, &AddButtonWidget::setCategoryName);
    setCategoryName(m_category->getName());
}

void AddButtonWidget::setDefaultAppsCategory(DefAppWorker::DefaultAppsCategory category)
{
    m_categoryValue = category;
}

void AddButtonWidget::setCategoryName(const QString &name)
{
    m_categoryName = name;
}

void AddButtonWidget::onAddBtnClicked()
{
    QFileDialog dialog = QFileDialog();
    dialog.setWindowTitle(tr("Open Desktop file"));
    QStringList screen;
    screen << tr("Apps (*.desktop)")
           << tr("All files (*)");
    dialog.setNameFilters(screen);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    QStringList directory = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    if (!directory.isEmpty())
        dialog.setDirectory(directory.first());
    if(dialog.exec() ==  QDialog::Accepted) {
        QString path = dialog.selectedFiles().first();
        if (path.isEmpty())
            return;

        QFileInfo info(path);
        Q_EMIT requestCreateFile(m_categoryName, info);
    }
}

