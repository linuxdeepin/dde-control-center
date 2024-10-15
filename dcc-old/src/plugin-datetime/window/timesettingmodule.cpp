//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "timesettingmodule.h"
#include "datetimemodel.h"
#include "datetimeworker.h"
#include "clock.h"
#include "datewidget.h"
#include "widgets/widgetmodule.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "widgets/comboxwidget.h"
#include "widgets/lineeditwidget.h"
#include "widgets/buttontuple.h"

#include <DIconButton>
#include <DLineEdit>
#include <DBackgroundGroup>

#include <QLineEdit>
#include <QTimer>
#include <QPushButton>
#include <QSpinBox>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(DdcDateTimeTimeSettingModule, "dcc-datetime-timesettingmodule")

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

const static int SpinBtnLength = 26;

class TimeSpinBox : public QSpinBox
{
public:
    explicit TimeSpinBox(QWidget *parent = nullptr)
        : QSpinBox(parent)
    {
        this->lineEdit()->setMaxLength(2);
    }

protected:
    QString textFromValue(int value) const override
    {
        return QString("%1").arg(value, 2, 10, QLatin1Char('0'));
    }
};

TimeSettingModule::TimeSettingModule(DatetimeModel *model, DatetimeWorker *work, QObject *parent)
    : PageModule("timeSettings", tr("Time Settings"), parent)
    , m_model(model)
    , m_work(work)
{
    deactive();
    appendChild(new WidgetModule<Clock>("time", tr("Time")));
    appendChild(new WidgetModule<SettingsGroup>("ntpServer", tr("Auto Sync"), this, &TimeSettingModule::initAutoSyncTime));
    appendChild(new WidgetModule<SettingsGroup>("time", QString(), this, &TimeSettingModule::initTimeSetting));
    appendChild(new WidgetModule<QWidget>("datetime", QString(), this, &TimeSettingModule::initDigitalClock));

    ModuleObject *saveButton = new WidgetModule<ButtonTuple>("datetimeDatesettingConfirmbtn","",[this](ButtonTuple *buttonTuple){
        m_buttonTuple = buttonTuple;
        m_buttonTuple->setButtonType(ButtonTuple::Save);
        QPushButton *cancelButton = m_buttonTuple->leftButton();
        QPushButton *confirmButton = m_buttonTuple->rightButton();
        cancelButton->setText(tr("Reset"));
        confirmButton->setText(tr("Save"));
        connect(cancelButton, &QPushButton::clicked, this, &TimeSettingModule::onCancelButtonClicked);
        connect(cancelButton, &QPushButton::clicked, this, &TimeSettingModule::onCancelButtonClicked);
        connect(confirmButton, &QPushButton::clicked, this, &TimeSettingModule::onConfirmButtonClicked);
        connect(cancelButton, &QPushButton::clicked, this, [this] {
            setBtnEnable(false);
        });
        connect(confirmButton, &QPushButton::clicked, this, [this] {
            setBtnEnable(false);
        });
        setButtonShowState(m_model->nTP());
        setBtnEnable(false);
    });
    saveButton->setExtra();
    appendChild(saveButton);

    connect(this, &TimeSettingModule::requestNTPServer, m_work, &DatetimeWorker::setNtpServer);
    connect(this, &TimeSettingModule::requestSetTime, m_work, &DatetimeWorker::setDatetime);
}

void TimeSettingModule::deactive()
{
    m_autoSyncTimeSwitch = nullptr;
    m_datetimeGroup = nullptr;
    m_timeHourWidget = nullptr;
    m_timeMinWidget = nullptr;
    m_yearWidget = nullptr;
    m_monthWidget = nullptr;
    m_dayWidget = nullptr;
    m_buttonTuple = nullptr;
}

