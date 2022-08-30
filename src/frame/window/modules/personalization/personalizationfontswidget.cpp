// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "personalizationfontswidget.h"

#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/settingsitem.h"
#include "modules/personalization/personalizationmodel.h"
#include "modules/personalization/model/fontsizemodel.h"
#include "modules/personalization/model/fontmodel.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include <DSlider>
#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QStandardItemModel>
#include <QDebug>
#include <QTimer>
#include <QAbstractItemView>
#include <QScrollBar>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;
using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE

PersonalizationFontsWidget::PersonalizationFontsWidget(QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout())
    , m_standardFontsCbBox(new QComboBox(this))
    , m_monoFontsCbBox(new QComboBox(this))
    , m_isAppend(false)
{
    setAccessibleName("PersonalizationFontsWidget");
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(10);
    m_centralLayout->setContentsMargins(ThirdPageContentsMargins);
    //font size
    //~ contents_path /personalization/Font
    //~ child_page Font
    m_fontSizeSlider = new TitledSliderItem(tr("Size"));
    m_fontSizeSlider->addBackground();
    m_fontSizeSlider->setObjectName("fontsizeslider");
    m_fontSizeSlider->slider()->setOrientation(Qt::Horizontal);

    DCCSlider *slider = m_fontSizeSlider->slider();
    QStringList annotions;
    annotions << "11" << "12" << "13" << "14" << "15" << "16" << "18" << "20";
    m_fontSizeSlider->setAnnotations(annotions);
    slider->setRange(0, annotions.size() - 1);
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);
    slider->setPageStep(1);

    m_centralLayout->addWidget(m_fontSizeSlider);

    //standard font
    QHBoxLayout *sfontLayout = new QHBoxLayout();
    sfontLayout->setContentsMargins(10, 6, 10, 6);
    m_sfontitem = new SettingsItem;
    m_sfontitem->addBackground();
    m_sfontitem->setLayout(sfontLayout);
    //~ contents_path /personalization/Font
    //~ child_page Font
    QString sf = tr("Standard Font");
    QLabel *sfLabel = new QLabel(sf);
    sfLabel->setWordWrap(true);
    sfLabel->setFixedWidth(140);
    sfontLayout->addWidget(sfLabel);
    sfontLayout->addWidget(m_standardFontsCbBox);

    m_standardFontsCbBox->setModel(new QStandardItemModel(this));
    m_standardFontsCbBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_centralLayout->addWidget(m_sfontitem);
    GSettingWatcher::instance()->bind("perssonalFontStandard", m_sfontitem);


    //mono font
    QHBoxLayout *mfontLayout = new QHBoxLayout();
    mfontLayout->setContentsMargins(10, 6, 10, 6);
    m_mfontitem = new SettingsItem;
    m_mfontitem->addBackground();
    m_mfontitem->setLayout(mfontLayout);
    //~ contents_path /personalization/Font
    //~ child_page Font
    QString mf = tr("Monospaced Font");
    QLabel *mfLabel = new QLabel(mf);
    mfLabel->setWordWrap(true);
    mfLabel->setFixedWidth(140);
    mfontLayout->addWidget(mfLabel);
    mfontLayout->addWidget(m_monoFontsCbBox);
    m_monoFontsCbBox->setModel(new QStandardItemModel(this));
    m_centralLayout->addWidget(m_mfontitem);
    m_centralLayout->addStretch();
    setLayout(m_centralLayout);
    GSettingWatcher::instance()->bind("perssonalFontMono", m_mfontitem);

    connect(slider, &DCCSlider::valueChanged, this, &PersonalizationFontsWidget::requestSetFontSize);
    connect(slider, &DCCSlider::sliderMoved, this, &PersonalizationFontsWidget::requestSetFontSize);
}

PersonalizationFontsWidget::~PersonalizationFontsWidget()
{
    GSettingWatcher::instance()->erase("perssonalFontStandard", m_sfontitem);
    GSettingWatcher::instance()->erase("perssonalFontMono", m_mfontitem);
    GSettingWatcher::instance()->erase("perssonalFontSize", m_fontSizeSlider);
}

