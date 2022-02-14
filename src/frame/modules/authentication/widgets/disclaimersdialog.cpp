#include "disclaimersdialog.h"

#include <DFontSizeManager>
#include <DTipLabel>
#include <DTitlebar>
#include <QBoxLayout>
#include <QScrollArea>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::authentication;

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

    DTipLabel *tipLabel = new DTipLabel("");
    if (state == DisclaimersObj::Faceid) {
        tipLabel->setText(tr("Before you set up face recognition, please pay attention to the following: \n"
                             "1. Your device may be unlocked by someone with a similar appearance or objects shaped like you.\n"
                             "2. Face unlock is less secure than a password.\n"
                             "3. Face recognition rates would reduce if showing a profile face, or in dark, bright, backlit environments.\n"
                             "4. Do not hand over your device to others to avoid malicious use of face recognition.\n"
                             "5. In addition, you should take care of other factors that may affect the normal use of face recognition.\n"
                             "For better face recognition, take notice of the following when enrolling your face:\n"
                             "1. Ensure sufficient light, avoid direct sunlight, and keep other people from appearing in the frame.\n"
                             "2. Pay attention to your appearance before enrollment. Do not cover your face with your hair, a hat, sunglasses, a mask, or heavy makeup.\n"
                             "3. Do not raise or lower your head, close your eyes, or show your profile face. Make sure your frontal face appears clearly and completely in the frame.\n"
                             "If you agree to this statement and enroll your face (we will not collect your facial information, which will be encrypted locally and stored separately on your device),  face recognition will be enabled. You can go to \"Biometric Authentication\" > \"Face\" to delete the enrolled facial data.\n"));
    } else if (state == DisclaimersObj::Finge || state == DisclaimersObj::Iris) {
        setFixedSize(QSize(382, 446));
        tipLabel->setText(tr("\"Biometric authentication\" is a function for user identity authentication provided by UnionTech Software Technology Co., Ltd. Through \"biometric authentication\", the biometric data collected will be compared with that stored in the device, and the user identity will be verified based on the comparison result.\n"
                             "Please be noted that UnionTech Software will not collect or access your biometric information, which will be stored on your local device. Please only enable the biometric authentication in your personal device and use your own biometric information for related operations, and promptly disable or delete other people's biometric information on that device, otherwise you will bear the risk arising therefrom. \n"
                             "UnionTech Softeware is committed to research and improve the security, accuracy and stability of biometric authentication. However, due to environmental, equipment, technical and other factors and risk control, there is no guarantee that you will pass the biometric authentication temporarily. Therefore, please do not take biometric authentication as the only way to log in to UnionTech OS. If you have any questions or suggestions when using the biometric authentication, you can give feedback through \"Service and Support\" in the UnionTech OS. \n"));
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
    pa.setColor(QPalette::Background, Qt::transparent);
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

