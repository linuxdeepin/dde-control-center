// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "personalizationthememodule.h"

#include "dccslider.h"
#include "model/fontmodel.h"
#include "model/fontsizemodel.h"
#include "model/thememodel.h"
#include "personalizationmodel.h"
#include "personalizationworker.h"
#include "settingsgroupmodule.h"
#include "titledslideritem.h"
#include "widgets/globalthemelistview.h"
#include "widgets/itemmodule.h"
#include "widgets/personalizationthemelist.h"
#include "widgets/ringcolorwidget.h"
#include <qpair.h>

#include <DGuiApplicationHelper>
#include <DLabel>
#include <DStyle>
#include <DIconTheme>

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDesktopServices>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QScroller>
#include <QToolButton>

using namespace DCC_NAMESPACE;
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

// clang-format off
#define CUSTOM_ACTIVE_COLOR "CUSTOM"
const QList<QString> ACTIVE_COLORS = {
    "#DF4187",
    "#FF5D00",
    "#F8CB00",
    "#23C400",
    "#00A48A",
    "#1F6EE7",
    "#3C02FF",
    "#8C00D4",
    "#4D4D4D",
    CUSTOM_ACTIVE_COLOR
};

const QList<QColor> ACTIVE_COLORST = {
    { 223, 65, 135 },
    { 234, 105, 31 },
    { 243, 181, 23 },
    { 73, 177, 37 },
    { 0, 164, 138 },
    {31, 110,  231},
    { 64, 47, 219 },
    { 119, 36, 177 },
    { 117, 117, 117 },
    QColor()
};

const QList<QColor> Dark_ACTIVE_COLORST = {
    { 168, 43, 98 },
    { 204, 77, 3 },
    { 208, 156, 0 },
    { 69, 159, 41 },
    { 24, 136, 118 },
    { 2, 76, 202 },
    { 68, 59, 186 },
    { 106, 36, 135 },
    { 134, 134, 134 },
    QColor()
};

// clang-format on

PersonalizationThemeModule::PersonalizationThemeModule(PersonalizationModel *model,
                                                       PersonalizationWorker *work,
                                                       QObject *parent)
    : PageModule("theme", tr("Theme"), parent)
    , m_standardModel(new QStandardItemModel(this))
    , m_monospacedModel(new QStandardItemModel(this))
    , m_model(model)
    , m_work(work)
{
    appendChild(new ItemModule("themeTitle",
                               tr("Theme"),
                               this,
                               &PersonalizationThemeModule::initThemeTitle,
                               false));
    SettingsGroupModule *group = new SettingsGroupModule("theme", tr("Theme"));
    appendChild(group);
    group->appendChild(new ItemModule("themeList",
                                      tr("Theme"),
                                      this,
                                      &PersonalizationThemeModule::initThemeList,
                                      false));
    group->appendChild(new ItemModule("themeMode",
                                      tr("Appearance"),
                                      this,
                                      &PersonalizationThemeModule::initThemeSwitch));

    appendChild(new ItemModule("accentColorTitle", tr("Accent Color")));
    appendChild(new ItemModule("accentColor",
                               tr("Accent Color"),
                               this,
                               &PersonalizationThemeModule::initAccentColor,
                               false));
    appendChild(new ItemModule("iconSettings", tr("Icon Settings")));
    ItemModule *tmpModule = new ItemModule("iconTheme",
                                           tr("Icon Theme"),
                                           this,
                                           &PersonalizationThemeModule::initIconTheme);
    tmpModule->setBackground(true);
    tmpModule->setClickable(true);
    connect(tmpModule, &ItemModule::clicked, this, &PersonalizationThemeModule::setIconTheme);
    appendChild(tmpModule);
    tmpModule = new ItemModule("cursorTheme",
                               tr("Cursor Theme"),
                               this,
                               &PersonalizationThemeModule::initCursorTheme);
    tmpModule->setBackground(true);
    tmpModule->setClickable(true);
    connect(tmpModule, &ItemModule::clicked, this, &PersonalizationThemeModule::setCursorTheme);
    appendChild(tmpModule);
    appendChild(new ItemModule("textSettings", tr("Text Settings")));
    tmpModule = new ItemModule("fontSize",
                               tr("Font Size"),
                               this,
                               &PersonalizationThemeModule::initFontSize,
                               false);
    appendChild(tmpModule);
    tmpModule = new ItemModule("standardFont",
                               tr("Standard Font"),
                               this,
                               &PersonalizationThemeModule::initStandardFont);
    tmpModule->setBackground(true);
    appendChild(tmpModule);
    tmpModule = new ItemModule("monospacedFont",
                               tr("Monospaced Font"),
                               this,
                               &PersonalizationThemeModule::initMonospacedFont);
    tmpModule->setBackground(true);
    appendChild(tmpModule);

    setStandList(m_model->getStandFontModel()->getFontList());
    connect(m_model->getStandFontModel(),
            &FontModel::listChanged,
            this,
            &PersonalizationThemeModule::setStandList);

    setMonoList(m_model->getMonoFontModel()->getFontList());
    connect(m_model->getMonoFontModel(),
            &FontModel::listChanged,
            this,
            &PersonalizationThemeModule::setMonoList);
}

