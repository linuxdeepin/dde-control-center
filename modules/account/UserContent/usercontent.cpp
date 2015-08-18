#include "usercontent.h"

UserContent::UserContent(const QString &userPath, QWidget *parent)
    : QWidget(parent),m_userPath(userPath)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignHCenter);

    //get dbus data
    m_accountUser = new DBusAccountUser(userPath, this);

    if (m_accountUser->isValid()){
        initSegmentedControl();
        initAvatarPanel();

        m_mainLayout->addStretch(1);
    }
}

void UserContent::initSegmentedControl()
{
    m_segmentedControl = new DSegmentedControl(this);
    m_segmentedControl->addSegmented("Recently Used");
    m_segmentedControl->addSegmented("Avatar");
    m_segmentedControl->addSegmented("Webcam");
    m_segmentedControl->setMaximumWidth(230);
    m_segmentedControl->setCurrentIndex(1);

    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(m_segmentedControl);
}

void UserContent::initAvatarPanel()
{
    AvatarGrid *historyAvatarGrid = new AvatarGrid(this);
    AvatarGrid *allAvatarGrid = new AvatarGrid(this);
    WebcamAvatarPanel *wap = new WebcamAvatarPanel(this);

    m_stackWidget = new QStackedWidget(this);
    connect(m_segmentedControl, &DSegmentedControl::currentChanged, m_stackWidget, &QStackedWidget::setCurrentIndex);
    connect(m_stackWidget, &QStackedWidget::currentChanged, [=](int index){
        historyAvatarGrid->setAvatars(m_accountUser->historyIcons());
        allAvatarGrid->setAvatars(m_accountUser->iconList());

        QSize ns;
        if (index == 0)
            ns = historyAvatarGrid->size();
        else if (index == 1)
            ns = allAvatarGrid->size();

        m_stackWidget->setFixedSize(ns);
        setFixedHeight(m_segmentedControl->height() + m_stackWidget->height());

        emit sizeChanged();
    });

    m_stackWidget->addWidget(historyAvatarGrid);
    m_stackWidget->addWidget(allAvatarGrid);
    m_stackWidget->addWidget(wap);
    m_stackWidget->setCurrentIndex(1);

    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(m_stackWidget);
}


