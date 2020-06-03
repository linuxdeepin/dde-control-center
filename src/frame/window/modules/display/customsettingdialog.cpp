/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#include "customsettingdialog.h"

#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"
#include "modules/display/monitorcontrolwidget.h"
#include "modules/display/monitorindicator.h"
#include "widgets/basiclistview.h"

#include <DSuggestButton>

#include <QLabel>
#include <QListView>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDebug>

using namespace dcc::display;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::display;
DWIDGET_USE_NAMESPACE

CustomSettingDialog::CustomSettingDialog(QWidget *parent)
    : DAbstractDialog(parent)
    , m_isPrimary(true)
{
    initUI();
}

CustomSettingDialog::CustomSettingDialog(dcc::display::Monitor *mon,
                                         dcc::display::DisplayModel *model,
                                         QWidget *parent)
    : DAbstractDialog(parent)
    , m_isPrimary(false)
    , m_model(model)
{
    initUI();
    resetMonitorObject(mon);
}


CustomSettingDialog::~CustomSettingDialog()
{
    qDeleteAll(m_otherDialog);
    m_fullIndication->deleteLater();
}

void CustomSettingDialog::initUI()
{
    setMinimumWidth(480);
    setMinimumHeight(600);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_layout = new QVBoxLayout();
    m_listLayout = new QVBoxLayout();

    auto btnBox = new DButtonBox(this);
    m_layout->addWidget(btnBox, 0, Qt::AlignHCenter);

    auto initlistfunc = [](DListView * list) {
        list->setEditTriggers(DListView::NoEditTriggers);
        list->setSelectionMode(DListView::NoSelection);
        list->setSizeAdjustPolicy(DListView::AdjustToContents);
        list->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    };

    if (m_isPrimary) {
        m_moniList = new DListView;
        initlistfunc(m_moniList);
        m_listLayout->addWidget(m_moniList);
        m_vSegBtn << new DButtonBoxButton(tr("Main Screen"));
    }

    m_resolutionList = new DListView;
    initlistfunc(m_resolutionList);
    m_resolutionList->setVisible(!m_isPrimary);
    m_vSegBtn << new DButtonBoxButton(tr("Resolution"));
    m_listLayout->addWidget(m_resolutionList);

    m_rateList = new DListView;
    m_rateList->setVisible(false);
    initlistfunc(m_rateList);
    m_vSegBtn << new DButtonBoxButton(tr("Refresh Rate"));
    btnBox->setButtonList(m_vSegBtn, true);
    m_vSegBtn[0]->setChecked(true);
    m_listLayout->addWidget(m_rateList);

    connect(btnBox, &DButtonBox::buttonToggled, this, &CustomSettingDialog::onChangList);

    m_layout->addLayout(m_listLayout);
    setLayout(m_layout);

    QHBoxLayout *hlayout = new QHBoxLayout();
    m_layout->addLayout(hlayout);

    QPushButton *rotate = new QPushButton(this);
    rotate->setIcon(QIcon::fromTheme("dcc_rotate"));

    hlayout->addWidget(rotate, 0, Qt::AlignLeft);
    connect(rotate, &QPushButton::clicked, this, [this]() {
        if (m_model->isMerge()) {
            Q_EMIT CustomSettingDialog::requestShowRotateDialog(nullptr);
        } else {
            Q_EMIT CustomSettingDialog::requestShowRotateDialog(m_monitor);
        }
    });

    hlayout->setMargin(10);

    if (m_isPrimary) {
        QPushButton *btn{nullptr};
        btn = new QPushButton(tr("Cancel"), this);
        connect(btn, &QPushButton::clicked, this, &CustomSettingDialog::reject);
        hlayout->addWidget(btn);

        btn = new DSuggestButton(tr("Save"), this);
        connect(btn, &DSuggestButton::clicked, this, &CustomSettingDialog::accept);
        hlayout->addWidget(btn);
    }

    m_fullIndication = std::unique_ptr<MonitorIndicator>(new MonitorIndicator());
    m_fullIndication->show();
    m_fullIndication->setVisible(false);

    for (auto obj : this->children()) {
        QWidget* item = qobject_cast<QWidget*>(obj);
        if(item !=nullptr){
            item->setFocusPolicy(Qt::NoFocus);
        }
    }
    for (auto item : m_vSegBtn) {
        item->setFocusPolicy(Qt::NoFocus);
    }
}

