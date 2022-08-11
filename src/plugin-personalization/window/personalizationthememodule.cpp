/*
 * Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
 *
 * Author:     caixiangrong <caixiangrong@uniontech.com>
 *
 * Maintainer: caixiangrong <caixiangrong@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "personalizationthememodule.h"
#include "personalizationmodel.h"
#include "personalizationworker.h"
#include "widgets/ringcolorwidget.h"
#include "model/fontsizemodel.h"
#include "model/fontmodel.h"
#include "widgets/themeitempic.h"
#include "model/thememodel.h"
#include "widgets/personalizationthemelist.h"
#include "widgets/globalthemelistview.h"

#include "widgets/widgetmodule.h"
#include "widgets/dcclistview.h"
#include "widgets/itemmodule.h"
#include "titledslideritem.h"
#include "dccslider.h"
#include "settingsgroupmodule.h"

#include <DGuiApplicationHelper>
#include <DStyle>
#include <QCheckBox>
#include <QComboBox>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QScroller>
#include <QToolButton>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

const QList<QString> ACTIVE_COLORS = {
    "#D8316C",
    "#FF5D00",
    "#F8CB00",
    "#23C400",
    "#00A48A",
    "#0081FF",
    "#3C02FF",
    "#8C00D4",
    "#4D4D4D"
};

struct ColorStru
{
    int r = 0;
    int g = 0;
    int b = 0;
    bool operator==(const ColorStru b) const
    {
        return (b.r == this->r && b.g == this->g && b.b == this->b);
    }
};

const QList<ColorStru> ACTIVE_COLORST = {
    { 216, 49, 108 },
    { 255, 93, 0 },
    { 248, 203, 0 },
    { 35, 196, 0 },
    { 0, 164, 138 },
    { 0, 129, 255 },
    { 60, 2, 215 },
    { 140, 0, 212 },
    { 77, 77, 77 },
};

const QList<ColorStru> Dark_ACTIVE_COLORST = {
    { 179, 33, 87 },
    { 204, 71, 0 },
    { 198, 162, 0 },
    { 28, 156, 0 },
    { 0, 130, 108 },
    { 0, 105, 204 },
    { 44, 0, 206 },
    { 109, 0, 168 },
    { 61, 61, 61 },
};

PersonalizationThemeModule::PersonalizationThemeModule(PersonalizationModel *model, PersonalizationWorker *work, QObject *parent)
    : PageModule("theme", tr("Theme"), parent)
    , m_standardModel(new QStandardItemModel(this))
    , m_monospacedModel(new QStandardItemModel(this))
    , m_model(model)
    , m_work(work)
{
    appendChild(new ItemModule("themeTitle", tr("Theme")));
    SettingsGroupModule *group = new SettingsGroupModule("theme", tr("Theme"));
    appendChild(group);
    group->appendChild(new ItemModule("themeList", tr("Theme"), this, &PersonalizationThemeModule::initThemeList, false));
    group->appendChild(new ItemModule("themeMode", tr("Appearance mode"), this, &PersonalizationThemeModule::initThemeSwitch));

    appendChild(new ItemModule("accentColorTitle", tr("Accent Color")));
    appendChild(new ItemModule("accentColor", tr("Accent Color"), this, &PersonalizationThemeModule::initAccentColor, false));
    appendChild(new ItemModule("iconSettings", tr("Icon Settings")));
    ItemModule *tmpModule = new ItemModule("iconTheme", tr("Icon Theme"), this, &PersonalizationThemeModule::initIconTheme);
    tmpModule->setBackground(true);
    tmpModule->setClickable(true);
    connect(tmpModule, &ItemModule::clicked, this, &PersonalizationThemeModule::setIconTheme);
    appendChild(tmpModule);
    tmpModule = new ItemModule("cursorTheme", tr("Cursor Theme"), this, &PersonalizationThemeModule::initCursorTheme);
    tmpModule->setBackground(true);
    tmpModule->setClickable(true);
    connect(tmpModule, &ItemModule::clicked, this, &PersonalizationThemeModule::setCursorTheme);
    appendChild(tmpModule);
    appendChild(new ItemModule("textSettings", tr("Text Settings")));
    tmpModule = new ItemModule("fontSize", tr("Font Size"), this, &PersonalizationThemeModule::initFontSize, false);
    appendChild(tmpModule);
    tmpModule = new ItemModule("standardFont", tr("Standard Font"), this, &PersonalizationThemeModule::initStandardFont);
    tmpModule->setBackground(true);
    appendChild(tmpModule);
    tmpModule = new ItemModule("monospacedFont", tr("Monospaced Font"), this, &PersonalizationThemeModule::initMonospacedFont);
    tmpModule->setBackground(true);
    appendChild(tmpModule);

    setStandList(m_model->getStandFontModel()->getFontList());
    connect(m_model->getStandFontModel(), &FontModel::listChanged, this, &PersonalizationThemeModule::setStandList);

    setMonoList(m_model->getMonoFontModel()->getFontList());
    connect(m_model->getMonoFontModel(), &FontModel::listChanged, this, &PersonalizationThemeModule::setMonoList);
}

void PersonalizationThemeModule::active()
{
    m_work->refreshFont();
}

void PersonalizationThemeModule::onActiveColorClicked()
{
    RoundColorWidget *pItem = dynamic_cast<RoundColorWidget *>(sender());
    //设置active color
    QString strColor = pItem->accessibleName();
    m_work->setActiveColor(strColor);
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
    connect(themeList, &PersonalizationThemeList::requestSetDefault, m_work, &PersonalizationWorker::setDefault);
    themeList->exec();
}

void PersonalizationThemeModule::setCursorTheme(QWidget *widget)
{
    PersonalizationThemeList *themeList = new PersonalizationThemeList(tr("Cursor Theme"), widget);
    themeList->setModel(m_model->getMouseModel());
    connect(themeList, &PersonalizationThemeList::requestSetDefault, m_work, &PersonalizationWorker::setDefault);
    themeList->exec();
}

QWidget *PersonalizationThemeModule::initThemeList(ModuleObject *module)
{
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

        m_work->setDefault(m_model->getGlobalThemeModel()->getList().value(index.data(GlobalThemeModel::IdRole).toString()));
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
        if (itemList.contains(themeId))
            m_work->setDefaultByType(itemList.value(themeId)["type"].toString(), themeId + dataMode);
    });
    return box;
}

QWidget *PersonalizationThemeModule::initAccentColor(ModuleObject *module)
{
    RingColorWidget *bgWidget = new RingColorWidget();
    bgWidget->setFixedHeight(40);
    QHBoxLayout *colorLayout = new QHBoxLayout(bgWidget);
    colorLayout->setAlignment(Qt::AlignLeft);
    colorLayout->setContentsMargins(10, 0, 10, 0);
    colorLayout->addStretch();
    int borderWidth = bgWidget->style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, bgWidget);
    int borderSpacing = bgWidget->style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, bgWidget);
    int totalSpace = borderWidth + borderSpacing + RoundColorWidget::EXTRA; // 2px extra space to avoid line cutted off

    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    for (ColorStru aColor : (themeType == DGuiApplicationHelper::ColorType::LightType ? ACTIVE_COLORST : Dark_ACTIVE_COLORST)) {
        QColor color;
        color.setRgb(aColor.r, aColor.g, aColor.b);
        RoundColorWidget *colorItem = new RoundColorWidget(color, bgWidget);
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
        effect->setBlurRadius(17); // 阴影圆角的大小

        color.setAlpha(68);
        effect->setColor(color); //阴影的颜色
        effect->setOffset(0, 5);
        colorItem->setGraphicsEffect(effect);

        if (themeType == DGuiApplicationHelper::ColorType::LightType) {
            colorItem->setAccessibleName(ACTIVE_COLORS[ACTIVE_COLORST.indexOf(aColor)]);
        } else {
            colorItem->setAccessibleName(ACTIVE_COLORS[Dark_ACTIVE_COLORST.indexOf(aColor)]);
        }

        DPalette pa = colorItem->palette();
        pa.setBrush(DPalette::Base, color);
        colorItem->setPalette(pa);
        colorItem->setFixedSize(20 + 2 * totalSpace, 40);
        colorLayout->addWidget(colorItem);
        connect(colorItem, &RoundColorWidget::clicked, this, &PersonalizationThemeModule::onActiveColorClicked);
    }
    colorLayout->addStretch();
    auto setColorFun = [bgWidget](const QString &newColor) {
        QLayout *lyt = bgWidget->layout();
        for (int i = lyt->count() - 2; i > 0; --i) {
            if (lyt->itemAt(i)->widget()->accessibleName() == newColor) {
                bgWidget->setSelectedItem(qobject_cast<RoundColorWidget *>(lyt->itemAt(i)->widget()));
            }
        }
    };
    setColorFun(m_model->getActiveColor());
    connect(m_model, &PersonalizationModel::onActiveColorChanged, bgWidget, setColorFun);

    return bgWidget;
}

QWidget *PersonalizationThemeModule::initIconTheme(ModuleObject *module)
{
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addStretch();

    QLabel *pic = new QLabel(widget);
    auto setPic = [pic, this]() {
        ThemeModel *model = m_model->getIconModel();
        QString picPath = model->getPicList().value(model->getDefault());
        pic->setPixmap(QPixmap(picPath));
    };
    setPic();
    connect(m_model->getIconModel(), &ThemeModel::defaultChanged, pic, setPic);
    connect(m_model->getIconModel(), &ThemeModel::picAdded, pic, setPic);
    layout->addWidget(pic);
    QLabel *enterIcon = new QLabel(widget);
    enterIcon->setPixmap(DStyle::standardIcon(widget->style(), DStyle::SP_ArrowEnter).pixmap(16, 16));
    layout->addWidget(enterIcon);
    return widget;
}

QWidget *PersonalizationThemeModule::initCursorTheme(ModuleObject *module)
{
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addStretch();

    QLabel *pic = new QLabel(widget);
    auto setPic = [pic, this]() {
        ThemeModel *model = m_model->getMouseModel();
        QString picPath = model->getPicList().value(model->getDefault());
        pic->setPixmap(QPixmap(picPath));
    };
    setPic();
    connect(m_model->getMouseModel(), &ThemeModel::defaultChanged, pic, setPic);
    connect(m_model->getMouseModel(), &ThemeModel::picAdded, pic, setPic);
    layout->addWidget(pic);
    QLabel *enterIcon = new QLabel(widget);
    enterIcon->setPixmap(DStyle::standardIcon(widget->style(), DStyle::SP_ArrowEnter).pixmap(16, 16));
    layout->addWidget(enterIcon);
    return widget;
}

QWidget *PersonalizationThemeModule::initFontSize(ModuleObject *module)
{
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
    fontSizeSlider->setIconSize(QSize(24, 24));
    fontSizeSlider->setLeftIcon(QIcon::fromTheme("fontsize_decrease"));
    fontSizeSlider->setRightIcon(QIcon::fromTheme("fontsize_increase"));

    DCCSlider *slider = fontSizeSlider->slider();
    slider->setOrientation(Qt::Horizontal);
    slider->setRange(0, annotions.size() - 1);
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);
    slider->setPageStep(1);
    auto fontSizeChanged = [fontSizeSlider](int fontSize) {
        fontSizeSlider->slider()->blockSignals(true);
        fontSizeSlider->slider()->setValue(fontSize);
        fontSizeSlider->slider()->blockSignals(false);
    };
    fontSizeChanged(m_model->getFontSizeModel()->getFontSize());
    connect(m_model->getFontSizeModel(), &FontSizeModel::sizeChanged, fontSizeSlider, fontSizeChanged);
    connect(slider, &DCCSlider::valueChanged, m_work, &PersonalizationWorker::setFontSize);
    connect(slider, &DCCSlider::sliderMoved, m_work, &PersonalizationWorker::setFontSize);
    return fontSizeSlider;
}

QWidget *PersonalizationThemeModule::initStandardFont(ModuleObject *module)
{
    QComboBox *comboBox = new QComboBox();
    comboBox->setModel(m_standardModel);
    initFontWidget(comboBox, m_model->getStandFontModel());
    return comboBox;
}

QWidget *PersonalizationThemeModule::initMonospacedFont(ModuleObject *module)
{
    QComboBox *comboBox = new QComboBox();
    comboBox->setModel(m_monospacedModel);
    initFontWidget(comboBox, m_model->getMonoFontModel());
    return comboBox;
}

void PersonalizationThemeModule::initFontWidget(QComboBox *combox, FontModel *fontModel)
{
    combox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto defaultFont = [combox, fontModel](const QString &name) {
        for (const QJsonObject &obj : fontModel->getFontList()) {
            if (obj["Id"].toString() == name) {
                combox->blockSignals(true);
                combox->setCurrentText(obj["Name"].toString());
                combox->blockSignals(false);
                return;
            }
        }
        combox->blockSignals(true);
        combox->setCurrentText(fontModel->getFontName() + tr(" (Unsupported font)"));
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