void TimeSettingModule::initAutoSyncTime(SettingsGroup *ntpGroup)
{
    ntpGroup->setBackgroundStyle(SettingsGroup::GroupBackground);
    m_autoSyncTimeSwitch = new SwitchWidget(ntpGroup);
    m_autoSyncTimeSwitch->setTitle(tr("Auto Sync"));

    m_ntpServerList = new ComboxWidget(ntpGroup);
    m_ntpServerList->setTitle(tr("Server"));
    m_ntpServerList->comboBox()->setMinimumWidth(240);

    m_customizeAddress = new LineEditWidget(ntpGroup);
    m_customizeAddress->setTitle(tr("Address"));
    m_customizeAddress->textEdit()->setMinimumWidth(240);
    m_customizeAddress->textEdit()->setPlaceholderText(tr("Required"));

    m_ntpServerList->comboBox()->addItems(m_model->ntpServerList());
    m_ntpServerList->comboBox()->addItem(tr("Customize"));

    auto setNtpServer = [this](QString server) {
        const QStringList &ntpServerList = m_model->ntpServerList();
        m_ntpServerList->comboBox()->blockSignals(true);
        m_customizeAddress->blockSignals(true);
        if (server.isEmpty()) {
            m_ntpServerList->comboBox()->setCurrentIndex(0);
            m_customizeAddress->setVisible(false);
        } else if (ntpServerList.contains(server)) {
            m_ntpServerList->comboBox()->setCurrentText(server);
            m_customizeAddress->setVisible(false);
        } else {
            m_ntpServerList->comboBox()->setCurrentText(tr("Customize"));
            m_customizeAddress->setText(server);
            m_customizeAddress->setVisible(true);
        }
        m_ntpServerList->comboBox()->blockSignals(false);
        m_customizeAddress->blockSignals(false);
    };
    setNtpServer(m_model->ntpServerAddress());
    connect(m_model, &DatetimeModel::NTPServerChanged, m_ntpServerList, setNtpServer);
    connect(m_model, &DatetimeModel::NTPServerNotChanged, m_ntpServerList, setNtpServer);

    const bool isNtp = m_model->nTP();
    m_autoSyncTimeSwitch->setChecked(isNtp);
    connect(m_autoSyncTimeSwitch, &SwitchWidget::checkedChanged, m_work, &DatetimeWorker::setNTP);
    connect(m_model, &DatetimeModel::NTPChanged, this, &TimeSettingModule::setControlVisible);
    connect(m_autoSyncTimeSwitch, &SwitchWidget::checkedChanged, this, [this] {
        setBtnEnable(false);
    });

    connect(m_ntpServerList->comboBox(), QOverload<const int>::of(&QComboBox::currentIndexChanged), this, [this](const int index) {
        const QString &text = m_ntpServerList->comboBox()->itemText(index);
        m_customizeAddress->setVisible(m_ntpServerList->isVisible() && text == tr("Customize"));
        isUserOperate();
        if (m_autoSyncTimeSwitch->checked()) {
            if (text == tr("Customize"))
                m_customizeAddress->setText(m_customNtpServer);

            if (m_customizeAddress->isShowAlert()) {
                m_customizeAddress->hideAlertMessage();
            }
        }
        if (!m_bIsUserOperate)
            return;

        m_bIsUserOperate = false;

        if (text != tr("Customize")) {
            if ("" != text) {
                Q_EMIT requestNTPServer(text);
            }
        } else if (!m_customizeAddress->text().isEmpty()) {
            Q_EMIT requestNTPServer(m_customNtpServer);
        }

        setButtonShowState(m_autoSyncTimeSwitch->checked());
    });
    connect(m_customizeAddress->dTextEdit(), &DLineEdit::focusChanged, this, [=] {
        m_buttonTuple->rightButton()->setEnabled(true);
    });
    m_ntpServerList->setVisible(isNtp);
    m_customizeAddress->setVisible(isNtp && m_ntpServerList->comboBox()->currentText() == tr("Customize"));
    ntpGroup->appendItem(m_autoSyncTimeSwitch);
    ntpGroup->appendItem(m_ntpServerList);
    ntpGroup->appendItem(m_customizeAddress);
}