void PersonalizationThemeModule::active()
{
    m_work->refreshFont();
}

void PersonalizationThemeModule::onActiveColorClicked()
{
    RoundColorWidget *pItem = dynamic_cast<RoundColorWidget *>(sender());
    // 设置active color
    QString strColor = pItem->accessibleName();
    if (strColor == CUSTOM_ACTIVE_COLOR) {
        QColorDialog *colorDialog = new QColorDialog(pItem->palette().highlight().color(), pItem);
        colorDialog->deleteLater();
        if (QDialog::Accepted == colorDialog->exec()) {
            m_work->setActiveColor(colorDialog->selectedColor().name());
        }
    } else {
        QString result = DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType ?
            pItem->activeColors().first : pItem->activeColors().second;
        m_work->setActiveColors(pItem->activeColors().first + "," + pItem->activeColors().second);
        pItem->setColor(result);
        m_work->setActiveColor(result);
    }
}

void PersonalizationThemeModule::setStandList(const QList<QJsonObject> &list)
{
    setList(list, m_standardModel);
    m_model->getStandFontModel()->defaultFontChanged(m_model->getStandFontModel()->getFontName());
}

void PersonalizationThemeModule::setMonoList(const QList<QJsonObject> &list)
{
    setList(list, m_monospacedModel);
    m_model->getMonoFontModel()->defaultFontChanged(m_model->getMonoFontModel()->getFontName());
}

void PersonalizationThemeModule::setList(const QList<QJsonObject> &list, QStandardItemModel *model)
{
    model->blockSignals(true);
    model->clear();
    for (QJsonObject item : list) {
        QString name = item["Name"].toString();
        QStandardItem *modelItem = new QStandardItem(name);
        modelItem->setFont(QFont(name));
        model->appendRow(modelItem);
    }
    model->blockSignals(false);
}

void PersonalizationThemeModule::setIconTheme(QWidget *widget)
{
    PersonalizationThemeList *themeList = new PersonalizationThemeList(tr("Icon Theme"), widget);
    themeList->setModel(m_model->getIconModel());
    connect(themeList,
            &PersonalizationThemeList::requestSetDefault,
            m_work,
            &PersonalizationWorker::setDefault);
    themeList->exec();
}

void PersonalizationThemeModule::setCursorTheme(QWidget *widget)
{
    PersonalizationThemeList *themeList = new PersonalizationThemeList(tr("Cursor Theme"), widget);
    themeList->setModel(m_model->getMouseModel());
    connect(themeList,
            &PersonalizationThemeList::requestSetDefault,
            m_work,
            &PersonalizationWorker::setDefault);
    themeList->exec();
}

QWidget *PersonalizationThemeModule::initThemeTitle(ModuleObject *module)
{
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);

    DLabel *leftWidget = new DLabel(module->displayName());
    leftWidget->setAccessibleName(module->name());
    leftWidget->setForegroundRole(DPalette::TextTitle);
    DFontSizeManager::instance()->bind(leftWidget, DFontSizeManager::T5, QFont::DemiBold);
    layout->addWidget(leftWidget);

    //    QToolButton *button = new QToolButton();
    //    button->setIcon(DIconTheme::findQIcon("help"));
    //    button->setFixedSize(24, 24);
    //    layout->addWidget(button);
    //    layout->addStretch();
    //    connect(button, &QToolButton::clicked, button, []() {
    //        QDesktopServices::openUrl(QUrl("file:///usr/share/dde-control-center/developdocument.html"));
    //    });
    return widget;
}