void CustomSettingDialog::setModel(DisplayModel *model)
{
    m_model = model;
    if (model->isRefreshRateEnable() == false) {
        for (auto btn : m_vSegBtn) {
            if (btn->text() == tr("Refresh Rate")) {
                btn->hide();
                break;
            }
        }
    }

    resetMonitorObject(model->primaryMonitor());
    m_isPrimary = true;
    initPrimaryDialog();

    initWithModel();
    initOtherDialog();
    initConnect();

    resetDialog();
}

void CustomSettingDialog::initWithModel()
{
    Q_ASSERT(m_model);
    if (m_model->isRefreshRateEnable() == false) {
        for (auto btn : m_vSegBtn) {
            if (btn->text() == tr("Refresh Rate")) {
                btn->hide();
                break;
            }
        }
    }
    initMoniControlWidget();
    initResolutionList();
    initRefreshrateList();

    if (m_moniList)
        m_moniList->setVisible(!m_model->isMerge()
                               && m_vSegBtn.at(0)->isChecked()
                               && m_vSegBtn.size() > 2);

    qDebug() << "isMerge:" << m_model->isMerge();
    qDebug() << "select monitor checked:" << m_vSegBtn.at(0)->isChecked();
    if (m_monitor->isPrimary())
        m_vSegBtn.at(0)->setVisible(!m_model->isMerge());

    if (m_model->isMerge() && m_vSegBtn.at(0)->isChecked())
        m_vSegBtn.at(1)->setChecked(true);
}

void CustomSettingDialog::initOtherDialog()
{
    if (m_otherDialog.size()) {
        for (auto dlg : m_otherDialog) {
            dlg->setVisible(false);
        }
    }

    int dlgIdx = 0;
    for (int idx = 0; idx < m_model->monitorList().size(); ++idx) {
        auto mon = m_model->monitorList()[idx];
        if (mon == m_model->primaryMonitor())
            continue;
        CustomSettingDialog *dlg = nullptr;
        if (dlgIdx < m_otherDialog.size()) {
            dlg = m_otherDialog[dlgIdx];
            dlg->resetMonitorObject(mon);
            ++dlgIdx;
        } else {
            dlg = new CustomSettingDialog(mon, m_model, this);
            m_otherDialog.append(dlg);

            dlg->initConnect();

            connect(dlg, &CustomSettingDialog::requestSetResolution, this,
                    &CustomSettingDialog::requestSetResolution);
            connect(dlg, &CustomSettingDialog::requestShowRotateDialog, this,
                    &CustomSettingDialog::requestShowRotateDialog);

            connect(dlg, &CustomSettingDialog::requestMerge, this,
                    &CustomSettingDialog::requestMerge);
            connect(dlg, &CustomSettingDialog::requestSplit, this,
                    &CustomSettingDialog::requestSplit);
            connect(dlg, &CustomSettingDialog::requestRecognize, this,
                    &CustomSettingDialog::requestRecognize);
        }

        dlg->initWithModel();
        if (!m_model->isMerge()) {
            dlg->show();
        }

        dlg->resetDialog();
    }
}

