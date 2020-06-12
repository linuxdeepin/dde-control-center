#include "manualrestore.h"
#include "backupandrestoremodel.h"

#include <DApplicationHelper>

#include <QVBoxLayout>
#include <QLabel>
#include <DBackgroundGroup>
#include <QPushButton>
#include <QDir>
#include <QProcess>
#include <QRadioButton>
#include <QCheckBox>
#include <QProcess>
#include <QFile>
#include <QSettings>
#include <QDebug>
#include <QSharedPointer>
#include <DDialog>
#include <DDBusSender>
#include <QCryptographicHash>

using namespace DCC_NAMESPACE::systeminfo;

class RestoreItem : public QWidget {
    Q_OBJECT
public:
    explicit RestoreItem(QWidget* parent = nullptr)
        : QWidget(parent)
        , m_layout(new QVBoxLayout)
        , m_radioBtn(new QRadioButton)
        , m_content(nullptr)
    {
        m_radioBtn->setCheckable(true);
        m_radioBtn->setMinimumSize(24, 24);

        QVBoxLayout* mainLayout = new QVBoxLayout;
        mainLayout->setMargin(0);
        mainLayout->setSpacing(0);

        mainLayout->addWidget(m_radioBtn, 0, Qt::AlignVCenter | Qt::AlignLeft);
        mainLayout->addLayout(m_layout);

        m_layout->setMargin(0);
        m_layout->setSpacing(5);

        setLayout(mainLayout);
    }

    QRadioButton* radioButton() const {
        return m_radioBtn;
    }

    void setContent(QWidget* content) {
        if (m_content) {
            m_layout->removeWidget(m_content);
            m_content->hide();
            m_content->setParent(nullptr);
            disconnect(m_radioBtn, &QRadioButton::toggled, m_content, &QWidget::setEnabled);
        }

        m_content = content;
        connect(m_radioBtn, &QRadioButton::toggled, m_content, &QWidget::setEnabled);
        content->setEnabled(m_radioBtn->isChecked());

        m_layout->addWidget(m_content);
    }

    void setTitle(const QString& title) {
        m_radioBtn->setText(title);
    }

    bool checked() const {
        return m_radioBtn->isChecked();
    }

    void setChecked(bool check) {
        m_radioBtn->setChecked(check);
        m_content->setEnabled(check);
    }

private:
    QVBoxLayout* m_layout;
    QRadioButton* m_radioBtn;
    QWidget* m_content;
};

