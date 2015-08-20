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

    setStyleSheet("background-color: transparent;");
}

void AccountMainWidget::initHeader()
{
    m_header = new ModuleHeader("Account", false, this);
    m_mainLayout->addWidget(m_header);

    QStackedWidget *stackWidget = new QStackedWidget();
    m_header->setRightContent(stackWidget);

    QWidget *headerButtonContent = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    DTextButton *deleteButton = new DTextButton("-");
    DTextButton *addButton = new DTextButton("+");
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addSpacing(DUI::BUTTON_MARGIN);
    buttonLayout->addWidget(addButton);
    headerButtonContent->setLayout(buttonLayout);

    DTextButton *listButton = new DTextButton("User List");

    stackWidget->addWidget(headerButtonContent);
    stackWidget->addWidget(listButton);

    connect(deleteButton, &DTextButton::clicked, this, &AccountMainWidget::requestDelete);
    connect(deleteButton, &DTextButton::clicked, [=]{
        if (m_state == StateDeleting){
            setPanelState(StateNormal);
            emit requestDelete(false);
        }
        else{
            setPanelState(StateDeleting);
            emit requestDelete(true);
        }

    });
    connect(addButton, &DTextButton::clicked, [=]{
        setPanelState(StateCreating);
    });

    connect(this, &AccountMainWidget::stateChanged, [=](PanelState state){
        switch (state){
        case StateNormal:
            stackWidget->setVisible(true);
            stackWidget->setCurrentIndex(0);
            break;
        case StateCreating:
            stackWidget->setVisible(false);
            break;
        case StateDeleting:
            stackWidget->setVisible(true);
            stackWidget->setCurrentIndex(0);
            break;
        case StateSetting:
            stackWidget->setVisible(true);
            stackWidget->setCurrentIndex(1);
            break;
        default:
            break;
        }
    });
}

void AccountMainWidget::initListPanel()
{
    m_listPanel = new UserListPanel();
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

    m_stackWidget->addWidget(m_createPanel);
}

void AccountMainWidget::setPanelState(AccountMainWidget::PanelState state)
{
    m_state = state;

    emit stateChanged(state);
}

