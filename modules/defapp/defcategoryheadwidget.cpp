#include "defcategoryheadwidget.h"
#include <QHBoxLayout>

DefCategoryHeadWidget::DefCategoryHeadWidget(const QString &name) {

    setFixedHeight(30);
    m_title = new QLabel;
    m_title->setText(name);

    m_edit = new FuncButton;
    m_edit->setAlignment(Qt::AlignCenter);
    m_edit->setStyleSheet("QLabel{color: #01bdff}");
    m_edit->setText(tr("Edit"));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(15, 0, 20, 0);
    mainLayout->addWidget(m_title);
    mainLayout->addStretch();
    mainLayout->addWidget(m_edit);

    setLayout(mainLayout);

    connect(m_edit, &FuncButton::clicked, this, &DefCategoryHeadWidget::clicked);
}

void DefCategoryHeadWidget::setedit(const bool edit)
{
    if (edit) {
        m_edit->setText(tr("Edit"));
        m_editState = edit;
        emit editChanged(false);
    }
}

void DefCategoryHeadWidget::clicked()
{
    if(m_editState) {
        m_edit->setText("Cancel");
    } else {
        m_edit->setText("Edit");
    }
    emit editChanged(m_editState);
    m_editState = !m_editState;
}