void CustomSettingDialog::initRefreshrateList()
{
    if (m_freshListModel)
        m_freshListModel->clear();
    else
        m_freshListModel = new QStandardItemModel(this);
    auto modes = m_monitor->modeList();
    m_rateList->setModel(m_freshListModel);
    Resolution pevR;

    auto moni = m_monitor;
    QList<double> rateList;
    bool isFirst = true;
    for (auto m : moni->modeList()) {
        if (!Monitor::isSameResolution(m, moni->currentMode()))
            continue;

        if (m_model->isMerge()) {
            bool isCommen = true;;
            for (auto tmonitor : m_model->monitorList()) {
                if (!tmonitor->hasResolutionAndRate(m)) {
                    isCommen = false;
                    break;
                }
            }

            if (!isCommen)
                continue;
        }
        auto trate = m.rate();
        DStandardItem *item = new DStandardItem;
        m_freshListModel->appendRow(item);

        auto tstr = QString::number(trate, 'g', 4) + tr("Hz");
        if (isFirst) {
            tstr += QString(" (%1)").arg(tr("Recommended"));
            isFirst = false;
        }
        if (fabs(trate - moni->currentMode().rate()) < 0.000001) {
            item->setCheckState(Qt::CheckState::Checked);
        } else {
            item->setCheckState(Qt::CheckState::Unchecked);
        }
        qDebug() << "set item id data:" << m.id();
        item->setData(QVariant(m.id()), IdRole);
        item->setData(QVariant(m.rate()), RateRole);
        item->setData(QVariant(m.width()), WidthRole);
        item->setData(QVariant(m.height()), HeightRole);
        item->setText(tstr);
    }
}

void CustomSettingDialog::initResolutionList()
{
    if (m_resolutionListModel)
        m_resolutionListModel->clear();
    else
        m_resolutionListModel = new QStandardItemModel(this);
    m_resolutionList->setModel(m_resolutionListModel);

    bool first = true;
    auto modes = m_monitor->modeList();
    const auto curMode = m_monitor->currentMode();

    DStandardItem *curIdx{nullptr};
    Resolution pevR;
    for (auto m : modes) {
        if (Monitor::isSameResolution(pevR, m)) {
            continue;
        }

        pevR = m;
        if (m_model->isMerge()) {
            bool isComm = true;
            for (auto moni : m_model->monitorList()) {
                if (!moni->hasResolution(m)) {
                    isComm = false;
                    break;
                }
            }

            if (!isComm) {
                continue;
            }
        }

        const QString res = QString::number(m.width()) + "×" + QString::number(m.height());
        auto *item = new DStandardItem();

        item->setData(QVariant(m.id()), IdRole);
        item->setData(QVariant(m.rate()), RateRole);
        item->setData(QVariant(m.width()), WidthRole);
        item->setData(QVariant(m.height()), HeightRole);
        if (first) {
            first = false;
            item->setText(res + QString(" (%1)").arg(tr("Recommended")));
        } else {
            item->setText(res);
        }

        if (Monitor::isSameResolution(curMode, m))
            curIdx = item;
        m_resolutionListModel->appendRow(item);
    }

    m_resolutionList->setModel(m_resolutionListModel);
    if (nullptr != curIdx)
        curIdx->setCheckState(Qt::Checked);
}

void CustomSettingDialog::initMoniList()
{
    auto listModel = new QStandardItemModel;
    m_moniList->setModel(listModel);

    auto moniList = m_model->monitorList();
    for (int idx = 0; idx < moniList.size(); ++idx) {
        auto item = new DStandardItem;
        item->setIcon(QIcon::fromTheme(idx % 2 ? "dcc_display_vga1" : "dcc_display_lvds1"));

        auto moni = moniList[idx];
        auto *titleAction = new DViewItemAction;
        titleAction->setText(moni->name());

        auto *subTitleAction = new DViewItemAction;
        QString str = QString("%1 %2 ").arg(QString::number(qSqrt(pow(moni->mmWidth(), 2) + pow(moni->mmHeight(), 2)) / 25.4, 'g', 3)).arg(tr("inch"));
        str += (tr("Resolution %1x%2").arg(QString::number(moni->w())).arg(QString::number(moni->h())));
        subTitleAction->setText(str);

        DViewItemActionList actionList;
        actionList << titleAction << subTitleAction;
        item->setTextActionList(actionList);
        listModel->appendRow(item);

        if (moni->isPrimary())
            item->setCheckState(Qt::Checked);
    }

    connect(m_moniList, &DListView::clicked, this, [this] {
        this->requestSetPrimaryMonitor(m_moniList->currentIndex().row());
    });
    connect(m_model, &DisplayModel::primaryScreenChanged, this, [ = ] {
        Q_ASSERT(listModel->rowCount() == m_model->monitorList().size());

        auto monis = m_model->monitorList();
        for (int idx = 0 ; idx < listModel->rowCount(); ++idx) {
            auto item = listModel->item(idx);
            if (idx < monis.size()) {
                item->setCheckState(monis[idx] == m_model->primaryMonitor() ? Qt::Checked : Qt::Unchecked);
            }
        }
    });
}