void TimeSettingModule::initTimeSetting(SettingsGroup *datetimeGroup)
{
    m_datetimeGroup = datetimeGroup;

    datetimeGroup->setHidden(m_model->nTP());
    connect(m_model, &DatetimeModel::NTPChanged, datetimeGroup, &SettingsGroup::setHidden);
    QLabel *centerLabel = new QLabel(" : ");
    QFont font;
    font.setPointSizeF(24);
    centerLabel->setFont(font);
    centerLabel->setContextMenuPolicy(Qt::NoContextMenu);

    QTime time(QTime::currentTime());
    m_timeHourWidget = createDSpinBox(datetimeGroup, 0, 23);
    m_timeMinWidget = createDSpinBox(datetimeGroup, 0, 59);
    m_timeHourWidget->setValue(time.hour());
    m_timeMinWidget->setValue(time.minute());
    m_timeHourWidget->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_timeMinWidget->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_timeMinWidget->setAccessibleName("TIME_MIN_WIDGET");
    m_timeHourWidget->setAccessibleName("TIME_HOUR_WIDGET");

    int nIndex = QFontDatabase::addApplicationFont(":/icons/deepin/builtin/resource/deepindigitaltimes-Regular.ttf");
    if (nIndex != -1) {
        QStringList strList(QFontDatabase::applicationFontFamilies(nIndex));
        if (strList.count() > 0) {
            QFont fontThis(strList.at(0));
            fontThis.setPointSize(28);
            m_timeHourWidget->setFont(fontThis);
            m_timeMinWidget->setFont(fontThis);
        }
    }

    QHBoxLayout *timeLayout = new QHBoxLayout;
    timeLayout->addStretch();
    timeLayout->addWidget(m_timeHourWidget);
    timeLayout->addWidget(centerLabel);
    timeLayout->addWidget(m_timeMinWidget);
    timeLayout->addStretch();
    SettingsItem *w = new SettingsItem(datetimeGroup);
    w->addBackground();
    w->setLayout(timeLayout);

    m_yearWidget = new DateWidget(DateWidget::Year, QDate::currentDate().year() - 30, QDate::currentDate().year() + 30);
    m_monthWidget = new DateWidget(DateWidget::Month, 1, 12);
    m_dayWidget = new DateWidget(DateWidget::Day, 1, 31);
    QDate currentDate(QDate::currentDate());
    m_yearWidget->setValue(currentDate.year());
    m_yearWidget->setAccessibleName("yearwidget");
    m_yearWidget->addBackground();
    m_monthWidget->setValue(currentDate.month());
    m_monthWidget->setAccessibleName("monthwidget");
    m_monthWidget->addBackground();
    m_dayWidget->setValue(currentDate.day());
    m_dayWidget->setAccessibleName("daywidget");
    m_dayWidget->addBackground();

    datetimeGroup->insertWidget(w);
    datetimeGroup->insertWidget(m_yearWidget);
    datetimeGroup->insertWidget(m_monthWidget);
    datetimeGroup->insertWidget(m_dayWidget);

    auto updateDayRange = [this]() {
        const int year = m_yearWidget->value();
        const int month = m_monthWidget->value();

        QDate date(year, month, 1);
        m_dayWidget->setRange(1, date.daysInMonth());
        qCDebug(DdcDateTimeTimeSettingModule) << " year : " << year << " , month : " << month << " day range : 1 to " << date.daysInMonth();
        if (m_dayWidget->maximum() < m_dayWidget->getCurrentText().toInt()) {
            m_dayWidget->setCurrentText(QString::number(m_dayWidget->maximum()));
        }
    };
    connect(m_monthWidget, &DateWidget::editingFinished, this, updateDayRange);
    connect(m_monthWidget, &DateWidget::notifyClickedState, this, updateDayRange);
    connect(m_yearWidget, &DateWidget::editingFinished, this, updateDayRange);
    connect(m_yearWidget, &DateWidget::notifyClickedState, this, updateDayRange);
    updateDayRange();
    datetimeGroup->setVisible(!m_model->nTP());
    connect(m_timeHourWidget, qOverload<int>(&QSpinBox::valueChanged), this, [this] {
        setBtnEnable(true);
    });
    connect(m_timeMinWidget, qOverload<int>(&QSpinBox::valueChanged), this, [this] {
        setBtnEnable(true);
    });
    connect(m_yearWidget, &DateWidget::chenged, this, [this] {
        setBtnEnable(true);
    });
    connect(m_monthWidget, &DateWidget::chenged, this, [this] {
        setBtnEnable(true);
    });
    connect(m_dayWidget, &DateWidget::chenged, this, [this] {
        setBtnEnable(true);
    });
}