ManualRestore::ManualRestore(BackupAndRestoreModel* model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_saveUserDataCheckBox(new QCheckBox)
    , m_directoryChooseWidget(new DFileChooserEdit)
    , m_tipsLabel(new DTipLabel)
    , m_backupBtn(new QPushButton(tr("Restore")))
    , m_actionType(ActionType::RestoreSystem)
    , m_loadingIndicator(new DWaterProgress)
{
    m_tipsLabel->setWordWrap(true);

    auto pa = DApplicationHelper::instance()->palette(m_tipsLabel);
    pa.setBrush(DPalette::TextTips, Qt::red);
    DApplicationHelper::instance()->setPalette(m_tipsLabel, pa);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(5);

    auto createBgGroup = [=](QLayout* layout) -> DBackgroundGroup* {
        DBackgroundGroup* bgGroup = new DBackgroundGroup;
        bgGroup->setLayout(layout);
        bgGroup->setBackgroundRole(QPalette::Window);
        bgGroup->setItemSpacing(1);
        return bgGroup;
    };

    m_systemRestore = new RestoreItem;
    m_manualRestore = new RestoreItem;

    // restore system
    {
        QHBoxLayout* chooseLayout = new QHBoxLayout;
        chooseLayout->setMargin(0);
        chooseLayout->setSpacing(0);
        chooseLayout->addSpacing(5);
        chooseLayout->addWidget(m_saveUserDataCheckBox);
        m_saveUserDataCheckBox->setMinimumSize(24, 24);
        m_saveUserDataCheckBox->setText(tr("Save User Data"));

        m_systemRestore->setTitle(tr("Reset All Settings"));

        QWidget* bgWidget = new QWidget;
        bgWidget->setLayout(chooseLayout);

        m_systemRestore->setContent(bgWidget);

        QVBoxLayout* bgLayout = new QVBoxLayout;
        bgLayout->addWidget(m_systemRestore);
        mainLayout->addWidget(createBgGroup(bgLayout));
    }

    // restore system end

    // manual restore
    {
        QHBoxLayout* chooseLayout = new QHBoxLayout;
        chooseLayout->setMargin(0);
        chooseLayout->setSpacing(0);
        chooseLayout->addSpacing(5);
        chooseLayout->addWidget(new QLabel(tr("Select restore directory")), 0, Qt::AlignVCenter);
        chooseLayout->addSpacing(5);
        chooseLayout->addWidget(m_directoryChooseWidget, 0, Qt::AlignVCenter);

        QWidget* bgWidget = new QWidget;
        bgWidget->setLayout(chooseLayout);

        m_manualRestore->setContent(bgWidget);
        m_manualRestore->setTitle(tr("Manual Restore"));

        QVBoxLayout* bgLayout = new QVBoxLayout;
        bgLayout->addWidget(m_manualRestore);
        mainLayout->addWidget(createBgGroup(bgLayout));
    }

    // manual restore end

    mainLayout->addWidget(m_tipsLabel);
    mainLayout->addStretch();
    mainLayout->addWidget(m_backupBtn);
    mainLayout->addWidget(m_loadingIndicator, 0, Qt::AlignHCenter);

    setLayout(mainLayout);

    m_tipsLabel->hide();

    m_directoryChooseWidget->setFileMode(QFileDialog::Directory);

    QString radioButtonStyleSheet("border-style: none; outline: 0px;");
    m_systemRestore->radioButton()->setStyleSheet(radioButtonStyleSheet);
    m_manualRestore->radioButton()->setStyleSheet(radioButtonStyleSheet);
    m_systemRestore->radioButton()->setChecked(true);

    connect(m_backupBtn, &QPushButton::clicked, this, &ManualRestore::restore, Qt::QueuedConnection);
    connect(m_systemRestore->radioButton(), &QRadioButton::toggled, this, &ManualRestore::onItemChecked);
    connect(m_manualRestore->radioButton(), &QRadioButton::toggled, this, &ManualRestore::onItemChecked);
    connect(model, &BackupAndRestoreModel::manualRestoreErrorTypeChanged, this, &ManualRestore::onManualRestoreErrorChanged);
    connect(model, &BackupAndRestoreModel::restoreButtonEnabledChanged, this, [=](bool enable) {
        m_backupBtn->setVisible(enable);
        m_loadingIndicator->setVisible(!enable);
    });

    m_directoryChooseWidget->lineEdit()->setText(model->restoreDirectory());
    m_saveUserDataCheckBox->setChecked(model->formatData());
    onManualRestoreErrorChanged(model->manualRestoreErrorType());

    m_backupBtn->setVisible(model->restoreButtonEnabled());
    m_loadingIndicator->setVisible(!model->restoreButtonEnabled());
    m_loadingIndicator->setValue(50);
    m_loadingIndicator->setTextVisible(false);
    m_loadingIndicator->setFixedSize(48, 48);
    m_loadingIndicator->start();
}

void ManualRestore::setTipsVisible(const bool &visible)
{
    m_tipsLabel->setVisible(visible);
}

void ManualRestore::onItemChecked()
{
    QRadioButton* button = qobject_cast<QRadioButton*>(sender());

    const bool check = button == m_systemRestore->radioButton();

    auto setCheck = [&](RestoreItem* item, bool c) {
        item->radioButton()->blockSignals(true);
        item->setChecked(c);
        item->radioButton()->blockSignals(false);
    };

    setCheck(m_systemRestore, check);
    setCheck(m_manualRestore, !check);

    m_actionType = check ? ActionType::RestoreSystem : ActionType::ManualRestore;
}

void ManualRestore::onManualRestoreErrorChanged(ErrorType errorType)
{
    m_tipsLabel->setVisible(true);

    switch (errorType) {
        case ErrorType::MD5Error: {
            m_tipsLabel->setText(tr("Backup file is invalid"));
            break;
        }
        case ErrorType::GrubError: {
            m_tipsLabel->setText(tr("Grub authentication failed"));
            break;
        }
        default: {
            m_tipsLabel->setVisible(false);
            break;
        }
    }
}

void ManualRestore::restore()
{
    if (m_actionType == ActionType::RestoreSystem) {
        const bool formatData = !m_saveUserDataCheckBox->isChecked();

        DDialog dialog;
        QString message{ tr("It will reset system settings to their defaults without affecting your files and apps, but the username and password will be cleared, please confirm before proceeding") };

        if (formatData) {
            message =
                tr("It will reinstall the system and clear all user data, which is highly risky, please confirm before proceeding");
        }

        dialog.setMessage(message);
        dialog.addButton(tr("Cancel"));
        int buttonIndex = dialog.addButton(tr("Confirm"), true, DDialog::ButtonWarning);
        if (buttonIndex != dialog.exec()) {
            return;
        }

        Q_EMIT requestSystemRestore(formatData);
        m_loadingIndicator->setVisible(true);
        m_backupBtn->setVisible(false);
    }

    if (m_actionType == ActionType::ManualRestore) {
        m_tipsLabel->hide();

        // TODO(justforlxz): 判断内容的有效性
        const QString& selectPath = m_directoryChooseWidget->lineEdit()->text();

        if (selectPath.isEmpty()) {
            // TODO(justforlxz): 这里要更换成相应的错误，应该是ErrorType::PathError
            return onManualRestoreErrorChanged(ErrorType::MD5Error);
        }

        Q_EMIT requestManualRestore(selectPath);
        m_loadingIndicator->setVisible(true);
        m_backupBtn->setVisible(false);
    }
    setFocus();
}

#include "manualrestore.moc"
