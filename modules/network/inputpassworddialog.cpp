#include <QVBoxLayout>
#include <QEventLoop>
#include <QDebug>

#include <libdui/dtextbutton.h>
#include <libdui/dthememanager.h>

#include "constants.h"
#include "inputpassworddialog.h"

InputPasswordDialog::InputPasswordDialog(QWidget *parent) :
    QFrame(parent)
{
    D_THEME_INIT_WIDGET(InputPasswordDialog);

    m_edit = new DPasswordEdit;
    m_edit->setFixedWidth(DCC::ModuleContentWidth - 60);
    m_edit->setPlaceholderText(tr("Password"));

    m_checkBox = new DCheckBox(tr("Auto-connect"));
    m_checkBox->setChecked(true);

    QHBoxLayout *h_layout = new QHBoxLayout;
    h_layout->setMargin(0);
    h_layout->setSpacing(0);

    DTextButton *cancel_button = new DTextButton(tr("Cancel"));
    DTextButton *connect_button = new DTextButton(tr("Connect"));

    h_layout->addStretch(1);
    h_layout->addWidget(cancel_button);
    h_layout->addSpacing(5);
    h_layout->addWidget(connect_button);

    m_mainWidget = new DVBoxWidget(this);

    m_mainWidget->layout()->setMargin(10);
    m_mainWidget->layout()->setSpacing(10);
    m_mainWidget->addWidget(m_edit);
    m_mainWidget->addWidget(m_checkBox);
    m_mainWidget->layout()->addLayout(h_layout);

    //m_topSeparator = new DSeparatorHorizontal(this);
    //m_bottomSeparator = new DSeparatorHorizontal(this);

    updateMainWidgetPos();

    connect(cancel_button, &DTextButton::clicked, this, &InputPasswordDialog::cancel);
    connect(connect_button, &DTextButton::clicked, this, &InputPasswordDialog::confirm);
    connect(this, &InputPasswordDialog::setText, m_edit, &DPasswordEdit::setPassword);
    connect(m_edit, &DPasswordEdit::textChanged, this, &InputPasswordDialog::textChanged);
    connect(m_mainWidget, &DVBoxWidget::sizeChanged, this, [this]{
        setFixedHeight(m_mainWidget->height());
        updateMainWidgetPos();
    });
}

bool InputPasswordDialog::autoConnect() const
{
    return m_checkBox->checkState() != Qt::Unchecked;
}

QString InputPasswordDialog::text() const
{
    return m_edit->text();
}

void InputPasswordDialog::setInputAlert(bool alert)
{
    m_edit->setAlertMode(alert);
}

int InputPasswordDialog::exec()
{
    QEventLoop loop(this);

    connect(this, &InputPasswordDialog::cancel, &loop, [&loop]{loop.exit(0);});
    connect(this, &InputPasswordDialog::confirm, &loop, [&loop]{loop.exit(1);});

    return loop.exec(QEventLoop::DialogExec);
}

void InputPasswordDialog::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);

//    m_topSeparator->setFixedWidth(width());
//    m_bottomSeparator->setFixedWidth(width());
//    m_bottomSeparator->move(0, height() - m_bottomSeparator->height());

    updateMainWidgetPos();
}

void InputPasswordDialog::updateMainWidgetPos()
{
    m_mainWidget->move(width() / 2 - m_mainWidget->width() / 2, 0);
}