void CustomSettingDialog::initMoniControlWidget()
{
    if (m_monitroControlWidget)
        m_monitroControlWidget->deleteLater();
    m_monitroControlWidget = new MonitorControlWidget();

    m_monitroControlWidget->setScreensMerged(m_model->isMerge());
    m_monitroControlWidget->setDisplayModel(m_model, m_isPrimary ? nullptr : m_monitor);

    connect(m_monitroControlWidget, &MonitorControlWidget::requestMonitorPress,
            this, &CustomSettingDialog::onMonitorPress);
    connect(m_monitroControlWidget, &MonitorControlWidget::requestMonitorRelease,
            this, &CustomSettingDialog::onMonitorRelease);
    connect(m_monitroControlWidget, &MonitorControlWidget::requestRecognize,
            this, &CustomSettingDialog::requestRecognize);
    connect(m_monitroControlWidget, &MonitorControlWidget::requestMerge,
            this, &CustomSettingDialog::requestMerge);
    connect(m_monitroControlWidget, &MonitorControlWidget::requestSplit,
            this, &CustomSettingDialog::requestSplit);
    connect(m_monitroControlWidget, &MonitorControlWidget::requestSetMonitorPosition,
            this, &CustomSettingDialog::requestSetMonitorPosition);

    m_layout->insertWidget(0, m_monitroControlWidget);
}

void CustomSettingDialog::initPrimaryDialog()
{
    Q_ASSERT(m_moniList);
    initMoniList();
}

void CustomSettingDialog::initConnect()
{
    connect(m_resolutionList, &QListView::clicked, this, [this](QModelIndex idx) {
        auto check = m_resolutionListModel->data(idx, Qt::CheckStateRole);
        if (check == Qt::Checked)
            return;

        auto w = m_resolutionListModel->data(idx, WidthRole).toInt();
        auto h = m_resolutionListModel->data(idx, HeightRole).toInt();
        auto id = m_resolutionListModel->data(idx, IdRole).toInt();

        if (m_model->isMerge()) {
            if (w == m_monitor->currentMode().width()
                    && h == m_monitor->currentMode().height()) {
                return;
            }

            ResolutionDate res;
            res.w = w;
            res.h = h;
            this->requestSetResolution(nullptr, res);
        } else {
            if (id == m_monitor->currentMode().id()) {
                return;
            }

            ResolutionDate res;
            res.id = id;
            this->requestSetResolution(m_monitor, res);
        }
    });
    connect(m_rateList, &DListView::clicked, this, [this](QModelIndex idx) {
        auto lm = m_rateList->model();
        auto check = lm->data(idx, Qt::CheckStateRole);
        if (check == Qt::Checked)
            return;

        if (m_model->isMerge()) {
            auto cm = m_model->primaryMonitor()->currentMode();
            auto w = cm.width();
            auto h = cm.height();
            auto rate = lm->data(idx, RateRole).toDouble();

            qDebug() << rate;
            if (fabs(cm.rate() - rate) < 0.00001) {
                return;
            }

            qDebug() << rate;
            ResolutionDate res;
            res.w = w;
            res.h = h;
            res.rate = rate;
            this->requestSetResolution(nullptr, res);
        } else {
            auto id = lm->data(idx, IdRole).toInt();
            if (id == m_monitor->currentMode().id()) {
                return;
            }

            ResolutionDate res;
            res.id = id;
            qDebug() << "request set resolution to id :" << id;
            this->requestSetResolution(m_monitor, res);
        }
    });
    connect(m_model, &DisplayModel::monitorListChanged, this, [this] {
        if (m_monitroControlWidget)
            m_monitroControlWidget->deleteLater();
        m_monitroControlWidget = nullptr;
    });

    if (m_isPrimary) {
        connect(m_model, &DisplayModel::primaryScreenChanged,
                this, &CustomSettingDialog::onPrimaryMonitorChanged);
        connect(m_model, &DisplayModel::isMergeChange,
                this, &CustomSettingDialog::onPrimaryMonitorChanged);
    }
    connect(m_model, &DisplayModel::screenWidthChanged, this, &CustomSettingDialog::resetDialog);
    connect(m_model, &DisplayModel::screenHeightChanged, this, &CustomSettingDialog::resetDialog);
    connect(m_model, &DisplayModel::isMergeChange, m_monitroControlWidget, &MonitorControlWidget::setScreensMerged);
}

