// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "avatarlistwidget.h"

#include "src/plugin-accounts/operation/user.h"
#include "widgets/buttontuple.h"

#include <DConfig>
#include <DDialog>
#include <DDialogCloseButton>
#include <DIconTheme>
#include <DStyle>

#include <QDebug>
#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QPainterPath>
#include <QPixmap>
#include <QPoint>
#include <QPushButton>
#include <QRect>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QWidget>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
using namespace DCC_NAMESPACE;

static QScrollArea *get_new_scrollarea(QWidget *parent)
{
    QScrollArea *avatarArea = new QScrollArea(parent);
    avatarArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    avatarArea->setWidgetResizable(true);
    avatarArea->setFrameShape(QFrame::NoFrame);
    return avatarArea;
}

AvatarListDialog::AvatarListDialog(User *usr, AccountsWorker *worker, QWidget *parent)
    : Dtk::Widget::DAbstractDialog(parent)
    , m_worker(worker)
    , m_curUser(usr)
    , m_mainContentLayout(new QHBoxLayout)
    , m_leftContentLayout(new QVBoxLayout)
    , m_rightContentLayout(new QVBoxLayout)
    , m_avatarSelectItem(new DListView(this))
    , m_avatarSelectItemModel(new QStandardItemModel(this))
    , m_path(std::nullopt)
{
    m_mainContentLayout->setContentsMargins(0, 0, 0, 0);
    m_rightContentLayout->setContentsMargins(0, 0, 0, 0);

    // 窗口Icon
    QLabel *iconLabel = new QLabel(this);
    iconLabel->setPixmap(qApp->windowIcon().pixmap(QSize(40, 40)));

    // 窗口关闭按钮
    auto closeBtn = new DDialogCloseButton(this);
    closeBtn->setIcon(DStyle().standardIcon(DStyle::SP_DialogCloseButton));
    closeBtn->setIconSize(QSize(30, 30));
    QHBoxLayout *closeBtnLayout = new QHBoxLayout;
    closeBtnLayout->setContentsMargins(0, 0, 0, 10);
    closeBtnLayout->addStretch();
    closeBtnLayout->addWidget(closeBtn);

    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);

    m_rightContentLayout->addLayout(closeBtnLayout);

    QList<AvatarItem> items = {
        AvatarItem(tr("Person"), "dcc_user_human", Role::Person, true),
        AvatarItem(tr("Animal"), "dcc_user_animal", Role::Animal, true),
        AvatarItem(tr("Scenery"), "dcc_user_scenery", Role::Scenery, true),
        AvatarItem(tr("Illustration"), "dcc_user_funny", Role::Illustration, true),
        AvatarItem(tr("Expression"), "dcc_user_emoji", Role::Expression, true),
        AvatarItem(tr("Custom Picture"), "dcc_user_custom", Role::Custom, true),
    };

    for (const auto &item : items) {
        if (item.isLoader) {
            DStandardItem *avatarItem = new DStandardItem(item.name);
            avatarItem->setFontSize(DFontSizeManager::SizeType::T5);
            avatarItem->setIcon(DIconTheme::findQIcon(item.icon));
            avatarItem->setData(item.role, AvatarItemNameRole);
            m_avatarSelectItemModel->appendRow(avatarItem);

            if (item.role == Role::Custom) {
                m_avatarFrames[AvatarAdd] =
                        new CustomAddAvatarWidget(m_curUser, Role::Custom, this);
                m_avatarFrames[Role::Custom] =
                        new CustomAvatarWidget(m_curUser, Role::Custom, this);
            } else {
                m_avatarFrames[item.role] = new AvatarListFrame(m_curUser, item.role, this);
            }
        }
    }

    // 添加选择Item
    m_avatarSelectItem->setModel(m_avatarSelectItemModel);
    m_avatarSelectItem->setIconSize(QSize{ 16, 16 });
    m_avatarSelectItem->setAccessibleName("List_AvatarSelect");
    m_avatarSelectItem->setFrameShape(QFrame::NoFrame);
    m_avatarSelectItem->setItemSpacing(2);
    m_avatarSelectItem->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_avatarSelectItem->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_avatarSelectItem->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_leftContentLayout->setContentsMargins(10, 0, 0, 0);
    m_leftContentLayout->addWidget(iconLabel);
    m_leftContentLayout->addSpacing(12);
    m_leftContentLayout->addWidget(m_avatarSelectItem);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 10, 0, 10);
    hLayout->addLayout(m_leftContentLayout);

    QFrame *avarSelectWidget = new QFrame(this);
    avarSelectWidget->setFixedSize(180, 472);
    avarSelectWidget->setLayout(hLayout);

    m_mainContentLayout->addWidget(avarSelectWidget);

    QStackedWidget *avatarSelectWidget = new QStackedWidget(this);
    avatarSelectWidget->setFixedWidth(450);

    for (const auto &frame : m_avatarFrames) {
        auto scrolllarea = get_new_scrollarea(this);
        scrolllarea->setWidget(frame);
        avatarSelectWidget->addWidget(scrolllarea);

        auto listView = frame->getCurrentListView();
        if (!listView || listView->getCurrentListViewRole() == Role::AvatarAdd) {
            continue;
        }
        connect(listView,
                &AvatarListView::requestUpdateListView,
                this,
                &AvatarListDialog::handleListViewRequestUpdate);
        if (listView->getCurrentListViewRole() == Role::Custom) {
            connect(listView,
                    &AvatarListView::requestDeleteUserIcon,
                    this,
                    &AvatarListDialog::handleRequestDeleteIcon);
        }
    }

    m_currentSelectAvatarWidget = m_avatarFrames[Person];

    connect(m_avatarSelectItem, &DListView::clicked, this, [this, avatarSelectWidget](auto &index) {
        // 如果没有添加自定义头像, 显示自定义添加图像页面
        if (!m_avatarFrames[Custom]->isExistCustomAvatar(m_avatarFrames[Custom]->getCurrentPath(),
                                                         m_curUser->name())) {
            if (index.row() == Custom) {
                avatarSelectWidget->setCurrentIndex(index.row() + 1);
                m_currentSelectAvatarWidget = m_avatarFrames[Custom];

                return;
            }
        }

        // 切换到自定义头像界面, 更新用户头像编辑页面
        if (index.row() == Custom) {
            getCustomAvatarWidget()->getCustomAvatarView()->setAvatarPath(
                    m_avatarFrames[Custom]->getCurrentListView()->getAvatarPath());
        }

        if (auto customFrame = qobject_cast<CustomAvatarWidget*>(m_avatarFrames[Custom])) {
            customFrame->stopAvatarModify();
        }

        avatarSelectWidget->setCurrentIndex(index.row());
        QScrollArea *area = static_cast<QScrollArea *>(avatarSelectWidget->currentWidget());
        m_currentSelectAvatarWidget = static_cast<AvatarListFrame *>(area->widget());
    });

    QHBoxLayout *avatarLayout = new QHBoxLayout();
    avatarLayout->addWidget(avatarSelectWidget, Qt::AlignCenter);
    m_rightContentLayout->addLayout(avatarLayout);

    // 添加（关闭，保存）按钮
    auto buttonTuple = new ButtonTuple(ButtonTuple::Save, this);
    auto cancelButton = buttonTuple->leftButton();
    cancelButton->setText(tr("Cancel"));
    auto saveButton = buttonTuple->rightButton();
    saveButton->setText(tr("Save"));
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(10, 10, 10, 10);
    btnLayout->addWidget(cancelButton);
    btnLayout->addSpacing(10);
    btnLayout->addWidget(saveButton);

    connect(getCustomAvatarWidget()->getCustomAvatarView(),
            &CustomAvatarView::requestSaveCustomAvatar,
            m_avatarFrames[Custom]->getCurrentListView(),
            &AvatarListView::saveAvatar);

    connect(static_cast<CustomAddAvatarWidget *>(m_avatarFrames[AvatarAdd]),
            &CustomAddAvatarWidget::requestUpdateCustomWidget,
            this,
            [avatarSelectWidget, this](const QString &path) {
                m_currentSelectAvatarWidget = m_avatarFrames[Custom];
                avatarSelectWidget->setCurrentIndex(Custom);
                m_currentSelectAvatarWidget->getCurrentListView()->requestAddCustomAvatar(path);
            });

    connect(saveButton, &QPushButton::clicked, this, [this]() {
        const QString path = getAvatarPath();
        if (!path.isEmpty() && path != m_curUser->currentAvatar()) {
            m_path = path;
        }
        // 成功设置头像后关闭窗口
        accept();
    });
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    m_rightContentLayout->addLayout(btnLayout);

    QFrame *frame = new QFrame(this);
    frame->setLayout(m_rightContentLayout);
    QPalette pa(DDialog().palette());
    pa.setColor(QPalette::Base, pa.color(QPalette::Window));
    frame->setAutoFillBackground(true);
    frame->setPalette(pa);

    m_avatarSelectItem->setCurrentIndex(m_avatarSelectItem->model()->index(0, 0));
    m_mainContentLayout->addWidget(frame);

    setLayout(m_mainContentLayout);

    setFixedSize(640, 472);
    installEventFilter(this);
}

