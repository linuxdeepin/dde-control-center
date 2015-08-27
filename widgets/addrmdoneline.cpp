#include <QDebug>

#include "dynamiclabel.h"
#include "addrmdoneline.h"
#include "imagenamebutton.h"

AddRmDoneLine::AddRmDoneLine(QWidget *parent) :
    DHeaderLine(parent)
{
    QFrame *frame = new QFrame(this);
    QHBoxLayout *layout = new QHBoxLayout;
    m_removeButton = new ImageNameButton("delete");
    m_addButton = new ImageNameButton("add");
    m_doneButton = new DTextButton(tr("Done"));
    m_doneButton->hide();

    DynamicLabel *dynamic_label = new DynamicLabel;

    QLabel *label = findChild<QLabel*>("DHeaderLineTitle");

    layout->setMargin(0);
    layout->addStretch(1);
    layout->addWidget(dynamic_label);
    layout->addWidget(m_removeButton);
    layout->addWidget(m_addButton);
    layout->addWidget(m_doneButton);

    connect(m_removeButton, &ImageNameButton::clicked, this, &AddRmDoneLine::removeClicked);
    connect(m_addButton, &ImageNameButton::clicked, this, &AddRmDoneLine::addClicked);
    connect(m_doneButton, &DTextButton::clicked, this, &AddRmDoneLine::doneClicked);
    connect(this, &AddRmDoneLine::setAddHidden, m_addButton, &ImageNameButton::setHidden);
    connect(this, &AddRmDoneLine::setRemoveHidden, m_removeButton, &ImageNameButton::setHidden);
    connect(this, &AddRmDoneLine::setDoneHidden, m_doneButton, &DTextButton::setHidden);
    connect(dynamic_label, &DynamicLabel::hideFinished, label, &QLabel::show);
    connect(m_removeButton, &DImageButton::stateChanged, [=](){
        if(m_removeButton->getState() == DImageButton::Hover){
            if(m_rmButtonToolTip.isEmpty())
                return;

            dynamic_label->setText(m_rmButtonToolTip);
            if(label)
                label->hide();
            dynamic_label->showLabel();
        }else{
            dynamic_label->hideLabel();
        }
    });

    connect(m_addButton, &DImageButton::stateChanged, [=](){
        if(m_addButton->getState() == DImageButton::Hover){
            if(m_addButtonToolTip.isEmpty())
                return;

            dynamic_label->setText(m_addButtonToolTip);
            if(label)
                label->hide();
            dynamic_label->showLabel();
        }else{
            dynamic_label->hideLabel();
        }
    });

    frame->setLayout(layout);

    setContent(frame);
    setFixedHeight(EXPAND_HEADER_HEIGHT);
}

ImageNameButton *AddRmDoneLine::removeButton() const
{
    return m_removeButton;
}

ImageNameButton *AddRmDoneLine::addButton() const
{
    return m_addButton;
}

DTextButton *AddRmDoneLine::doneButton() const
{
    return m_doneButton;
}

QString AddRmDoneLine::rmButtonToolTip() const
{
    return m_rmButtonToolTip;
}

QString AddRmDoneLine::addButtonToolTip() const
{
    return m_addButtonToolTip;
}

QString AddRmDoneLine::doneButtonToolTip() const
{
    return m_doneButtonToolTip;
}

void AddRmDoneLine::setRmButtonToolTip(QString rmButtonToolTip)
{
    m_rmButtonToolTip = rmButtonToolTip;
}

void AddRmDoneLine::setAddButtonToolTip(QString addButtonToolTip)
{
    m_addButtonToolTip = addButtonToolTip;
}

void AddRmDoneLine::setDoneButtonToolTip(QString doneButtonToolTip)
{
    m_doneButtonToolTip = doneButtonToolTip;
}