void CustomSettingDialog::resetMonitorObject(Monitor *moni)
{
    if (m_monitor == moni)
        return;

    if (!m_monitor) {
        disconnect(m_monitor, &Monitor::currentModeChanged, this, &CustomSettingDialog::onMonitorModeChange);
        disconnect(m_monitor, &Monitor::scaleChanged, this, &CustomSettingDialog::resetDialog);
        disconnect(m_monitor, &Monitor::geometryChanged, this, &CustomSettingDialog::resetDialog);
    }

    m_monitor = moni;
    connect(m_monitor, &Monitor::currentModeChanged, this, &CustomSettingDialog::onMonitorModeChange);
    connect(m_monitor, &Monitor::scaleChanged, this, &CustomSettingDialog::resetDialog);
    connect(m_monitor, &Monitor::geometryChanged, this, &CustomSettingDialog::resetDialog);
}

void CustomSettingDialog::onChangList(QAbstractButton *btn, bool beChecked)
{
    if (!beChecked)
        return;

    if (m_moniList)
        m_moniList->setVisible(false);

    m_resolutionList->setVisible(false);
    m_rateList->setVisible(false);

    auto segBtn = qobject_cast<DButtonBoxButton *>(btn);
    auto btnIdx = m_vSegBtn.indexOf(segBtn);

    switch (btnIdx) {
    case 0:
        if (m_isPrimary) {
            m_moniList->setVisible(true);
        } else {
            m_resolutionList->setVisible(true);
        }
        break;
    case 1:
        if (m_isPrimary) {
            m_resolutionList->setVisible(true);
        } else {
            m_rateList->setVisible(true);
        }
        break;
    case 2:
        m_rateList->setVisible(true);
        break;
    default:
        break;
    }
}

void CustomSettingDialog::onMonitorModeChange(const Resolution &r)
{
    initResolutionList();
    initRefreshrateList();
    resetDialog();
}

void CustomSettingDialog::resetDialog()
{
    //当收到屏幕变化的消息后，屏幕数据还是旧的
    //需要用QTimer把对窗口的改变放在屏幕数据应用后
    QTimer::singleShot(sender() ? 1000 : 0, this, [ = ] {
        auto rt = rect();
        if (rt.width() > m_monitor->w())
            rt.setWidth(m_monitor->w());

        if (rt.height() > m_monitor->h())
            rt.setHeight(m_monitor->h());

        auto mrt = m_monitor->rect();
        auto tsize = (mrt.size() / m_model->monitorScale(m_monitor) - rt.size()) / 2;

        qDebug() << Q_FUNC_INFO << "-----------------------";

        qDebug() << "monitor name:" << m_monitor->name();
        qDebug() << "rt :" << rt;
        qDebug() << "tsize :" << tsize;
        qDebug() << "scale :" << m_model->monitorScale(m_monitor);
        rt.moveTo(m_monitor->x() + tsize.width(), m_monitor->y() + tsize.height());

        qDebug() << "mrt :" << mrt;
        qDebug() << "final rt :" << rt;
        setGeometry(rt);
    });
}

void CustomSettingDialog::onPrimaryMonitorChanged()
{
    resetMonitorObject(m_model->primaryMonitor());

    initWithModel();
    initOtherDialog();

    resetDialog();
}

void CustomSettingDialog::onMonitorPress(Monitor *mon)
{
    m_fullIndication->setGeometry(mon->rect());

    m_fullIndication->setVisible(true);
}

void CustomSettingDialog::onMonitorRelease(Monitor *mon)
{
    Q_UNUSED(mon)
    m_fullIndication->setVisible(false);
}