void TimeSettingModule::initDigitalClock(QWidget *w)
{
    QLabel *centerLabel = new QLabel(" : ");
    QLabel *hourLabel = new QLabel();
    QLabel *minLabel = new QLabel();
    QLabel *yearLabel = new QLabel();
    QLabel *monthLabel = new QLabel();
    QLabel *dayLabel = new QLabel();
    centerLabel->setAlignment(Qt::AlignCenter);
    hourLabel->setAlignment(Qt::AlignCenter);
    minLabel->setAlignment(Qt::AlignCenter);
    yearLabel->setAlignment(Qt::AlignCenter);
    monthLabel->setAlignment(Qt::AlignCenter);
    dayLabel->setAlignment(Qt::AlignCenter);
    QFont font;
    font.setPointSizeF(24);
    centerLabel->setFont(font);
    centerLabel->setContextMenuPolicy(Qt::NoContextMenu);

    int nIndex = QFontDatabase::addApplicationFont(":/icons/deepin/builtin/resource/deepindigitaltimes-Regular.ttf");
    if (nIndex != -1) {
        QStringList strList(QFontDatabase::applicationFontFamilies(nIndex));
        if (strList.count() > 0) {
            QFont fontThis(strList.at(0));
            fontThis.setPointSize(28);
            hourLabel->setFont(fontThis);
            minLabel->setFont(fontThis);
        }
    }
    QHBoxLayout *timeLayout = new QHBoxLayout;
    timeLayout->addWidget(hourLabel);
    timeLayout->addWidget(centerLabel);
    timeLayout->addWidget(minLabel);
    QHBoxLayout *dataLayout = new QHBoxLayout;
    dataLayout->addWidget(yearLabel);
    dataLayout->addWidget(monthLabel);
    dataLayout->addWidget(dayLabel);
    DBackgroundGroup *bggroup = new DBackgroundGroup(dataLayout);
    bggroup->setAccessibleName("bggroup");
    bggroup->setBackgroundRole(QPalette::Window);
    bggroup->setItemSpacing(1);
    bggroup->setUseWidgetBackground(false);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(timeLayout);
    layout->addWidget(bggroup);
    layout->setSpacing(10);
    layout->setContentsMargins(0, 0, 0, 0);
    w->setLayout(layout);
    QTimer::singleShot(10, w, [this, w]() {
        w->setVisible(m_model->nTP());
    });
    connect(m_model, &DatetimeModel::NTPChanged, w, &QWidget::setVisible);
    QTimer *timer = new QTimer(w);
    auto updateTime = [minLabel, hourLabel, yearLabel, monthLabel, dayLabel]() {
        QDateTime datetime = QDateTime::currentDateTime();
        QTime time = QTime::currentTime();
        minLabel->setText(QString::number(time.minute()));
        hourLabel->setText(QString::number(time.hour()));
        QDate date = QDate::currentDate();
        yearLabel->setText(QString("%1 %2").arg(date.year()).arg(tr("Year")));
        monthLabel->setText(QString("%1 %2").arg(date.month()).arg(tr("Month")));
        dayLabel->setText(QString("%1 %2").arg(date.day()).arg(tr("Day")));
    };
    connect(timer, &QTimer::timeout, w, updateTime);
    timer->start(1000);
    updateTime();
}

void TimeSettingModule::setButtonShowState(bool state)
{
    if (m_customizeAddress->isShowAlert()) {
        m_customizeAddress->hideAlertMessage();
    }
    m_buttonTuple->leftButton()->setVisible(!state);
    m_buttonTuple->rightButton()->setVisible(!state || m_ntpServerList->comboBox()->currentText() == tr("Customize"));
//    m_buttonTuple->rightButton()->setText(state ? tr("Save") : tr("Confirm"));
}

