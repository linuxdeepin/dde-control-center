#include "defcategoryaddwidget.h"
#include <QHBoxLayout>
#include <QMouseEvent>


DefCategoryAddWidget::DefCategoryAddWidget(QObject *parent)
{
    Q_UNUSED(parent);
    setFixedHeight(30);
    m_add = new FuncButton;
    m_add->setAlignment(Qt::AlignCenter);
    m_add->setText(tr("Add"));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(15,0,20,0);
    mainLayout->addWidget(m_add);
    mainLayout->addStretch();

    setLayout(mainLayout);
    connect(m_add, &FuncButton::clicked, this, &DefCategoryAddWidget::clicked);
}

void DefCategoryAddWidget::clicked()
{
    //暂时使用该对话框，测试添加代码
    if(isEnabled()) {
        emit requestFrameAutoHide(false);
        OpenWithOtherDialog * dialog = new OpenWithOtherDialog;
        if(dialog->exec() == DDialog::Accepted) {
            QString path = dialog->getDesktopFile();
//            emit addUserItem(m_category,path);
            QTimer::singleShot(1000, this, [=] { emit requestFrameAutoHide(true); });
        }
        delete dialog;
    }
}


