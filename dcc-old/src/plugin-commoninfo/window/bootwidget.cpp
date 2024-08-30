//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "bootwidget.h"
#include "commonbackgrounditem.h"

#include "pwqualitymanager.h"
#include "src/plugin-commoninfo/operation/commoninfomodel.h"
#include "src/frame/utils.h"

#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"

#include <DDialog>
#include <DWidget>
#include <DTipLabel>
#include <DPasswordEdit>
#include <DCommandLinkButton>
#include <DPaletteHelper>
#include <DSysInfo>

#include <QVBoxLayout>
#include <QScrollBar>
#include <QListView>
#include <QDebug>
#include <QSizePolicy>
#include <QScroller>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QLabel>
#include <QLoggingCategory>


Q_LOGGING_CATEGORY(DccCommonInfoBoot, "dcc-commoninfo-bootwidget");

Q_DECLARE_METATYPE(QMargins)

const QMargins ListViewItemMargin(10, 8, 10, 8);
const QVariant VListViewItemMargin = QVariant::fromValue(ListViewItemMargin);

constexpr static int LISTVIEW_ITEM_SPACING = 3;
constexpr static int LISTVIEW_ITEM_HEIGHT = 32;
constexpr static int LISTVIEW_ITEM_HEIGHT_WITH_SPACING = LISTVIEW_ITEM_SPACING + LISTVIEW_ITEM_HEIGHT;
constexpr static int LISTVIEW_MAX_HEIGHT = 350;
constexpr static int MAX_BACKGROUND_HEIGHT = LISTVIEW_MAX_HEIGHT + LISTVIEW_ITEM_HEIGHT_WITH_SPACING;

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