void TimeSettingModule::setControlVisible(bool state)
{
    m_datetimeGroup->setVisible(!state);
    m_ntpServerList->setVisible(state);
    setButtonShowState(state);
    m_autoSyncTimeSwitch->setChecked(state);
    m_customizeAddress->setVisible(state && m_ntpServerList->comboBox()->currentText() == tr("Customize"));
}

void TimeSettingModule::onCancelButtonClicked()
{
    // 取消操作
    QDate date(QDate::currentDate());
    m_yearWidget->setValue(date.year());
    m_monthWidget->setValue(date.month());
    m_dayWidget->setValue(date.day());
    QTime time(QTime::currentTime());
    m_timeHourWidget->setValue(time.hour());
    m_timeMinWidget->setValue(time.minute());
}

void TimeSettingModule::onConfirmButtonClicked()
{
    if (m_autoSyncTimeSwitch->checked() && m_ntpServerList->comboBox()->currentText() == tr("Customize")) {
        m_buttonTuple->rightButton()->setEnabled(false);
        if (m_customizeAddress->text().isEmpty()) {
            qCDebug(DdcDateTimeTimeSettingModule) << "The customize address is nullptr.";
            m_customizeAddress->setIsErr(true);
            return;
        }
        //        this->setFocus();
        //        m_customNtpServer = m_addressContent->text();
        //        QGSettings("com.deepin.dde.control-center","/com/deepin/dde/control-center/").set("custom-ntpserver", m_customNtpServer);
        qCDebug(DdcDateTimeTimeSettingModule) << "ok clicked, requestNTPServer";
        Q_EMIT requestNTPServer(m_customizeAddress->text());
    } else {
        qCDebug(DdcDateTimeTimeSettingModule) << "ok clicked, requestSetTime";

        QDateTime datetime;
        datetime.setDate(QDate(m_yearWidget->value(), m_monthWidget->value(), m_dayWidget->value()));
        datetime.setTime(QTime(m_timeHourWidget->value(), m_timeMinWidget->value()));
        Q_EMIT requestSetTime(datetime);
    }
}

QSpinBox *TimeSettingModule::createDSpinBox(QWidget *parent, int min, int max)
{
    QSpinBox *spinBox = new TimeSpinBox(parent);
    spinBox->setFixedSize(100, 60);
    spinBox->setRange(min, max);
    spinBox->setSingleStep(1);
    spinBox->setWrapping(true);
    spinBox->setValue(0);

    DIconButton *btnUp = new DIconButton(spinBox);
    DIconButton *btnDown = new DIconButton(spinBox);
    if (max == 59) {
        btnUp->setAccessibleName("MINUP_BUTTON");
        btnDown->setAccessibleName("MINDOWM_BUTTON");
    } else {
        btnUp->setAccessibleName("HOURUP_BUTTON");
        btnDown->setAccessibleName("HOURDOWM_BUTTON");
    }
    btnUp->setIcon(DStyle::SP_ArrowUp);
    btnDown->setIcon(DStyle::SP_ArrowDown);
    btnUp->setFixedSize(QSize(SpinBtnLength, SpinBtnLength));
    btnDown->setFixedSize(QSize(SpinBtnLength, SpinBtnLength));
    btnUp->move(70, 4);
    btnDown->move(70, 31);

    connect(btnUp, &DIconButton::clicked, spinBox, &QSpinBox::stepUp);
    connect(btnDown, &DIconButton::clicked, spinBox, &QSpinBox::stepDown);

    return spinBox;
}

void TimeSettingModule::isUserOperate()
{
    if (!m_bIsUserOperate) {
        m_bIsUserOperate = true;
    }
}

void TimeSettingModule::setBtnEnable(bool state)
{
    m_buttonTuple->leftButton()->setEnabled(state);
    m_buttonTuple->rightButton()->setEnabled(state);
}
