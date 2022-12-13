// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "timezonechooser.h"
#include "timezone_map.h"
#include "widgets/searchinput.h"
#include "../datetimemodel.h"

#include <DPlatformWindowHandle>
#include <DDialogCloseButton>
#include <DApplicationHelper>
#include <DTitlebar>

#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTimeZone>
#include <QTimer>
#include <QCompleter>
#include <QKeyEvent>
#include <QDebug>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QStyleFactory>
#include <QAbstractItemView>
#include <QGraphicsOpacityEffect>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace dcc::datetime;

TimeZoneChooser::TimeZoneChooser(QWidget* parent)
    : DAbstractDialog(parent)
    , m_blurEffect(new DBlurEffectWidget(this))
    , m_map(new installer::TimezoneMap(this))
    , m_searchInput(new SearchInput(this))
    , m_title(new QLabel)
    , m_cancelBtn(new QPushButton(tr("Cancel")))
    , m_confirmBtn(new DSuggestButton(tr("Confirm")))
    , m_currLangSelector(new LangSelector("com.deepin.daemon.LangSelector",
                                          "/com/deepin/daemon/LangSelector",
                                          QDBusConnection::sessionBus(), this))
    , m_totalZones(installer::GetZoneInfoList())
    , m_model(nullptr)
{
    setAttribute(Qt::WA_TranslucentBackground);

    DPalette pa = DApplicationHelper::instance()->palette(m_title);
    pa.setBrush(QPalette::WindowText, pa.windowText());
    DApplicationHelper::instance()->setPalette(m_title, pa);

    DPlatformWindowHandle handle(this);
    handle.setWindowRadius(18);

    m_blurEffect->setAccessibleName("blurEffect");
    m_blurEffect->setBlendMode(DBlurEffectWidget::BehindWindowBlend);
    // 浅色主题时使用白色透明，让地图空白区域与界面保持一致，而深色主题时使用黑色透明与界面保持一致
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        m_blurEffect->setMaskColor(Qt::white);
    } else {
        m_blurEffect->setMaskColor(Qt::black);
    }

    //删除部分重复设置的代码，并移动部分代码到合适位置，尽量将相同功能的代码放在一起
    m_searchInput->setContentsMargins(0, 0, 0, 0);
    m_searchInput->setFixedSize(350, 36);

    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(m_searchInput);
    opacityEffect->setOpacity(0.7);
    m_searchInput->setGraphicsEffect(opacityEffect);

    m_cancelBtn->setFixedSize(200, 36);
    m_confirmBtn->setFixedSize(200, 36);
    m_confirmBtn->setEnabled(false);

    // 标题栏
    DTitlebar *titleBar = new DTitlebar(this);
    titleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    titleBar->setAccessibleName("DDialogTitleBar");
    titleBar->setMenuVisible(false);
    titleBar->setBackgroundTransparent(true);
    titleBar->setFixedHeight(50);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(m_cancelBtn, 0, Qt::AlignHCenter);
    hLayout->addSpacing(20);
    hLayout->addWidget(m_confirmBtn, 0, Qt::AlignHCenter);
    hLayout->addStretch();

    QHBoxLayout *mapLayout = new QHBoxLayout();
    mapLayout->setContentsMargins(40, 0, 40, 0);
    mapLayout->setSpacing(0);
    mapLayout->addWidget(m_map);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(titleBar);
    layout->addWidget(m_title, 0, Qt::AlignHCenter | Qt::AlignTop);
    layout->addSpacing(20);
    layout->addWidget(m_searchInput, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addLayout(mapLayout);
    layout->addSpacing(20);
    layout->addLayout(hLayout);
    layout->addSpacing(20);

    setLayout(layout);

    setupSize();

    connect(m_confirmBtn, &DSuggestButton::clicked, [this] {
        QString zone = m_map->getTimezone();
        Q_EMIT confirmed(zone);
    });

    connect(m_cancelBtn, &QPushButton::clicked, this, [this] {
        close();
    });

    connect(m_searchInput, &SearchInput::editingFinished, [this] {
        QString timezone = m_searchInput->text();
        timezone = m_completionCache.value(timezone, timezone);
        if (m_map->setTimezone(timezone) && !m_confirmBtn->isEnabled())
            qInfo() << " SearchInput editingFinished, currentZone : " << m_model->getTimeZone() << timezone;
            m_confirmBtn->setEnabled(m_model->getTimeZone() != timezone);
    });

    connect(m_searchInput, &SearchInput::textChanged, m_searchInput, &SearchInput::editingFinished);

    connect(m_map, &installer::TimezoneMap::timezoneUpdated, this, [this] (const QString & _t1) {
        setRightBtnState(_t1);
    });
    connect(m_map, &installer::TimezoneMap::notifyPopupWindowVisibleChanged, this, [this]() {
        setRightBtnState();
    });

    QTimer::singleShot(0, [this] {
        QStringList completions;
        QStringList completions_filter;
        for (const auto &zoneInfo : m_totalZones) {
            auto timezone = zoneInfo.timezone;
            completions << timezone; // timezone as completion candidate.

            // localized timezone as completion candidate.
            const QString locale = QLocale::system().name();
            QString localizedTimezone = installer::GetLocalTimezoneName(timezone, locale);
            completions << localizedTimezone;

            m_completionCache[localizedTimezone] = timezone;
        }

        if ("en_US.UTF-8" == m_currLangSelector->currentLocale()) {
            int i = 0;
            for (auto str : completions) {
                if ( i++ % 2 == 0) continue;
                completions_filter << str;
            }
            m_completer = new QCompleter(completions_filter, m_searchInput);
        } else {
            m_completer = new QCompleter(completions, m_searchInput);
        }

        m_completer->popup()->setAttribute(Qt::WA_InputMethodEnabled);
        m_completer->setCompletionMode(QCompleter::PopupCompletion);
        m_completer->setCaseSensitivity(Qt::CaseInsensitive);
        m_completer->setFilterMode(Qt::MatchContains);

        m_searchInput->setCompleter(m_completer);

        m_popup = m_completer->popup();
        m_popup->setObjectName("TimezoneCompleter");
        m_popup->setAttribute(Qt::WA_TranslucentBackground);
        m_popup->installEventFilter(this);

        DBlurEffectWidget *blurEffect = new DBlurEffectWidget(m_popup);
        blurEffect->setAccessibleName("blurEffect");
        blurEffect->setMaskColor(Qt::white);

        QHBoxLayout *layout = new QHBoxLayout;
        layout->setSpacing(0);
        layout->setMargin(0);
        layout->addWidget(blurEffect);
        m_popup->setLayout(layout);

        blurEffect->lower();
    });
    connect(m_searchInput, &SearchInput::returnPressed, [this] {
        QModelIndex index = m_popup->model()->index(0, 0);
        if (index.isValid()) {
            m_searchInput->setText(index.data().toString());
            m_popup->close();
        }
    });
}