BootWidget::BootWidget(QWidget *parent)
    : QWidget(parent)
    , m_isCommoninfoBootWallpaperConfigValid(!QSysInfo::currentCpuArchitecture().contains("arm"))
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
    SettingsGroup *groupOther = new SettingsGroup;
    groupOther->getLayout()->setContentsMargins(0, 0, 0, 0);

    m_listLayout = new QVBoxLayout;
    m_listLayout->addSpacing(10);
    m_listLayout->setMargin(0);

    m_bootList = new CommonInfoListView(this);
    m_bootList->setAccessibleName("List_bootlist");
    m_bootList->setAutoScroll(false);
    m_bootList->setFrameShape(QFrame::NoFrame);
    m_bootList->setDragDropMode(QListView::DragDrop);
    m_bootList->setDefaultDropAction(Qt::MoveAction);
    m_bootList->setAutoFillBackground(false);
    m_bootList->setDragEnabled(false);
    m_bootList->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
    m_bootList->setSelectionMode(DListView::NoSelection);
    m_bootList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_bootList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_bootList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_bootList->setMinimumWidth(240);
    m_bootList->setItemSpacing(LISTVIEW_ITEM_SPACING);

    QSize itemSize = m_bootList->itemSize();
    itemSize.setHeight(LISTVIEW_ITEM_HEIGHT);
    m_bootList->setItemSize(itemSize);

    m_bootList->setMaxShowHeight(LISTVIEW_MAX_HEIGHT);
    m_bootList->setWordWrap(true);

    DPalette dp = DPaletteHelper::instance()->palette(m_bootList);
    dp.setColor(DPalette::Text, QColor(255, 255, 255));
    DPaletteHelper::instance()->setPalette(m_bootList, dp);

    m_updatingLabel = new QLabel(tr("Updating..."), this);
    m_updatingLabel->setVisible(false);

    DPalette dpLabel = DPaletteHelper::instance()->palette(m_updatingLabel);
    dpLabel.setColor(DPalette::Text, QColor(255, 255, 255));
    DPaletteHelper::instance()->setPalette(m_updatingLabel, dpLabel);
    m_listLayout->addWidget(m_updatingLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);

    m_background = new CommonBackgroundItem(this);
    m_background->setLayout(m_listLayout);

    m_bootDelay = new SwitchWidget(this);
    m_bootDelay->setTitle(tr("Startup Delay"));
    m_theme = new SwitchWidget(this);
    m_theme->setTitle(tr("Theme"));

    QMap<bool, QString> mapBackgroundMessage;
    mapBackgroundMessage[true] = tr("Click the option in boot menu to set it as the first boot, and drag and drop a picture to change the background");
    mapBackgroundMessage[false] = tr("Click the option in boot menu to set it as the first boot");
    DTipLabel *backgroundLabel = new DTipLabel(mapBackgroundMessage[false], this);
    backgroundLabel->setText(mapBackgroundMessage[true]);
    backgroundLabel->setWordWrap(true);
    backgroundLabel->setContentsMargins(0, 0, 0, 0);
    backgroundLabel->setAlignment(Qt::AlignLeft);
    m_themeLbl = new DTipLabel(tr("Switch theme on to view it in boot menu"), this);
    m_themeLbl->setAccessibleName("themeLbl");
    m_themeLbl->setWordWrap(true);
    m_themeLbl->setContentsMargins(0, 0, 0, 0);
    m_themeLbl->setAlignment(Qt::AlignLeft);
    groupOther->appendItem(m_bootDelay);
    groupOther->setSpacing(10);
    groupOther->appendItem(m_theme);
    groupOther->setSpacing(10);
    layout->setMargin(0);
    layout->addSpacing(10);
    layout->addWidget(m_background);
    layout->addSpacing(10);
    layout->addWidget(backgroundLabel);
    layout->addSpacing(10);
    layout->addWidget(groupOther);
    layout->addWidget(m_themeLbl);

    if (!m_isCommoninfoBootWallpaperConfigValid) {
        m_theme->setVisible(false);
        m_themeLbl->setVisible(false);
    }

    m_grubVerification = new SwitchWidget(this);
    m_grubVerification->setTitle(tr("GRUB Authentication"));
    m_grubVerification->addBackground();
    layout->addSpacing(10);
    layout->addWidget(m_grubVerification);

    m_grubVerifyLbl = new DTipLabel(tr("GRUB password is required to edit its configuration"), this);
    m_grubVerifyLbl->setAccessibleName("grubVerifyLbl");
    m_grubVerifyLbl->setWordWrap(true);
    m_grubVerifyLbl->setAlignment(Qt::AlignLeft);
    QHBoxLayout *hLayout = new QHBoxLayout;
    layout->addLayout(hLayout);
    hLayout->addWidget(m_grubVerifyLbl, 1);
    m_grubModifyPasswdLink = new DCommandLinkButton(tr("Change Password"), this);
    m_grubModifyPasswdLink->hide();
    hLayout->addWidget(m_grubModifyPasswdLink, 0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch();
    layout->setContentsMargins(0, 10, 0, 10);
    setWindowTitle(tr("Boot Menu"));

    if (IS_COMMUNITY_SYSTEM) {
        m_grubVerification->hide();
        m_grubVerifyLbl->hide();
    }

    connect(m_theme, &SwitchWidget::checkedChanged, this, &BootWidget::enableTheme);
    connect(m_bootDelay, &SwitchWidget::checkedChanged, this, &BootWidget::bootdelay);
    connect(m_bootList, &DListView::clicked, this ,&BootWidget::onCurrentItem);
    connect(m_background, &CommonBackgroundItem::requestEnableTheme, this, &BootWidget::enableTheme);
    connect(m_background, &CommonBackgroundItem::requestSetBackground, this, &BootWidget::requestSetBackground);

    m_background->setThemeEnable(m_isCommoninfoBootWallpaperConfigValid);
    backgroundLabel->setText(mapBackgroundMessage[m_isCommoninfoBootWallpaperConfigValid]);
    // 修改grub密码
    connect(m_grubVerification, &SwitchWidget::checkedChanged, this, &BootWidget::enableGrubEditAuth);
    connect(m_grubModifyPasswdLink, &DCommandLinkButton::clicked, this, [this]{
        showGrubEditAuthPasswdDialog(true);
    });
}

BootWidget::~BootWidget()
{
}

void BootWidget::setDefaultEntry(const QString &value)
{
    m_defaultEntry = value;

    blockSignals(true);
    int row_count = m_bootItemModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *item = m_bootItemModel->item(i, 0);
        if (item->text() == value) {
            m_curSelectedIndex = item->index();
            item->setCheckState(Qt::CheckState::Checked);
        } else {
            item->setCheckState(Qt::CheckState::Unchecked);
        }
    }
    blockSignals(false);
}