QWidget *PersonalizationThemeModule::initThemeList(ModuleObject *module)
{
    Q_UNUSED(module)
    GlobalThemeListView *view = new GlobalThemeListView();
    view->setThemeModel(m_model->getGlobalThemeModel());
    connect(view, &GlobalThemeListView::applied, this, [this](const QModelIndex &index) {
        qDebug() << "applied global theme" << index.data(GlobalThemeModel::IdRole).toString();
        QString id = index.data(GlobalThemeModel::IdRole).toString();
        ThemeModel *globalTheme = m_model->getGlobalThemeModel();
        QString mode;
        QString themeId = getGlobalThemeId(globalTheme->getDefault(), mode);

        const QMap<QString, QJsonObject> &itemList = globalTheme->getList();
        if (itemList.contains(id))
            m_work->setDefaultByType(itemList.value(id)["type"].toString(), id + mode);
    });
    return view;
}

QWidget *PersonalizationThemeModule::initThemeSwitch(ModuleObject *module)
{
    QComboBox *box = new QComboBox();
    ThemeModel *globalTheme = m_model->getGlobalThemeModel();

    auto updateDefault = [box, module, this]() {
        ThemeModel *globalTheme = m_model->getGlobalThemeModel();
        QString mode;
        QString themeId = getGlobalThemeId(globalTheme->getDefault(), mode);
        box->clear();
        box->addItem(tr("Light"), ".light");
        const QJsonObject &json = globalTheme->getList().value(themeId);
        if (json.isEmpty())
            return;
        if (json["hasDark"].toBool()) {
            box->addItem(tr("Auto"), "");
            box->addItem(tr("Dark"), ".dark");
            module->setDisabled(false);
        } else {
            module->setDisabled(true);
        }
        for (int i = 0; i < box->count(); i++) {
            if (box->itemData(i).toString() == mode) {
                box->blockSignals(true);
                box->setCurrentIndex(i);
                box->blockSignals(false);
                break;
            }
        }
    };
    updateDefault();

    connect(globalTheme, &ThemeModel::defaultChanged, box, updateDefault);
    connect(globalTheme, &ThemeModel::itemAdded, box, updateDefault);
    connect(globalTheme, &ThemeModel::itemRemoved, box, updateDefault);

    connect(box, QOverload<int>::of(&QComboBox::activated), box, [this, box](int index) {
        QString dataMode = box->itemData(index).toString();
        ThemeModel *globalTheme = m_model->getGlobalThemeModel();
        QString mode;
        QString themeId = getGlobalThemeId(globalTheme->getDefault(), mode);
        const QMap<QString, QJsonObject> &itemList = globalTheme->getList();
        if (itemList.contains(themeId)) {
            m_work->setDefaultByType(itemList.value(themeId)["type"].toString(),
                                     themeId + dataMode);
        }

    });
    return box;
}

