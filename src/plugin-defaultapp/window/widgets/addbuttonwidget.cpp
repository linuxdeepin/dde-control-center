#include "addbuttonwidget.h"
#include "category.h"
#include "defappmodel.h"

#include <DFloatingButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

AddButtonWidget::AddButtonWidget(DefAppWorker::DefaultAppsCategory category, QWidget *parent)
    : QWidget(parent)
    , m_addBtn(new DFloatingButton(DStyle::SP_IncreaseElement))
    , m_categoryValue(category)
    , m_category(nullptr)
    , m_createFile(new QFileDialog)
{
    QVBoxLayout *centralLayout = new QVBoxLayout;

    centralLayout->addWidget(m_addBtn, 0, Qt::AlignHCenter | Qt::AlignBottom);
    setLayout(centralLayout);

    connect(m_addBtn, &Dtk::Widget::DFloatingButton::clicked, this, &AddButtonWidget::onAddBtnClicked);
    m_addBtn->setToolTip(tr("Add Application"));
    m_createFile->setModal(true);
    m_createFile->setWindowTitle(tr("Open Desktop file"));
    m_createFile->setAcceptMode(QFileDialog::AcceptOpen);
    QStringList directory = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    if (!directory.isEmpty())
        m_createFile->setDirectory(directory.first());

    connect(m_createFile, &QFileDialog::finished, this, [ = ](int result) {
        Q_EMIT requestFrameAutoHide(true);
        if (result == QFileDialog::Accepted) {
            QString path = m_createFile->selectedFiles().first();
            if (path.isEmpty())
                return;

            QFileInfo info(path);
            Q_EMIT requestCreateFile(m_categoryName, info);
        }
    });
}

AddButtonWidget::~AddButtonWidget()
{
    if (m_createFile)
        m_createFile->deleteLater();
}

void AddButtonWidget::setModel(DefAppModel * const model)
{
    switch (m_categoryValue) {
    case DefAppWorker::Browser:
        setCategory(model->getModBrowser());
        break;
    case DefAppWorker::Mail:
        setCategory(model->getModMail());
        break;
    case DefAppWorker::Text:
        setCategory(model->getModText());
        break;
    case DefAppWorker::Music:
        setCategory(model->getModMusic());
        break;
    case DefAppWorker::Video:
        setCategory(model->getModVideo());
        break;
    case DefAppWorker::Picture:
        setCategory(model->getModPicture());
        break;
    case DefAppWorker::Terminal:
        setCategory(model->getModTerminal());
        break;
    default:
        break;
    }
}

void AddButtonWidget::setCategory(Category * const category)
{
    m_category = category;
    connect(m_category, &Category::categoryNameChanged, this, &AddButtonWidget::setCategoryName);
    setCategoryName(m_category->getName());
}

void AddButtonWidget::setCategoryName(const QString &name)
{
    m_categoryName = name;
}

void AddButtonWidget::onAddBtnClicked()
{
    Q_EMIT requestFrameAutoHide(false);
    m_createFile->show();
}