void BootWidget::setModel(CommonInfoModel *model)
{
    m_commonInfoModel = model;

    connect(model, &CommonInfoModel::bootDelayChanged, m_bootDelay, &SwitchWidget::setChecked);
    connect(model, &CommonInfoModel::themeEnabledChanged, m_theme, &SwitchWidget::setChecked);
    connect(model, &CommonInfoModel::defaultEntryChanged, this, &BootWidget::setDefaultEntry);
    connect(model, &CommonInfoModel::updatingChanged, m_updatingLabel, &QLabel::setVisible);
    connect(model, &CommonInfoModel::entryListsChanged, this, &BootWidget::setEntryList);
    connect(model, &CommonInfoModel::themeEnabledChanged, this, [&](const bool _t1) {
        if (m_isCommoninfoBootWallpaperConfigValid) {
            m_background->setThemeEnable(_t1);
            m_background->updateBackground(m_commonInfoModel->background());
        }
    });
    connect(model, &CommonInfoModel::backgroundChanged, m_background, &CommonBackgroundItem::updateBackground);

    // modified by wuchuanfei 20190909 for 8613
    m_bootDelay->setChecked(model->bootDelay());
    m_theme->setChecked(model->themeEnabled());
    m_updatingLabel->setVisible(model->updating());
    m_background->setThemeEnable(model->themeEnabled() && m_isCommoninfoBootWallpaperConfigValid);

    setEntryList(model->entryLists());
    setDefaultEntry(model->defaultEntry());

    if(m_isCommoninfoBootWallpaperConfigValid)
        m_background->updateBackground(model->background());

    m_grubVerification->setChecked(m_commonInfoModel->grubEditAuthEnabled());
    m_grubModifyPasswdLink->setVisible(m_commonInfoModel->isShowGrubEditAuth() && m_grubVerification->checked());
    connect(model, &CommonInfoModel::grubEditAuthEnabledChanged, this, [&](const bool &value) {
        // from CommonInfoWork::onEnabledUsersChanged
        m_grubVerification->setChecked(value);
        m_grubModifyPasswdLink->setVisible(m_commonInfoModel->isShowGrubEditAuth() && value);
    });

    connect(m_grubVerification, &SwitchWidget::checkedChanged, this, [&](const bool &value){
        Q_UNUSED(value);
        m_grubModifyPasswdLink->setVisible(m_commonInfoModel->isShowGrubEditAuth() && m_grubVerification->checked());
    });
}

void BootWidget::setEntryList(const QStringList &list)
{
    m_bootItemModel = new QStandardItemModel(this);
    m_bootList->setModel(m_bootItemModel);

    for (int i = 0; i < list.count(); i++) {
        const QString entry = list.at(i);

        DStandardItem *item = new DStandardItem();
        item->setText(entry);
        item->setCheckable(false); // for Bug 2449
        item->setData(VListViewItemMargin, Dtk::MarginsRole);

        m_bootItemModel->appendRow(item);

        if (m_defaultEntry == entry) {
            m_curSelectedIndex = item->index();
            item->setCheckState(Qt::CheckState::Checked);
        } else {
            item->setCheckState(Qt::CheckState::Unchecked);
        }
    }

    setBootList();
}

void BootWidget::setBootList()
{
    int cout = m_bootList->count();
    int height = (cout + 2) * LISTVIEW_ITEM_HEIGHT;

    m_listLayout->addWidget(m_bootList, Qt::AlignCenter);
    m_listLayout->addSpacing(15);
    m_background->setFixedHeight(std::min(height + LISTVIEW_ITEM_HEIGHT , MAX_BACKGROUND_HEIGHT));
}

void BootWidget::onCurrentItem(const QModelIndex &curIndex)
{
    QString curText = curIndex.data().toString();
    if (curText.isEmpty())
        return;

    // 获取当前被选项
    QString selectedText = m_curSelectedIndex.data().toString();

    qCDebug(DccCommonInfoBoot) << "current text" << curText << "selectedText" << selectedText;
    // NOTE: If the type of current selected boot entry is submenu whitch is not shown in current boot list, do not care, and do the set action
    if (curText != selectedText) {
        qCInfo(DccCommonInfoBoot) << "start to set first menu to "<< curText;
        Q_EMIT defaultEntry(curText);
    }
}

void BootWidget::onGrubEditAuthCancel(bool toEnable)
{
    m_grubVerification->setChecked(toEnable);
    m_grubModifyPasswdLink->setVisible(m_commonInfoModel->isShowGrubEditAuth() && toEnable);
}

void BootWidget::setGrubEditAuthVisible(bool show)
{
    if (!show) {
        m_grubModifyPasswdLink->hide();
    }
    bool isShow = (!IS_COMMUNITY_SYSTEM) && show;
    m_grubVerifyLbl->setVisible(isShow);
    m_grubVerification->setVisible(isShow);
}

