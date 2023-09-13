//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "disclaimersdialog.h"

#include <DFontSizeManager>
#include <DSuggestButton>
#include <DTipLabel>
#include <DTitlebar>
#include <QBoxLayout>
#include <QPushButton>
#include <QScrollArea>

DWIDGET_USE_NAMESPACE

DisclaimersDialog::DisclaimersDialog(DisclaimersObj disobj, DAbstractDialog *parent)
    : DAbstractDialog(parent)
    , m_mainLayout(new QVBoxLayout(this))
    , m_cancelBtn(new QPushButton(this))
    , m_acceptBtn(new DSuggestButton(this))
{
    initWidget(disobj);
    initConnect();
    QWidget::installEventFilter(this);
}

DisclaimersDialog::~DisclaimersDialog()
{

}

void DisclaimersDialog::initWidget(DisclaimersObj state)
{
    setFixedSize(QSize(454, 542));
    m_mainLayout->setAlignment(Qt::AlignHCenter);

    DTitlebar *titleIcon = new DTitlebar(this);
    titleIcon->setFrameStyle(QFrame::NoFrame);
    titleIcon->setBackgroundTransparent(true);
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle(tr("Disclaimer"));

    DLabel *tipLabel = new DLabel("");
    if (state == DisclaimersObj::Faceid) {
        tipLabel->setText(tr("Before using face recognition, please note that: \n"
                             "1. Your device may be unlocked by people or objects that look or appear similar to you.\n"
                             "2. Face recognition is less secure than digital passwords and mixed passwords.\n"
                             "3. The success rate of unlocking your device through face recognition will be reduced in a low-light, high-light, back-light, large angle scenario and other scenarios.\n"
                             "4. Please do not give your device to others randomly, so as to avoid malicious use of face recognition.\n"
                             "5. In addition to the above scenarios, you should pay attention to other situations that may affect the normal use of face recognition.\n"
                             "\n"
                             "In order to better use of face recognition, please pay attention to the following matters when inputting the facial data:\n"
                             "1. Please stay in a well-lit setting, avoid direct sunlight and other people appearing in the recorded screen.\n"
                             "2. Please pay attention to the facial state when inputting data, and do not let your hats, hair, sunglasses, masks, heavy makeup and other factors to cover your facial features.\n"
                             "3. Please avoid tilting or lowering your head, closing your eyes or showing only one side of your face, and make sure your front face appears clearly and completely in the prompt box.\n"));
    } else if (state == DisclaimersObj::Finge || state == DisclaimersObj::Iris) {
        setFixedSize(QSize(382, 446));
        tipLabel->setText(tr("\"Biometric authentication\" is a function for user identity authentication provided by UnionTech Software Technology Co., Ltd. Through \"biometric authentication\", the biometric data collected will be compared with that stored in the device, and the user identity will be verified based on the comparison result.\n"
                             "Please be noted that UnionTech Software will not collect or access your biometric information, which will be stored on your local device. Please only enable the biometric authentication in your personal device and use your own biometric information for related operations, and promptly disable or delete other people's biometric information on that device, otherwise you will bear the risk arising therefrom. \n"
                             "UnionTech Software is committed to research and improve the security, accuracy and stability of biometric authentication. However, due to environmental, equipment, technical and other factors and risk control, there is no guarantee that you will pass the biometric authentication temporarily. Therefore, please do not take biometric authentication as the only way to log in to UnionTech OS. If you have any questions or suggestions when using the biometric authentication, you can give feedback through \"Service and Support\" in the UnionTech OS. \n"));
    }


    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);
    tipLabel->adjustSize();
    tipLabel->setWordWrap(true);
    tipLabel->setContentsMargins(20, 0, 20, 0);
    tipLabel->setAlignment(Qt::AlignJustify);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->setWidget(tipLabel);
    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
    QPalette pa = scrollArea->palette();
    pa.setColor(QPalette::Window, Qt::transparent);
    scrollArea->setPalette(pa);

    // 下方按钮
    QHBoxLayout *btnLayout = new QHBoxLayout(this);
    m_cancelBtn->setText(tr("Cancel"));
    m_acceptBtn->setText(tr("Next"));
    m_acceptBtn->setDisabled(false);

    btnLayout->addWidget(m_cancelBtn, Qt::AlignCenter);
    btnLayout->addSpacing(10);
    btnLayout->addWidget(m_acceptBtn, Qt::AlignCenter);
    btnLayout->setContentsMargins(20, 10, 20, 20);

    m_mainLayout->addWidget(titleIcon, Qt::AlignTop | Qt::AlignRight);
    m_mainLayout->addSpacing(10);
    m_mainLayout->setMargin(0);
    m_mainLayout->addWidget(scrollArea, 0, Qt::AlignHCenter);
    m_mainLayout->setSpacing(5);
    m_mainLayout->addLayout(btnLayout);
    setLayout(m_mainLayout);
}

void DisclaimersDialog::initConnect()
{
    connect(m_cancelBtn, &QPushButton::clicked, this, &DisclaimersDialog::close);
    connect(m_acceptBtn, &QPushButton::clicked, this, [this] {
        Q_EMIT requestClickStatus(true);
        this->close();
    });
}

void DisclaimersDialog::closeEvent(QCloseEvent *event)
{
    Q_EMIT requesetCloseDlg(true);
    QDialog::closeEvent(event);
}

