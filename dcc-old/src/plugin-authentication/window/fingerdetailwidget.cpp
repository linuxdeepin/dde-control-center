//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/finger/addfingerdialog.h"
#include "fingerdetailwidget.h"
#include "charamangermodel.h"

#include <DFontSizeManager>
#include <DTipLabel>
#include <DIconTheme>
#include <DGuiApplicationHelper>

#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QPalette>
#include <QSize>

using namespace DCC_NAMESPACE;
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

FingerDetailWidget::FingerDetailWidget(QWidget *parent)
    : QWidget(parent)
    , m_model(nullptr)
    , m_fingerWidget(new FingerWidget)
    , m_disclaimer(nullptr)
{
}

FingerDetailWidget::~FingerDetailWidget()
{

}

void FingerDetailWidget::initFingerUI()
{
    //整体布局
    QVBoxLayout *mainContentLayout = new QVBoxLayout(this);
    mainContentLayout->setContentsMargins(0, 10, 0, 0);
    mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    m_fingerWidget->setContentsMargins(0, 0, 0, 0);
    m_fingerWidget->layout()->setMargin(0);
    mainContentLayout->addWidget(m_fingerWidget);
    mainContentLayout->addSpacing(30);

    setLayout(mainContentLayout);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    //指纹界面操作
    connect(m_fingerWidget, &FingerWidget::requestAddThumbs, this, &FingerDetailWidget::showFingerDisclaimer);
    connect(m_fingerWidget, &FingerWidget::requestDeleteFingerItem, this, &FingerDetailWidget::requestDeleteFingerItem);
    connect(m_fingerWidget, &FingerWidget::requestRenameFingerItem, this, &FingerDetailWidget::requestRenameFingerItem);
    connect(m_fingerWidget, &FingerWidget::noticeEnrollCompleted, this, &FingerDetailWidget::noticeEnrollCompleted);
}

void FingerDetailWidget::initNotFingerDevice()
{
    QVBoxLayout *mainContentLayout = new QVBoxLayout(this);
    mainContentLayout->setContentsMargins(0, 10, 0, 0);
    mainContentLayout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);

    QLabel *pNotDevice = new QLabel;

    // 显示高亮字体
    DLabel *tip = new DLabel(tr("No supported devices found"));
    tip->setEnabled(false);
    auto pal = tip->palette();
    DFontSizeManager::instance()->bind(tip, DFontSizeManager::T7);
    QColor base_color = pal.text().color();
    base_color.setAlpha(255 / 10 * 2);
    pal.setColor(QPalette::Text, base_color);
    tip->setPalette(pal);

    connect(Dtk::Gui::DGuiApplicationHelper::instance(), &Dtk::Gui::DGuiApplicationHelper::themeTypeChanged,
    this, [ = ](Dtk::Gui::DGuiApplicationHelper::ColorType themeType) {
        Q_UNUSED(themeType);
        pNotDevice->setPixmap(DIconTheme::findQIcon(getDisplayPath()).pixmap(64, 64));

    });
    pNotDevice->setPixmap(DIconTheme::findQIcon(getDisplayPath()).pixmap(64, 64));
    pNotDevice->setAlignment(Qt::AlignHCenter);


    mainContentLayout->addWidget(pNotDevice);
    mainContentLayout->addWidget(tip);
    setLayout(mainContentLayout);
}

QString FingerDetailWidget::getDisplayPath()
{
    QString theme;
    DGuiApplicationHelper::ColorType type = DGuiApplicationHelper::instance()->themeType();
    switch (type) {
    case DGuiApplicationHelper::UnknownType:
        break;
    case DGuiApplicationHelper::LightType:
        theme = QString("light");
        break;
    case DGuiApplicationHelper::DarkType:
        theme = QString("dark");
        break;
    }
    return QString(":/authentication/themes/%1/icons/icon_unknown_device.svg").arg(theme);
}

void FingerDetailWidget::showDeviceStatus(bool hasDevice)
{
    // 指纹设备不支持热插拔 直接显示对应信息
    if (hasDevice) {
        initFingerUI();
    } else {
        initNotFingerDevice();
    }
}

void FingerDetailWidget::showAddFingerDialog(const QString &name, const QString &thumb)
{
    AddFingerDialog *dlg = new AddFingerDialog(thumb, this);
    connect(dlg, &AddFingerDialog::requestEnrollThumb, this, [ = ] {
        dlg->deleteLater();
        showAddFingerDialog(name, thumb);
    });
    connect(dlg, &AddFingerDialog::requestStopEnroll, this, &FingerDetailWidget::requestStopEnroll);
    connect(dlg, &AddFingerDialog::requesetCloseDlg, dlg, [ = ](const QString & userName) {
        Q_EMIT noticeEnrollCompleted(userName);
        if (m_disclaimer != nullptr) {
            m_disclaimer->close();
            delete m_disclaimer;
            m_disclaimer = nullptr;
        }
        dlg->deleteLater();
    });

    connect(m_model, &CharaMangerModel::enrollResult, dlg, [ = ](CharaMangerModel::EnrollResult res) {
        // 第一次tryEnroll进入时显示添加指纹对话框
        if (res == CharaMangerModel::Enroll_Success) {
            m_model->resetProgress();
            dlg->setFingerModel(m_model);
            dlg->setWindowFlags(Qt::Dialog | Qt::Popup | Qt::WindowStaysOnTopHint);
            dlg->setUsername(name);
            dlg->show();
            dlg->setFocus();
            dlg->activateWindow();
        } else if (res == CharaMangerModel::Enroll_Failed) {
            qDebug() << "CharaMangerModel::Enroll_Failed";
            Q_EMIT requestStopEnroll(name);
            if (m_disclaimer != nullptr) {
                m_disclaimer->close();
                delete m_disclaimer;
                m_disclaimer = nullptr;
            }
            dlg->deleteLater();
        } else if (res == CharaMangerModel::Enroll_AuthFailed) {
            qDebug() << "CharaMangerModel::Enroll_AuthFailed";
            dlg->deleteLater();
        }
    });

    Q_EMIT requestAddThumbs(name, thumb);
}

void FingerDetailWidget::setFingerModel(CharaMangerModel *model)
{
    if (!model)
        return;

    m_model = model;
    m_fingerWidget->setFingerModel(model);
    connect(model, &CharaMangerModel::vaildFingerChanged, this, &FingerDetailWidget::showDeviceStatus);
    showDeviceStatus(model->fingerVaild());
}

void FingerDetailWidget::showFingerDisclaimer(const QString &name, const QString &thumb)
{
    if (m_disclaimer != nullptr) {
        return;
    }
    m_disclaimer = new FingerDisclaimer(this);
    m_disclaimer->setVisible(true);

    connect(m_disclaimer, &FingerDisclaimer::requestShowFingeInfoDialog, this, [ = ] {
        m_disclaimer->setVisible(false);
        showAddFingerDialog(name, thumb);
    });

    connect(m_disclaimer, &FingerDisclaimer::requesetCloseDlg, this, [ = ] {
        if (m_disclaimer != nullptr)
        {
            delete m_disclaimer;
            m_disclaimer = nullptr;
        }
    });
}