QWidget *PersonalizationThemeModule::initAccentColor(ModuleObject *module)
{
    Q_UNUSED(module)
    RingColorWidget *bgWidget = new RingColorWidget();
    bgWidget->setFixedHeight(40);
    QHBoxLayout *colorLayout = new QHBoxLayout(bgWidget);
    colorLayout->setAlignment(Qt::AlignLeft);
    colorLayout->setContentsMargins(10, 0, 10, 0);
    colorLayout->addStretch();
    int borderWidth = bgWidget->style()->pixelMetric(
            static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth),
            nullptr,
            bgWidget);
    int borderSpacing = bgWidget->style()->pixelMetric(
            static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing),
            nullptr,
            bgWidget);
    int totalSpace = borderWidth + borderSpacing
            + RoundColorWidget::EXTRA; // 2px extra space to avoid line cutted off

    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    const QList<QColor> &activeColors =
            (themeType == DGuiApplicationHelper::ColorType::LightType ? ACTIVE_COLORST
                                                                      : Dark_ACTIVE_COLORST);
    for (int i = 0; i < activeColors.size(); ++i) {
        QColor color = activeColors.at(i);
        RoundColorWidget *colorItem = new RoundColorWidget(color, bgWidget);
        colorItem->setActiveColors(QPair<QString, QString>(ACTIVE_COLORST[i].name(), Dark_ACTIVE_COLORST[i].name()));
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
        effect->setBlurRadius(17); // 阴影圆角的大小

        color.setAlpha(68);
        effect->setColor(color); // 阴影的颜色
        effect->setOffset(0, 5);
        colorItem->setGraphicsEffect(effect);

        colorItem->setAccessibleName(ACTIVE_COLORS[i]);

        DPalette pa = colorItem->palette();
        pa.setBrush(DPalette::Base, color);
        colorItem->setPalette(pa);
        colorItem->setFixedSize(20 + 2 * totalSpace, 40);
        colorLayout->addWidget(colorItem);
        connect(colorItem,
                &RoundColorWidget::clicked,
                this,
                &PersonalizationThemeModule::onActiveColorClicked);
    }
    colorLayout->addStretch();
    auto setColorFun = [bgWidget](const QString &newColor) {
        QLayout *lyt = bgWidget->layout();
        int endIndex = lyt->count() - 2;
        for (int i = 1; i <= endIndex; ++i) {
            RoundColorWidget *w = qobject_cast<RoundColorWidget *>(lyt->itemAt(i)->widget());
            if (!w) {
                break;
            }

            if (w->activeColors().first.compare(newColor, Qt::CaseInsensitive) == 0 ||
                w->activeColors().second.compare(newColor, Qt::CaseInsensitive) == 0) {
                bgWidget->setSelectedItem(w);
                break;
            } else if (i == endIndex) {
                bgWidget->setSelectedItem(w);
            }
        }
        bgWidget->update();
    };
    setColorFun(m_model->getActiveColor());
    connect(m_model, &PersonalizationModel::onActiveColorChanged, bgWidget, setColorFun);

    return bgWidget;
}

QWidget *PersonalizationThemeModule::initIconTheme(ModuleObject *module)
{
    Q_UNUSED(module)
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addStretch();

    QLabel *pic = new QLabel(widget);
    auto setPic = [pic, this]() {
        ThemeModel *model = m_model->getIconModel();
        QString picPath = model->getPicList().value(model->getDefault());
        QPixmap pixmap(picPath);
        pixmap.setDevicePixelRatio(pic->devicePixelRatioF());
        pic->setPixmap(pixmap);
    };
    setPic();
    connect(m_model->getIconModel(), &ThemeModel::defaultChanged, pic, setPic);
    connect(m_model->getIconModel(), &ThemeModel::picAdded, pic, setPic);
    layout->addWidget(pic);
    QLabel *enterIcon = new QLabel(widget);
    enterIcon->setPixmap(
            DStyle::standardIcon(widget->style(), DStyle::SP_ArrowEnter).pixmap(16, 16));
    layout->addWidget(enterIcon);
    return widget;
}

QWidget *PersonalizationThemeModule::initCursorTheme(ModuleObject *module)
{
    Q_UNUSED(module)
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addStretch();

    QLabel *pic = new QLabel(widget);
    auto setPic = [pic, this]() {
        ThemeModel *model = m_model->getMouseModel();
        QString picPath = model->getPicList().value(model->getDefault());
        QPixmap pixmap(picPath);
        pixmap.setDevicePixelRatio(pic->devicePixelRatioF());
        pic->setPixmap(pixmap);
    };
    setPic();
    connect(m_model->getMouseModel(), &ThemeModel::defaultChanged, pic, setPic);
    connect(m_model->getMouseModel(), &ThemeModel::picAdded, pic, setPic);
    layout->addWidget(pic);
    QLabel *enterIcon = new QLabel(widget);
    enterIcon->setPixmap(
            DStyle::standardIcon(widget->style(), DStyle::SP_ArrowEnter).pixmap(16, 16));
    layout->addWidget(enterIcon);
    return widget;
}