void PersonalizationFontsWidget::setModel(dcc::personalization::PersonalizationModel *const model)
{
    m_model = model;

    //font size
    connect(m_model->getFontSizeModel(), &dcc::personalization::FontSizeModel::sizeChanged, this, &PersonalizationFontsWidget::setFontSize);
    setFontSize(m_model->getFontSizeModel()->getFontSize());
    GSettingWatcher::instance()->bind("perssonalFontSize", m_fontSizeSlider);

    //standard font & mono font
    dcc::personalization::FontModel *standmodel = model->getStandFontModel();
    dcc::personalization::FontModel *monomodel  = model->getMonoFontModel();
    //set fonts when they are already available
    if (standmodel->getFontList().size() > 0)
        setList(standmodel->getFontList(), standmodel);

    if (monomodel->getFontList().size() > 0)
        setList(monomodel->getFontList(), monomodel);

    connect(standmodel, &dcc::personalization::FontModel::defaultFontChanged, this,
            reinterpret_cast<void (PersonalizationFontsWidget::*)(const QString &)>(&PersonalizationFontsWidget::onDefaultFontChanged));
    connect(monomodel, &dcc::personalization::FontModel::defaultFontChanged, this,
            reinterpret_cast<void (PersonalizationFontsWidget::*)(const QString &)>(&PersonalizationFontsWidget::onDefaultFontChanged));

    connect(standmodel, &dcc::personalization::FontModel::listChanged, this,
            reinterpret_cast<void (PersonalizationFontsWidget::*)(const QList<QJsonObject> &)>(&PersonalizationFontsWidget::setList));
    connect(monomodel, &dcc::personalization::FontModel::listChanged, this,
            reinterpret_cast<void (PersonalizationFontsWidget::*)(const QList<QJsonObject> &)>(&PersonalizationFontsWidget::setList));

    connect(m_standardFontsCbBox, &QComboBox::currentTextChanged, this, &PersonalizationFontsWidget::onSelectChanged);
    connect(m_monoFontsCbBox, &QComboBox::currentTextChanged, this, &PersonalizationFontsWidget::onSelectChanged);

    connect(standmodel, &dcc::personalization::FontModel::listChanged, this, [ = ] {
        onSelectChanged(m_standardFontsCbBox->currentText());
    });
}

void PersonalizationFontsWidget::setFontSize(int size)
{
    m_fontSizeSlider->blockSignals(true);
    m_fontSizeSlider->slider()->setValue(size);
    m_fontSizeSlider->blockSignals(false);
    QTimer::singleShot(100, this, [&, size] {
        QList<int> FontSizeList {11, 12, 13, 14, 15, 16, 18, 20};
        if (size >= 0 && size < FontSizeList.count()) {
            setCommboxItemFontSize(FontSizeList[size]);
        } else {
            qWarning() << __FUNCTION__ << " out range of font size. ";
        }
    });
}

void PersonalizationFontsWidget::setList(const QList<QJsonObject> &list, dcc::personalization::FontModel *model)
{
    QStandardItemModel *fontModel{nullptr};
    if (sender())
        model = qobject_cast<dcc::personalization::FontModel *>(sender());

    QComboBox *combox{nullptr};
    combox = (model == m_model->getStandFontModel()) ? m_standardFontsCbBox : m_monoFontsCbBox;
    fontModel = dynamic_cast<QStandardItemModel *>(combox->model());

    //clear the model
    fontModel->clear();
    m_isAppend = true;
    for (QJsonObject item : list) {
        QString name = item["Name"].toString();
        QStandardItem *modelItem = new QStandardItem(name);
        modelItem->setFont(QFont(name));
        fontModel->appendRow(modelItem);
    }
    m_isAppend = false;

    onDefaultFontChanged(model->getFontName(), model);
}

void PersonalizationFontsWidget::setCommboxItemFontSize(int fontSize)
{
    auto setCommboxSize = [=](QComboBox *cb){
        auto model = qobject_cast<QStandardItemModel *>(cb->model());
        int maxLen = 0, itemWidth = 0;
        for (auto i = 0; i < model->rowCount(); ++i) {
            auto item = model->item(i);
            auto font = item->font();
            font.setPixelSize(fontSize);
            QFontMetrics fm(font);
            itemWidth = fm.width(item->text());
            maxLen = qMax(maxLen, itemWidth);
            item->setFont(font);
        }
        maxLen += cb->view()->verticalScrollBar()->depth() + 30;
        cb->view()->setMinimumWidth(maxLen);
    };

    setCommboxSize(m_standardFontsCbBox);
    setCommboxSize(m_monoFontsCbBox);
}

void PersonalizationFontsWidget::onSelectChanged(const QString &name)
{
    // combobox appends data, ignoring signal currentTextChanged
    if (m_isAppend)
        return;

    auto combox  = qobject_cast<QComboBox *>(sender());
    auto list = (combox == m_standardFontsCbBox) ? m_model->getStandFontModel()->getFontList() : m_model->getMonoFontModel()->getFontList();

    auto res = std::find_if(list.begin(), list.end(), [=] (const QJsonObject &data)->bool{
        return data["Name"].toString() == name;
    });

    if (res != list.cend()) {
        Q_EMIT requestSetDefault(*res);
        return;
    }

}

void PersonalizationFontsWidget::onDefaultFontChanged(const QString &name, dcc::personalization::FontModel *sender)
{
    if (this->sender())
        sender = qobject_cast<dcc::personalization::FontModel *>(this->sender());
    auto comboBox = (sender == m_model->getMonoFontModel()) ? m_monoFontsCbBox : m_standardFontsCbBox;

    for (const QJsonObject &obj : sender->getFontList()) {
        if (obj["Id"].toString() == name) {
            comboBox->setCurrentText(obj["Name"].toString());
            return;
        }
    }
    comboBox->setCurrentText(sender->getFontName() + tr(" (Unsupported font)"));
}