void TimeZoneChooser::setIsAddZone(const bool isAdd)
{
    m_isAddZone = isAdd;

    if (isAdd) {
        m_title->setText(tr("Add Timezone"));
        m_confirmBtn->setText(tr("Add"));
    } else {
        m_title->setText(tr("Change Timezone"));
        m_confirmBtn->setText(tr("Confirm"));
    }
}

void TimeZoneChooser::setCurrentTimeZoneText(const QString &zone)
{
    if (zone.isEmpty()) return;

    const QString locale = QLocale::system().name();
    const QString name = installer::GetLocalTimezoneName(zone, locale);
}

void TimeZoneChooser::setMode(DatetimeModel *model)
{
    m_model = model;
    m_map->setSystemActiveColor(m_model->systemActiveColor());
}

void TimeZoneChooser::setMarkedTimeZone(const QString &timezone)
{
    m_map->setTimezone(timezone);
}

void TimeZoneChooser::setRightBtnState(QString zone)
{
    m_searchInput->setText("");
    m_searchInput->clearFocus();

    if (zone == "") {
        m_confirmBtn->setEnabled(false);
    } else {
        m_confirmBtn->setEnabled(m_model->getTimeZone() != zone);
    }
}

void TimeZoneChooser::resizeEvent(QResizeEvent *event)
{
    DAbstractDialog::resizeEvent(event);

    m_blurEffect->resize(event->size());
    m_blurEffect->lower();
}

