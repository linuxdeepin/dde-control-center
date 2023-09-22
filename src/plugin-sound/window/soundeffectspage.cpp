// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "soundeffectspage.h"

#include "soundmodel.h"
#include "widgets/dcclistview.h"
#include "widgets/switchwidget.h"
#include "widgets/titlelabel.h"

#include <DIconButton>
#include <DListView>
#include <DIconTheme>

#include <QLabel>
#include <QListView>
#include <QScroller>
#include <QSound>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTimer>
#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

const int AnimationDuration = 5000;

SoundEffectsPage::SoundEffectsPage(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_effectList(new DCCListView(this))
    , m_sound(nullptr)
{
    m_layout->setContentsMargins(0, 0, 0, 10);

    TitleLabel *lblTitle = new TitleLabel(tr("Sound Effects"));
    lblTitle->setContentsMargins(0, 0, 0, 0);
    DFontSizeManager::instance()->bind(lblTitle, DFontSizeManager::T6);
    m_sw = new SwitchWidget(nullptr, lblTitle);
    m_sw->addBackground();
    m_sw->setFocusPolicy(Qt::ClickFocus);
    m_layout->addWidget(m_sw, 0, Qt::AlignTop);
    m_layout->setSpacing(10);

    m_effectList->setAccessibleName("List_effectlist");
    m_effectList->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_effectList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_effectList->setSelectionMode(QListView::SelectionMode::NoSelection);
    m_effectList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_effectList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_effectList->setEditTriggers(DListView::NoEditTriggers);
    m_effectList->setFrameShape(DListView::NoFrame);
    m_effectList->setViewportMargins(0, 0, 0, 0);
    m_effectList->setItemSpacing(1);

    QMargins itemMargins(m_effectList->itemMargins());
    itemMargins.setLeft(14);
    m_effectList->setItemMargins(itemMargins);

    m_layout->addWidget(m_effectList);

    m_aniTimer = new QTimer(this);
    m_aniTimer->setSingleShot(false);
    setLayout(m_layout);
}

SoundEffectsPage::~SoundEffectsPage()
{
    QScroller *scroller = QScroller::scroller(m_effectList->viewport());
    if (scroller) {
        scroller->stop();
    }
}

void SoundEffectsPage::setModel(SoundModel *model)
{
    m_model = model;
    connect(m_model, &SoundModel::enableSoundEffectChanged, this, [this](bool on) {
        m_sw->blockSignals(true);
        m_sw->setChecked(on);
        m_sw->blockSignals(false);
        m_effectList->setVisible(on);
    });

    connect(m_sw,
            &SwitchWidget::checkedChanged,
            this,
            &SoundEffectsPage::requestSwitchSoundEffects);
    m_effectList->setVisible(m_model->enableSoundEffect());
    initList();
}

void SoundEffectsPage::startPlay(const QModelIndex &index)
{
    if (m_playIdx.isValid()) {
        auto item = static_cast<DStandardItem *>(m_listModel->itemFromIndex(m_playIdx));
        auto aniAction = item->actionList(Qt::Edge::RightEdge)[0];
        aniAction->setVisible(false);
        m_effectList->update(m_playIdx);
    }
    m_playIdx = index;

    auto eff = m_model->soundEffectMap()[index.row()].second;
    m_sound.reset(new QSound(m_model->soundEffectPathByType(eff)));
    m_sound->stop();
    m_sound->play();

    m_aniTimer->disconnect();
    auto item = static_cast<DStandardItem *>(m_listModel->itemFromIndex(index));
    auto aniAction = item->actionList(Qt::Edge::RightEdge)[0];
    int intervalal = 300;
    m_aniTimer->setInterval(intervalal);
    aniAction->setVisible(true);
    connect(m_aniTimer, &QTimer::timeout, this, [=] {
        auto aniIdx = (m_aniDuration / intervalal) % 3 + 1;
        auto icon = DIconTheme::findQIcon("dcc_volume" + QString::number(aniIdx));
        aniAction->setIcon(icon);

        m_aniDuration += intervalal;
        if (m_aniDuration > AnimationDuration) {
            aniAction->setVisible(false);
            m_aniTimer->stop();
            m_aniDuration = 0;
        }
        m_effectList->update(index);
    });

    m_aniTimer->start();
}

void SoundEffectsPage::initList()
{
    m_sw->setChecked(m_model->enableSoundEffect());

    m_listModel = new QStandardItemModel(this);
    m_effectList->setModel(m_listModel);
    connect(m_effectList, &DListView::clicked, this, &SoundEffectsPage::startPlay);
    connect(m_effectList, &DListView::activated, m_effectList, &QListView::clicked);
    connect(m_model,
            &SoundModel::soundEffectDataChanged,
            this,
            [=](DDesktopServices::SystemSoundEffect effect, const bool enable) {
                for (int idx = 0; idx < m_model->soundEffectMap().size(); ++idx) {
                    auto ite = m_model->soundEffectMap().at(idx);
                    if (ite.second != effect) {
                        continue;
                    }

                    auto items = static_cast<DStandardItem *>(m_listModel->item(idx));
                    if (items == nullptr || items->actionList(Qt::Edge::RightEdge).count() < 2) {
                        qWarning() << "items or items->actionList data is valid.";
                        continue;
                    }
                    auto action = items->actionList(Qt::Edge::RightEdge)[1];
                    auto checkstatus =
                            enable ? DStyle::SP_IndicatorChecked : DStyle::SP_IndicatorUnchecked;
                    auto icon = DStyle::standardIcon(style(), checkstatus);
                    action->setIcon(icon);
                    m_effectList->update(items->index());
                    break;
                }
            });

    QTimer::singleShot(0, this, [=] {
        QSize size(16, 16);
        DStandardItem *item = nullptr;
        for (auto se : m_model->soundEffectMap()) {
            item = new DStandardItem(se.first);
            item->setFontSize(DFontSizeManager::T8);
            auto action = new DViewItemAction(Qt::AlignVCenter, size, size, true);
            auto checkstatus = m_model->queryEffectData(se.second) ? DStyle::SP_IndicatorChecked
                                                                   : DStyle::SP_IndicatorUnchecked;
            auto icon = DStyle::standardIcon(style(), checkstatus);
            action->setIcon(icon);
            auto aniAction = new DViewItemAction(Qt::AlignVCenter, size, size);
            aniAction->setVisible(false);
            item->setActionList(Qt::Edge::RightEdge, { aniAction, action });
            m_listModel->appendRow(item);

            connect(action, &DViewItemAction::triggered, this, [=] {
                auto isSelected = m_model->queryEffectData(se.second);
                this->requestSetEffectAble(se.second, !isSelected);
                requestRefreshList();
            });
        }
    });
}