AvatarListDialog::~AvatarListDialog()
{
    if (m_avatarSelectItemModel) {
        m_avatarSelectItemModel->clear();
        m_avatarSelectItemModel->deleteLater();
        m_avatarSelectItemModel = nullptr;
    }

    m_avatarFrames.clear();
}

CustomAvatarWidget *AvatarListDialog::getCustomAvatarWidget()
{
    return static_cast<CustomAvatarWidget *>(m_avatarFrames[Custom]);
}

QString AvatarListDialog::getAvatarPath() const
{
    return m_currentSelectAvatarWidget->getAvatarPath();
}

void AvatarListDialog::handleRequestDeleteIcon(const QString &iconPath)
{
    m_worker->deleteUserIcon(m_curUser, iconPath);
}

void AvatarListDialog::handleListViewRequestUpdate(bool isSave, const int &role, const int &type)
{

    Q_UNUSED(type);

    for (auto frame : m_avatarFrames) {
        if (frame->getCurrentRole() != role) {
            if (frame->getCurrentListView()) {
                frame->getCurrentListView()->setCurrentAvatarUnChecked();
            }
        }
    }

    if (role != Custom) {
        return;
    }
    // 如果是新添加进来的用户头像, 先保存, 然后再更新用户头像编辑界面
    if (isSave) {
        m_worker->setAvatar(m_curUser, m_avatarFrames[role]->getCurrentListView()->getAvatarPath());

        connect(m_curUser, &User::currentAvatarChanged, this, [this](const QString &path) {
            if (path.contains(m_avatarFrames[Custom]->getCurrentPath())) {
                getCustomAvatarWidget()->getCurrentListView()->requestUpdateCustomAvatar(path);
                getCustomAvatarWidget()->getCustomAvatarView()->setAvatarPath(
                        m_avatarFrames[Custom]->getCurrentListView()->getAvatarPath());
            }
        });

        return;
    }

    getCustomAvatarWidget()->getCustomAvatarView()->setAvatarPath(
            m_avatarFrames[role]->getCurrentListView()->getAvatarPath());
}