void TimeZoneChooser::keyReleaseEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Cancel)) {
       // Q_EMIT cancelled();

        close();
    }
}

bool TimeZoneChooser::eventFilter(QObject *watched, QEvent *event)
{
    // Qt make popups' position 2px higher than the bottom of its associated widget,
    // and the 2px's hard coded, so I need to move the popup to a proper position so
    // it won't overlap with the SearchInput.
    if (watched == m_popup && event->type() == QEvent::Move) {
        const QMoveEvent *move = static_cast<QMoveEvent *>(event);
        const QPoint destPos = m_searchInput->mapToGlobal(QPoint(0, m_searchInput->height() + 1));
        // Don't panic, it won't cause dead loop.
        if (move->pos() != destPos) {
            m_popup->move(destPos);
        }
    }

    return false;
}

void TimeZoneChooser::showEvent(QShowEvent *event)
{
    DAbstractDialog::showEvent(event);

    move(qApp->primaryScreen()->geometry().center() - rect().center());
}

void TimeZoneChooser::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    return;
}

QSize TimeZoneChooser::getScreenSize() const
{
    const QDesktopWidget *desktop = QApplication::desktop();
    const QRect primaryRect = desktop->availableGeometry(desktop->primaryScreen());

    // 界面和屏幕左右上下有20像素间隔,再减去任务栏在左右或上下位置时所占区域
    double width = primaryRect.width() - 100 - 20 * 2;
    double height = primaryRect.height() - 100/* dock */ - 20 * 2;

    return QSize(static_cast<int>(width), static_cast<int>(height));
}

int TimeZoneChooser::getFontSize() const
{
    //根据屏幕大小设置标题字体大小，不至于因为屏幕太小时，字段显得太大
    const QDesktopWidget *desktop = QApplication::desktop();
    const QRect primaryRect = desktop->availableGeometry(desktop->primaryScreen());

    if (primaryRect.width() <= 1024) {
        return 24;
    } else if (primaryRect.width() <= 1440) {
        return 28;
    } else {
        return 32;
    }
}

void TimeZoneChooser::setupSize()
{
    static const double MapPixWidth = 978.0;
    static const double MapPixHeight = 500.0;
    static const double MapPictureWidth = 978.0;
    static const double MapPictureHeight = 500.0;

    QFont font = m_title->font();
    font.setPointSizeF(getFontSize());
    m_title->setFont(font);
    //获取标题部分根据字体大小计算得到的字体高度
    double fontHeight =  m_title->fontMetrics().height();

    QSize ScreenSize = getScreenSize();
    //先根据屏幕大小，计算出大小合适的地图尺寸
    const double offsetW = 40 * 2.0;
    // 标题栏高度 + 标题高度 + 搜索框和确认按钮高度 + 四个间隔高度
    const double offsetH =50 + fontHeight + 36 * 2.0 + 20 * 4.0;

    //比对地图和屏幕大小，取其中最小的大小
    const float mapWidth = qMin(MapPixWidth, ScreenSize.width() - offsetW);
    const float mapHeight = qMin(MapPixHeight, ScreenSize.height() - offsetH);

    //再计算地图和界面宽高比，取其中最小最大缩放比
    const double widthScale = MapPictureWidth / mapWidth;
    const double heightScale = MapPictureHeight / mapHeight;
    const double scale = qMax(widthScale, heightScale);
    //根据宽高最大缩放比设置地图大小
    m_map->setFixedSize(MapPictureWidth / scale, MapPictureHeight / scale);

    //再根据地图大小设置界面大小，使界面与地图保持合适位置
    setFixedSize(m_map->size().width() + offsetW, m_map->size().height() + offsetH);
}
