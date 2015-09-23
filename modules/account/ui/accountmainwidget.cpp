#include "accountmainwidget.h"

AccountMainWidget::AccountMainWidget(QWidget *parent) : QFrame(parent)
{
    D_THEME_INIT_WIDGET(AccountMainWidget);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    m_stackWidget = new QStackedWidget();

    DSeparatorHorizontal *sh = new DSeparatorHorizontal(this);
    m_mainLayout->addWidget(sh);

    initHeader();
    initListPanel();
    initCreatePanel();

    m_mainLayout->addWidget(m_stackWidget);
    connect(this, &AccountMainWidget::stateChanged, [=](PanelState state){
        if (state == StateCreating)
            m_stackWidget->setCurrentIndex(1);
        else
            m_stackWidget->setCurrentIndex(0);
    });
}

void AccountMainWidget::preDestroy()
{
    m_createPanel->preDestroy();
    m_listPanel->preDestroy();
}

void AccountMainWidget::initHeader()
{
    m_header = new ModuleHeader(tr("Account"), false, this);
    m_mainLayout->addWidget(m_header);
    DSeparatorHorizontal *separator = new DSeparatorHorizontal;
    m_mainLayout->addWidget(separator);


    initHeaderStackWidget();

    m_header->setRightContent(m_headerStackWidget);

    //dsTr("Delete Account")dsTr("Add Account")
    QWidget *headerButtonContent = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(0);
    buttonLayout->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    GeneralRemoveButton *deleteButton = new GeneralRemoveButton;
    GeneralAddButton *addButton = new GeneralAddButton;
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addSpacing(DUI::BUTTON_MARGIN);
    buttonLayout->addWidget(addButton);
    headerButtonContent->setLayout(buttonLayout);

    DTextButton *listButton = new DTextButton(tr("User List"));
    listButton->setFixedHeight(DUI::BUTTON_HEIGHT);
    connect(listButton, &DTextButton::clicked, [=]{
        m_headerStackWidget->setVisible(true);
        m_headerStackWidget->setCurrentIndex(0);
        m_state = StateNormal;
        emit showForNormal();
    });
    QFrame *lbFrame = new QFrame;
    QHBoxLayout *lbLayout = new QHBoxLayout(lbFrame);
    lbLayout->setContentsMargins(0, 0, 0, 0);
    lbLayout->setAlignment(Qt::AlignVCenter);
    lbLayout->addWidget(listButton);

    m_headerStackWidget->addWidget(headerButtonContent);
    m_headerStackWidget->addWidget(lbFrame);

    DynamicLabel *toolTip = new DynamicLabel(m_header);

    connect(this, &AccountMainWidget::cancelDelete, [=]{
       if (deleteButton){
           deleteButton->setChecked(false);

           m_state = StateNormal;
       }
    });

    connect(deleteButton, &GeneralRemoveButton::stateChanged, [=]{
        DImageButton::State buttonState = deleteButton->getState();
        if (buttonState == DImageButton::Hover || buttonState == DImageButton::Press)
            return;

        switch (buttonState) {
        case DImageButton::Checked:
            if (m_state == StateDeleting)
                break;
            setPanelState(StateDeleting);
            emit requestDelete(true);
            break;
        default:
            if (m_state == StateNormal)
                break;
            setPanelState(StateNormal);
            emit requestDelete(false);
            break;
        }
        toolTip->hideLabel();
    });
    connect(deleteButton, &GeneralRemoveButton::mouseEnter, [=]{
        toolTip->setText(tr("Delete Account"));
        //the x or width value is valid after all component ready,infact it only need move once
        toolTip->move(deleteButton->mapTo(this, QPoint(0, 0)).x() - toolTip->width() - DUI::TEXT_RIGHT_MARGIN,
                      (m_header->height() - toolTip->height()) / 2);

        toolTip->showLabel();
    });
    connect(deleteButton, &GeneralRemoveButton::mouseLeave, [=]{
        toolTip->hideLabel();
    });

    connect(addButton, &GeneralAddButton::clicked, [=]{
        setPanelState(StateCreating);
        toolTip->hideLabel();
    });
    connect(addButton, &GeneralAddButton::mouseEnter, [=]{
        toolTip->setText(tr("Add Account"));
        //the x or width value is valid after all component ready,infact it only need move once
        toolTip->move(deleteButton->mapTo(this, QPoint(0, 0)).x() - toolTip->width() - DUI::TEXT_RIGHT_MARGIN,
                      (m_header->height() - toolTip->height()) / 2);

        toolTip->showLabel();
    });
    connect(addButton, &GeneralAddButton::mouseLeave, [=]{
        toolTip->hideLabel();
    });

}

void AccountMainWidget::initListPanel()
{
    m_listPanel = new UserListPanel();
    connect(this, &AccountMainWidget::requestDelete, m_listPanel, &UserListPanel::requestDelete);
    connect(this, &AccountMainWidget::hideForSetting, m_listPanel, &UserListPanel::hideForSetting);
    connect(this, &AccountMainWidget::showForNormal, m_listPanel, &UserListPanel::showForNormal);
    connect(m_listPanel, &UserListPanel::changeToSetting, [=](bool setting){
        if (setting){
            m_headerStackWidget->setVisible(true);
            m_headerStackWidget->setCurrentIndex(1);

            m_state = StateSetting;
            emit hideForSetting();
        }
        else{
            m_headerStackWidget->setVisible(true);
            m_headerStackWidget->setCurrentIndex(0);

            m_state = StateNormal;
            emit showForNormal();
        }
    });
    connect(m_listPanel, &UserListPanel::cancelDelete, [=]{
        emit cancelDelete();
        emit requestDelete(false);
        setPanelState(StateNormal);
    });
    m_listScrollArea = new QScrollArea();
    m_listScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listScrollArea->setWidgetResizable(true);
    m_listScrollArea->setWidget(m_listPanel);

    m_stackWidget->addWidget(m_listScrollArea);
}

void AccountMainWidget::initCreatePanel()
{
    m_createPanel = new CreateUserPanel();
    connect(m_createPanel, &CreateUserPanel::createCancel, [=]{
        setPanelState(StateNormal);
    });
    connect(m_createPanel, &CreateUserPanel::createConfirm, [=]{
        setPanelState(StateNormal);
    });

    m_stackWidget->addWidget(m_createPanel);
}

void AccountMainWidget::initHeaderStackWidget()
{
    m_headerStackWidget = new QStackedWidget();

    connect(this, &AccountMainWidget::stateChanged, [=](PanelState state){
        switch (state){
        case StateNormal:
            m_headerStackWidget->setVisible(true);
            m_headerStackWidget->setCurrentIndex(0);
            break;
        case StateCreating:
            m_headerStackWidget->setVisible(false);
            break;
        case StateDeleting:
            m_headerStackWidget->setVisible(true);
            m_headerStackWidget->setCurrentIndex(0);
            break;
        case StateSetting:
            m_headerStackWidget->setVisible(true);
            m_headerStackWidget->setCurrentIndex(1);
            break;
        default:
            break;
        }
    });
}

void AccountMainWidget::setPanelState(AccountMainWidget::PanelState state)
{
    m_state = state;

    emit stateChanged(state);
}