QWidget *PersonalizationThemeModule::initFontSize(ModuleObject *module)
{
    Q_UNUSED(module)
    TitledSliderItem *fontSizeSlider = new TitledSliderItem();
    fontSizeSlider->addBackground();
    fontSizeSlider->setObjectName("fontsizeslider");
    QStringList annotions;
    annotions << "11"
              << "12"
              << "13"
              << "14"
              << "15"
              << "16"
              << "18"
              << "20";
    fontSizeSlider->setAnnotations(annotions);

    fontSizeSlider->setIconSize(QSize(16, 16));

    fontSizeSlider->setLeftIcon(DIconTheme::findQIcon("fontsize_decrease"));
    fontSizeSlider->setRightIcon(DIconTheme::findQIcon("fontsize_increase"));

    DCCSlider *slider = fontSizeSlider->slider();
    slider->setOrientation(Qt::Horizontal);
    slider->setRange(0, annotions.size() - 1);
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);
    slider->setPageStep(1);
    auto fontSizeChanged = [fontSizeSlider, annotions](int fontSize) {
        fontSizeSlider->slider()->blockSignals(true);
        fontSizeSlider->slider()->setValue(fontSize);
        fontSizeSlider->slider()->blockSignals(false);
        fontSizeSlider->setValueLiteral(annotions[fontSize]);
    };
    fontSizeChanged(m_model->getFontSizeModel()->getFontSize());
    connect(m_model->getFontSizeModel(),
            &FontSizeModel::sizeChanged,
            fontSizeSlider,
            fontSizeChanged);
    connect(slider, &DCCSlider::valueChanged, m_work, &PersonalizationWorker::setFontSize);
    connect(slider, &DCCSlider::sliderMoved, m_work, &PersonalizationWorker::setFontSize);
    return fontSizeSlider;
}

QWidget *PersonalizationThemeModule::initStandardFont(ModuleObject *module)
{
    Q_UNUSED(module)
    QComboBox *comboBox = new QComboBox();
    comboBox->setModel(m_standardModel);
    initFontWidget(comboBox, m_model->getStandFontModel(), m_standardModel);
    return comboBox;
}

QWidget *PersonalizationThemeModule::initMonospacedFont(ModuleObject *module)
{
    Q_UNUSED(module)
    QComboBox *comboBox = new QComboBox();
    comboBox->setModel(m_monospacedModel);
    initFontWidget(comboBox, m_model->getMonoFontModel(), m_monospacedModel);
    return comboBox;
}

void PersonalizationThemeModule::initFontWidget(QComboBox *combox,
                                                FontModel *fontModel,
                                                QStandardItemModel *model)
{
    combox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto defaultFont = [combox, fontModel, model](const QString &name) {
        for (const QJsonObject &obj : fontModel->getFontList()) {
            if (obj["Id"].toString() == name) {
                combox->blockSignals(true);
                combox->setCurrentText(obj["Name"].toString());
                combox->blockSignals(false);
                return;
            }
        }
        bool hasItem = false;
        for (int i = 0; i < model->rowCount(); ++i) {
            if (model->item(i)->text() == name) {
                hasItem = true;
                break;
            }
        }
        if (!hasItem) {
            QStandardItem *modelItem = new QStandardItem(name);
            modelItem->setFont(QFont(name));
            model->appendRow(modelItem);
        }

        combox->blockSignals(true);
        combox->setCurrentText(name);
        combox->blockSignals(false);
    };
    defaultFont(fontModel->getFontName());
    connect(fontModel, &FontModel::defaultFontChanged, combox, defaultFont);
    connect(combox, &QComboBox::currentTextChanged, this, [fontModel, this](const QString &name) {
        for (const QJsonObject &obj : fontModel->getFontList()) {
            if (obj["Name"].toString() == name) {
                m_work->setDefault(obj);
                return;
            }
        }
    });
}

QString PersonalizationThemeModule::getGlobalThemeId(const QString &themeId, QString &mode)
{
    QString id = themeId;
    mode.clear();
    if (id.endsWith(".light")) {
        id.chop(6);
        mode = ".light";
    } else if (id.endsWith(".dark")) {
        id.chop(5);
        mode = ".dark";
    }
    return id;
}
