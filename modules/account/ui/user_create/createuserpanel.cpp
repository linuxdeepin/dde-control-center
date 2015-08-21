#include "createuserpanel.h"

CreateUserPanel::CreateUserPanel(QWidget *parent) : QWidget(parent)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    initDbusData();
    if (m_account && m_account->isValid()){
        initHeader();
        initInputLline();
        initConfirmLine();
        initInfoLine();

        m_layout->addStretch();
    }
}

void CreateUserPanel::initDbusData()
{
    m_account = new DBusAccount(this);
    connect(m_account, &DBusAccount::UserAdded, this, &CreateUserPanel::onUserAdded);
    if (m_account->isValid())
        m_randIcon = m_account->RandUserIcon().value();
}

void CreateUserPanel::initHeader()
{
    DSeparatorHorizontal *s1 = new DSeparatorHorizontal();
    DSeparatorHorizontal *s2 = new DSeparatorHorizontal();
    QLabel *headerLabel = new QLabel(tr("Create New User"));
    headerLabel->setObjectName("CreateHeaderLabel");
    headerLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    QFrame *header = new QFrame;
    header->setFixedHeight(DUI::EXPAND_HEADER_HEIGHT);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(0);
    headerLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    headerLayout->addSpacing(DUI::HEADER_LEFT_MARGIN);
    headerLayout->addWidget(headerLabel);

    m_layout->addWidget(s1);
    m_layout->addWidget(header);
    m_layout->addWidget(s2);
}

void CreateUserPanel::initInfoLine()
{
    QFrame *infoFrame = new QFrame;
    infoFrame->setFixedHeight(100);
    QHBoxLayout *hLayout = new QHBoxLayout(infoFrame);
    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    QVBoxLayout *vLayout  = new QVBoxLayout;
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    UserIcon *avatar = new UserIcon;
    avatar->setFixedSize(ICON_SIZE, ICON_SIZE);
    avatar->setIcon(m_randIcon);

    QLabel *newNameLabel = new QLabel(tr("New User"));
    newNameLabel->setObjectName("NewNameLabel");
    newNameLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    connect(m_nameLine, &InputLine::textChanged, [=](QString text){
        newNameLabel->setText(text.isEmpty() ? tr("New User") : text.toLower());
    });
    QLabel *newTypeLabel = new QLabel(tr("Normal User"));
    newTypeLabel->setObjectName("NewTypeLabel");
    newTypeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    connect(m_accountType, &AccountTypeLine::typeChanged, [=](int type){
        switch (type) {
        case 1:
            newTypeLabel->setText(tr("Administor"));
            break;
        default:
            newTypeLabel->setText(tr("Normal User"));
            break;
        }
    });

    vLayout->addWidget(newNameLabel);
    vLayout->addWidget(newTypeLabel);

    hLayout->addSpacing(DUI::HEADER_LEFT_MARGIN);
    hLayout->addWidget(avatar);
    hLayout->addSpacing(DUI::HEADER_LEFT_MARGIN);
    hLayout->addLayout(vLayout);
    hLayout->addStretch(1);

    DSeparatorHorizontal *s = new DSeparatorHorizontal();
    m_layout->insertWidget(1, s);
    m_layout->insertWidget(1, infoFrame);
}

void CreateUserPanel::initInputLline()
{
    m_nameLine = new InputLine();
    m_passwdNew = new PasswdLine();
    m_passwdRepeat = new PasswdLine();
    m_accountType = new AccountTypeLine();
    m_autoLogin = new SwitchLine();

    QFont f = m_nameLine->lineEdit()->font();
    f.setCapitalization(QFont::AllLowercase);
    m_nameLine->lineEdit()->setFont(f);

    m_nameLine->setTitle(tr("User Name"));
    m_passwdNew->setTitle(tr("Password"));
    m_passwdRepeat->setTitle(tr("Repeat Password"));
    m_accountType->setTitle(tr("Account Type"));
    m_autoLogin->setTitle(tr("Auto Login"));

    DSeparatorHorizontal *s1 = new DSeparatorHorizontal();
    DSeparatorHorizontal *s2 = new DSeparatorHorizontal();
    DSeparatorHorizontal *s3 = new DSeparatorHorizontal();
    m_layout->addWidget(m_nameLine);
    m_layout->addWidget(s1);
    m_layout->addWidget(m_passwdNew);
    m_layout->addWidget(s2);
    m_layout->addWidget(m_passwdRepeat);
    m_layout->addWidget(s3);
    m_layout->addWidget(m_accountType);
    m_layout->addWidget(m_autoLogin);

    QTimer::singleShot(200, this, SLOT(updateLineStyle()));
}

void CreateUserPanel::initConfirmLine()
{
    m_confirmLine = new ConfirmButtonLine();
    connect(m_confirmLine, &ConfirmButtonLine::cancel, this, &CreateUserPanel::onCancel);
    connect(m_confirmLine, &ConfirmButtonLine::confirm, this, &CreateUserPanel::onConfirm);

    DSeparatorHorizontal *s4 = new DSeparatorHorizontal();
    m_layout->addWidget(m_confirmLine);
    m_layout->addWidget(s4);
}

void CreateUserPanel::updateLineStyle()
{
    //For Style sheet
    //DUI with a higher priority
    QString backgrounStyle = QString("DUI--DBaseLine { background-color: %1}").arg(lineBackgroundColor());
    m_nameLine->setStyleSheet(m_nameLine->styleSheet() + backgrounStyle);
    m_passwdNew->setStyleSheet(m_passwdNew->styleSheet() + backgrounStyle);
    m_passwdRepeat->setStyleSheet(m_passwdRepeat->styleSheet() + backgrounStyle);
    m_accountType->setStyleSheet(m_accountType->styleSheet() + backgrounStyle);
    m_autoLogin->setStyleSheet(m_autoLogin->styleSheet() + backgrounStyle);
    m_confirmLine->setStyleSheet(m_confirmLine->styleSheet() + backgrounStyle);
}

void CreateUserPanel::onCancel()
{
    emit createCancel();
}

void CreateUserPanel::onConfirm()
{
    if (!m_nameLine->text().isEmpty()
            && !m_passwdNew->text().isEmpty()
            && m_passwdNew->text() == m_passwdRepeat->text()){
        m_account->CreateUser(m_nameLine->text().toLower(), "", m_accountType->currentIndex());

        emit createConfirm();
    }
    else{
        //TODO
        qWarning() << "[Warnig] Info got error ==========";
    }
}

void CreateUserPanel::onUserAdded(const QString &path)
{
    DBusAccountUser *user = new DBusAccountUser(path, this);
    if (user->isValid()){
        if (!m_randIcon.isEmpty())
            user->SetIconFile(m_randIcon);
        if (!m_passwdNew->text().isEmpty() && m_passwdNew->text() == m_passwdRepeat->text())
            user->SetPassword(m_passwdNew->text());
        user->SetAutomaticLogin(m_autoLogin->check());
    }
}
QString CreateUserPanel::lineBackgroundColor() const
{
    return m_lineBackgroundColor;
}

void CreateUserPanel::setLineBackgroundColor(const QString &lineBackgroundColor)
{
    m_lineBackgroundColor = lineBackgroundColor;
}