void BootWidget::showGrubEditAuthPasswdDialog(bool isReset)
{
    if (m_grubEditAuthDialog) {
        return;
    }
    m_grubEditAuthDialog = new DDialog(tr("Change GRUB password"), nullptr, nullptr);
    m_grubEditAuthDialog->setIcon(DStyle().standardIcon(DStyle::SP_MessageBoxWarning));
    // 需要重新布局
    DWidget *widget = new DWidget;
    QGridLayout *grid = new QGridLayout(widget);
    DLabel *usernameLabel = new DLabel(tr("Username:"));
    DLabel *rootLabel = new DLabel(tr("root"));
    DLabel *label1 = new DLabel(tr("New password:"));
    DLabel *label2 = new DLabel(tr("Repeat password:"));
    DPasswordEdit *edit1 = new DPasswordEdit();
    DPasswordEdit *edit2 = new DPasswordEdit();
    edit1->setPlaceholderText(tr("Required"));
    edit2->setPlaceholderText(tr("Required"));
    edit1->setCutEnabled(false);
    edit1->setCopyEnabled(false);
    edit2->setCutEnabled(false);
    edit2->setCopyEnabled(false);
    grid->addWidget(usernameLabel, 0, 0, 1, 1);
    grid->addWidget(rootLabel, 0, 1, 1, 1);
    grid->addWidget(label1, 1, 0, 1, 1);
    grid->addWidget(edit1, 1, 1, 1, 1);
    grid->addWidget(label2, 2, 0, 1, 1);
    grid->addWidget(edit2, 2, 1, 1, 1);
    grid->setRowMinimumHeight(0, 40);
    grid->setMargin(0);
    m_grubEditAuthDialog->addContent(widget);
    m_grubEditAuthDialog->addButton(tr("Cancel", "button"), false, DDialog::ButtonNormal);
    m_grubEditAuthDialog->addButton(tr("Confirm", "button"), true, DDialog::ButtonRecommend);

    QList<QAbstractButton*> buttons = m_grubEditAuthDialog->getButtons();
    buttons[1]->setEnabled(false);

    QObject::connect(edit1, &DPasswordEdit::textChanged, [edit1, edit2, buttons](const QString &text){
        if (text.isEmpty()) {
            buttons[1]->setEnabled(false);
            if (!edit2->text().isEmpty()) {
                edit1->setAlert(true);
                edit1->showAlertMessage(tr("Password cannot be empty"));
            }
            return;
        }
        // "root" 是设置/修改 grub 密码默认的账户, 同 GRUB_EDIT_AUTH_ACCOUNT
        PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword("", edit1->lineEdit()->text(), PwqualityManager::CheckType::Grub2);
        if (error != PwqualityManager::ERROR_TYPE::PW_NO_ERR) {
            edit1->showAlertMessage(PwqualityManager::instance()->getErrorTips(error, PwqualityManager::CheckType::Grub2));
            buttons[1]->setEnabled(false);
            edit1->setAlert(true);
        } else {
            if (!edit2->text().isEmpty() && text != edit2->text()) {
                edit1->setAlert(true);
                edit1->showAlertMessage(tr("Passwords do not match"));
            }
            bool isAlert = text != edit2->text() && !edit2->text().isEmpty();
            if (!isAlert) {
                edit1->hideAlertMessage();
                edit2->hideAlertMessage();
                edit2->setAlert(false);
            }
            edit1->setAlert(isAlert);
            buttons[1]->setEnabled(!isAlert && text == edit2->text());
        }
    });
    QObject::connect(edit2, &DPasswordEdit::textChanged, [edit1, edit2, buttons](const QString &text){
        if (text.isEmpty()) {
            buttons[1]->setEnabled(false);
            if (!edit1->text().isEmpty()) {
                edit2->setAlert(true);
                edit2->showAlertMessage(tr("Password cannot be empty"));
            }
            return;
        }
        PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword("", text, PwqualityManager::CheckType::Grub2);
        if (error != PwqualityManager::ERROR_TYPE::PW_NO_ERR) {
            edit2->showAlertMessage(PwqualityManager::instance()->getErrorTips(error, PwqualityManager::CheckType::Grub2));
            buttons[1]->setEnabled(false);
            edit2->setAlert(true);
            return;
        }

        if (text != edit1->text()) {
            edit2->showAlertMessage(tr("Passwords do not match"));
        }

        bool isValid = !edit1->text().isEmpty() && text == edit1->text();
        edit2->setAlert(!isValid);
        if (isValid) {
            edit1->hideAlertMessage();
            edit2->hideAlertMessage();
            edit1->setAlert(false);
        }
        buttons[1]->setEnabled(isValid);
    });

    QObject::connect(m_grubEditAuthDialog, &DDialog::buttonClicked, [=](int index){
        if (index == 1) {
            // 需要将密码发送后在worker里加密
            Q_EMIT setGrubEditPasswd(edit1->text(), isReset);
        } else {
            if (!isReset) {
                m_grubVerification->setChecked(false);
            }
        }
    });
    QObject::connect(m_grubEditAuthDialog, &DDialog::closed, [=](){
        if (!isReset) {
            m_grubVerification->setChecked(false);
        }
    });
    m_grubEditAuthDialog->exec();
    m_grubEditAuthDialog->deleteLater();
    m_grubEditAuthDialog = nullptr;
}

void BootWidget::resizeEvent(QResizeEvent *event)
{
    auto w = event->size().width();
    m_listLayout->setContentsMargins(static_cast<int>(w * 0.2), 0, static_cast<int>(w * 0.2), 0);
}
