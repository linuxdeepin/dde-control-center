#include "thirdpage1.h"

#include "shortcutitem.h"

#include <widgets/labels/tipslabel.h>
#include <widgets/settingsgroup.h>
#include <widgets/switchwidget.h>

#include <DListView>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>

DWIDGET_USE_NAMESPACE
using namespace dcc::widgets;

ThirdPage1::ThirdPage1(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *contentLayout = new QVBoxLayout(this);
    contentLayout->setSpacing(0);
    contentLayout->setAlignment(Qt::AlignTop);
    setLayout(contentLayout);

    TipsLabel *tip = new TipsLabel(tr("Read the selected text"));
    contentLayout->addWidget(tip);
    contentLayout->addSpacing(10);

    SettingsGroup *generalSettingsGrp = new SettingsGroup;
    generalSettingsGrp->setSpacing(10);
    generalSettingsGrp->getLayout()->setMargin(0);
    contentLayout->addWidget(generalSettingsGrp);

    SwitchWidget *readSwitch = new SwitchWidget(tr("Read the selected text"));
    generalSettingsGrp->appendItem(readSwitch);

    ShortcutItem *shortcut = new ShortcutItem();
    generalSettingsGrp->appendItem(shortcut, SettingsGroup::NoneBackground);
    shortcut->addBackground();
    shortcut->setTitle(tr("Shortcut"));
    shortcut->setShortcut("Control-C");

    // 音源
    SettingsItem *sourceSetting = new SettingsItem();
    sourceSetting->addBackground();
    generalSettingsGrp->appendItem(sourceSetting);

    QHBoxLayout *sourceLayout = new QHBoxLayout();
    sourceSetting->setLayout(sourceLayout);

    QLabel *sourceLabel = new QLabel(tr("Source"));
    sourceLayout->addWidget(sourceLabel);

    QComboBox *sourceCombo = new QComboBox();
    sourceLayout->addWidget(sourceCombo);
    sourceCombo->addItem("xxx");
    sourceCombo->addItem("yyy");
    sourceCombo->addItem("zzz");

}
