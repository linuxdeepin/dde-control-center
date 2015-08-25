#include <QDebug>
#include <QApplication>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>

#include <libdui/dslider.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dlineedit.h>
#include <libdui/dheaderline.h>
#include <libdui/dswitchbutton.h>
#include <libdui/dimagebutton.h>
#include <libdui/dthememanager.h>
#include <libdui/dtextbutton.h>
#include <libdui/dbuttonlist.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dsearchedit.h>

#include "moduleheader.h"
#include "normallabel.h"
#include "addrmdoneline.h"

#include "keyboard.h"
#include "dbus/dbusinputdevices.h"
#include "dbus/dbuslangselector.h"
#include "firstletterclassify.h"
#include "keyboardlayoutitem.h"

DUI_USE_NAMESPACE

Keyboard::Keyboard() :
    QObject(),
    m_frame(new QFrame),
    m_dbusKeyboard(NULL)
{
    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    initBackend();
    if (m_dbusKeyboard) initUI();
}

Keyboard::~Keyboard()
{
    qDebug() << "~Keyboard and Language";
    m_frame->deleteLater();
}

void Keyboard::initBackend()
{
    DBusInputDevices * dbusInputDevices = new DBusInputDevices(this);
    foreach (InputDevice device, dbusInputDevices->infos()) {
        if (device.deviceType == "keyboard") {
            m_dbusKeyboard = new DBusKeyboard(this);
            break;
        }
    }
}

void Keyboard::updateKeyboardLayout(SearchList *button_list, AddRmDoneLine *line, bool showRemoveButton)
{
    QStringList user_layout_list = m_dbusKeyboard->userLayoutList();
    QString current_layout = m_dbusKeyboard->currentLayout();

    foreach (const QString &str, user_layout_list) {
        QDBusPendingReply<QString> tmp = m_dbusKeyboard->GetLayoutDesc(str);
        tmp.waitForFinished();

        m_mapUserLayoutInfo[tmp.value()] = str;

        KeyboardLayoutItem *item = new KeyboardLayoutItem(showRemoveButton&&str!=current_layout);
        connect(item, &KeyboardLayoutItem::removeButtonClicked, [=]{
            m_dbusKeyboard->DeleteUserLayout(str);
        });
        connect(line, &AddRmDoneLine::removeClicked, item, &KeyboardLayoutItem::showRemoveButton);
        connect(line, &AddRmDoneLine::doneClicked, item, &KeyboardLayoutItem::hideRemoveButton);
        item->setTitle(tmp.value());
        m_mapUserLayoutIndex[str] = button_list->addItem(item);

        if(str == current_layout){
            button_list->setCheckable(true);
            button_list->setCheckedItem(button_list->count()-1);
            button_list->setCheckable(!showRemoveButton);
        }
    }

    button_list->setFixedSize(310, button_list->count()*EXPAND_HEADER_HEIGHT);
}

void Keyboard::initUI()
{
    m_frame->setFixedWidth(310);
    m_frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    QVBoxLayout * mainLayout = new QVBoxLayout(m_frame);
    mainLayout->setMargin(10);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    /// Header
    ModuleHeader * header = new ModuleHeader("Keyboard and Language");
    mainLayout->addWidget(header);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addSpacing(10);
    connect(header, &ModuleHeader::resetButtonClicked, [=] {
        m_dbusKeyboard->Reset();
    });

    /// Basic Settings
    QGridLayout * basicSettingsLayout = new QGridLayout;
    basicSettingsLayout->setColumnStretch(0, 1);
    basicSettingsLayout->setColumnMinimumWidth(1, 180);
    basicSettingsLayout->setSpacing(20);
    basicSettingsLayout->setContentsMargins(0, 0, 14, 0);

    NormalLabel * repeatDelayTitle = new NormalLabel("Repeat Delay");
    DSlider * repeatDelaySlider = new DSlider(Qt::Horizontal);
    repeatDelaySlider->setRange(20, 600);
    repeatDelaySlider->setValue(m_dbusKeyboard->repeatDelay());
    basicSettingsLayout->addWidget(repeatDelayTitle, 0, 0, Qt::AlignRight);
    basicSettingsLayout->addWidget(repeatDelaySlider, 0, 1);

    connect(repeatDelaySlider, &DSlider::valueChanged, [=](int value){
        m_dbusKeyboard->setRepeatDelay(value);
    });
    connect(m_dbusKeyboard, &DBusKeyboard::RepeatDelayChanged, [=]{
        repeatDelaySlider->setValue(m_dbusKeyboard->repeatDelay());
    });

    NormalLabel * repeatSpeedTitle = new NormalLabel("Repeat Rate");
    DSlider * repeatSpeedSlider = new DSlider(Qt::Horizontal);
    repeatSpeedSlider->setRange(200, 1000);
    repeatSpeedSlider->setValue(1000 - (m_dbusKeyboard->repeatInterval() * 10 - 200));
    basicSettingsLayout->addWidget(repeatSpeedTitle, 1, 0, Qt::AlignRight);
    basicSettingsLayout->addWidget(repeatSpeedSlider, 1, 1);

    connect(repeatSpeedSlider, &DSlider::valueChanged, [=](int value){
        m_dbusKeyboard->setRepeatInterval(((1000 - value) + 200) / 10);
    });
    connect(m_dbusKeyboard, &DBusKeyboard::RepeatIntervalChanged, [=]{
        repeatSpeedSlider->setValue(1000 - (m_dbusKeyboard->repeatInterval() * 10 - 200));
    });

    QApplication * application = qobject_cast<QApplication*>(QApplication::instance());
    NormalLabel * cursorBlinkIntervalTitle = new NormalLabel("Cursor Blink Rate");
    DSlider * cursorBlinkIntervalSlider = new DSlider(Qt::Horizontal);
    cursorBlinkIntervalSlider->setRange(100, 2500);
    cursorBlinkIntervalSlider->setValue(2500 - (m_dbusKeyboard->cursorBlink() - 100));
    application->setCursorFlashTime(m_dbusKeyboard->cursorBlink());
    basicSettingsLayout->addWidget(cursorBlinkIntervalTitle, 2, 0, Qt::AlignRight);
    basicSettingsLayout->addWidget(cursorBlinkIntervalSlider, 2, 1);

    connect(cursorBlinkIntervalSlider, &DSlider::valueChanged, [=](int value){
        m_dbusKeyboard->setCursorBlink((2500 - value) + 100);
    });
    connect(m_dbusKeyboard, &DBusKeyboard::CursorBlinkChanged, [=]{
        cursorBlinkIntervalSlider->setValue(2500 - (m_dbusKeyboard->cursorBlink() - 100));
        application->setCursorFlashTime(m_dbusKeyboard->cursorBlink());
    });

    NormalLabel * testAreaTitle = new NormalLabel("Test Area");
    DLineEdit * testAreaEdit = new DLineEdit;
    basicSettingsLayout->addWidget(testAreaTitle, 3, 0, Qt::AlignRight);
    basicSettingsLayout->addWidget(testAreaEdit, 3, 1);

    mainLayout->addLayout(basicSettingsLayout);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(new DSeparatorHorizontal);

    DHeaderLine * capsLockLine = new DHeaderLine;
    DSwitchButton * capsLockSwitch = new DSwitchButton(capsLockLine);
    capsLockSwitch->setChecked(m_dbusKeyboard->capslockToggle());
    capsLockLine->setTitle("Caps Lock prompt");
    capsLockLine->setContent(capsLockSwitch);

    connect(capsLockSwitch, &DSwitchButton::checkedChanged, [=]{
        m_dbusKeyboard->setCapslockToggle(capsLockSwitch->checked());
    });

    connect(m_dbusKeyboard, &DBusKeyboard::CapslockToggleChanged, [=]{
        capsLockSwitch->setChecked(m_dbusKeyboard->capslockToggle());
    });

    DArrowLineExpand *language_expand = new DArrowLineExpand;
    language_expand->setTitle(tr("Language"));

    AddRmDoneLine *keyboardLayoutLine = new AddRmDoneLine;
    keyboardLayoutLine->setTitle(tr("Keyboard Layout"));
    keyboardLayoutLine->setRmButtonToolTip(tr("Remove Keyboard Layout"));
    keyboardLayoutLine->setAddButtonToolTip(tr("Add Keyboard Layout"));

    SearchList *user_layout_list = new SearchList;
    user_layout_list->setItemSize(290, EXPAND_HEADER_HEIGHT);
    user_layout_list->setCheckable(true);
    updateKeyboardLayout(user_layout_list, keyboardLayoutLine);
    keyboardLayoutLine->setRemoveHidden(user_layout_list->count()<2);

    FirstLetterClassify *letter_list = new FirstLetterClassify(m_frame);
    QDBusPendingReply<KeyboardLayoutList> list = m_dbusKeyboard->LayoutList();
    list.waitForFinished();
    QList<SearchItem*> tmp_list;
    KeyboardLayoutList tmp_map = list.value();

    foreach (const QString &str, tmp_map.keys()) {
        if(m_mapUserLayoutInfo.contains(tmp_map[str]))
            continue;

        KeyboardLayoutDelegate *tmpw = new KeyboardLayoutDelegate(tmp_map[str]);
        connect(tmpw, &KeyboardLayoutDelegate::checkedChanged, [=](bool checked){
           if(checked){
               m_dbusKeyboard->AddUserLayout(str);
               m_selectLayoutList << tmpw;
           }else{
               m_dbusKeyboard->DeleteUserLayout(str);
               m_selectLayoutList.removeOne(tmpw);
           }
        });

        tmp_list << tmpw;
    }

    letter_list->hide();
    letter_list->addItems(tmp_list);
    letter_list->setFixedWidth(310);

    connect(user_layout_list, &SearchList::countChanged, [=]{
        if(user_layout_list->isVisible()&&keyboardLayoutLine->doneButton()->isHidden())
            keyboardLayoutLine->setRemoveHidden(user_layout_list->count()<2);
    });

    connect(keyboardLayoutLine, &AddRmDoneLine::addClicked, [=]{
        keyboardLayoutLine->setAddHidden(true);
        keyboardLayoutLine->setRemoveHidden(true);
        keyboardLayoutLine->setDoneHidden(false);

        user_layout_list->hide();
        letter_list->show();
        letter_list->letterList()->setCurrentIndex(0);
        language_expand->setExpand(false);
    });
    connect(keyboardLayoutLine, &AddRmDoneLine::removeClicked, [=]{
        keyboardLayoutLine->setAddHidden(true);
        keyboardLayoutLine->setRemoveHidden(true);
        keyboardLayoutLine->setDoneHidden(false);

        user_layout_list->setCheckable(false);
    });
    connect(keyboardLayoutLine, &AddRmDoneLine::doneClicked, [=]{
        keyboardLayoutLine->setDoneHidden(true);
        keyboardLayoutLine->setAddHidden(false);
        keyboardLayoutLine->setRemoveHidden(user_layout_list->count()<2);

        letter_list->hide();
        user_layout_list->show();
        letter_list->removeItems(m_selectLayoutList);
        m_selectLayoutList.clear();

        user_layout_list->setCheckable(true);
    });

    connect(language_expand, &DArrowLineExpand::expandChange, [=](bool arg){
        if(arg&&keyboardLayoutLine->doneButton()->isVisible()){
            keyboardLayoutLine->setDoneHidden(true);
            keyboardLayoutLine->setAddHidden(false);
            keyboardLayoutLine->setRemoveHidden(user_layout_list->count()<2);

            letter_list->hide();
            user_layout_list->show();
            letter_list->removeItems(m_selectLayoutList);
            m_selectLayoutList.clear();

            user_layout_list->setCheckable(true);
        }
    });

    connect(user_layout_list, &SearchList::checkedItemChanged, [=](int index){
        if(index<0)
            return;

        QString str = m_mapUserLayoutInfo[user_layout_list->getItemData(index).toString()];

        if(m_dbusKeyboard->currentLayout() != str)
            m_dbusKeyboard->setCurrentLayout(str);
    });
    connect(m_dbusKeyboard, &DBusKeyboard::CurrentLayoutChanged, [=]{
        QString str = m_dbusKeyboard->currentLayout();
        int index = m_mapUserLayoutIndex[str];
        if(index>=0&&index<user_layout_list->count())
            user_layout_list->setCheckedItem(index);
    });

    connect(m_dbusKeyboard, &DBusKeyboard::UserLayoutListChanged, [=]{
        user_layout_list->clear();
        updateKeyboardLayout(user_layout_list, keyboardLayoutLine,
                             user_layout_list->isVisible()
                             &&keyboardLayoutLine->doneButton()->isVisible());
    });

    QFrame *lang_list_frame = new QFrame;
    lang_list_frame->setFixedWidth(310);

    QVBoxLayout *lang_frame_layout = new QVBoxLayout;
    lang_frame_layout->setMargin(0);

    DSearchEdit *lang_search = new DSearchEdit;
    lang_search->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    SearchList *language_searchList = new SearchList;
    language_searchList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    language_searchList->setCheckable(true);
    language_searchList->setFixedWidth(310);
    language_searchList->setItemSize(290, EXPAND_HEADER_HEIGHT);

    lang_frame_layout->addWidget(lang_search, 0, Qt::AlignTop);
    lang_frame_layout->addWidget(language_searchList, 100);
    lang_frame_layout->addStretch(1);

    lang_list_frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    lang_list_frame->setLayout(lang_frame_layout);
    language_expand->setContent(lang_list_frame);

    DbusLangSelector *dbusLangSelector = new DbusLangSelector(this);
    QDBusPendingReply<LocaleList> lang_list = dbusLangSelector->GetLocaleList();
    lang_list.waitForFinished();

    QString current_lang = dbusLangSelector->currentLocale();

    foreach (const LocaleInfo &info, lang_list.value()) {
        KeyboardLayoutItem *item = new KeyboardLayoutItem;
        QString theme = DThemeManager::instance()->theme();

        item->setKeyWords(QStringList()<<info.name);
        if(theme == "dark"){
            item->setImageNormal(":/lang_images/normal/"+info.id+".png");
        }else{
            item->setImageNormal(":/lang_images/dark/"+info.id+".png");
        }
        item->setImageHover(":/lang_images/hover/"+info.id+".png");
        item->setImagePress(":/lang_images/hover/"+info.id+".png");
        item->setImageChecked(":/lang_images/active/"+info.id+".png");

        language_searchList->addItem(item);
        if(info.id == current_lang){
            language_searchList->setCheckedItem(language_searchList->count()-1);
        }

        m_mapUserLayoutInfo[info.name] = info.id;
        m_mapUserLayoutIndex[info.id] = language_searchList->count()-1;
    }

    lang_list_frame->setMinimumHeight(lang_search->height()+language_searchList->height());
    language_searchList->beginSearch();

    connect(lang_search, &DSearchEdit::textChanged, [=]{
        language_searchList->setKeyWord(lang_search->text());
    });
    connect(language_searchList, &SearchList::checkedItemChanged, [=](int index){
        if(index<0)
            return;

        QString str = m_mapUserLayoutInfo[language_searchList->getItem(index)->keyWords()[0]];
        if(dbusLangSelector->currentLocale() != str){
            dbusLangSelector->SetLocale(str);
        }
    });
    connect(dbusLangSelector, &DbusLangSelector::CurrentLocaleChanged, [=]{
        QString str = dbusLangSelector->currentLocale();
        int index = m_mapUserLayoutIndex[str];
        if(index>=0&&index<language_searchList->count())
            language_searchList->setCheckedItem(index);
    });

    mainLayout->addWidget(capsLockLine);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(keyboardLayoutLine);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(user_layout_list);
    mainLayout->addWidget(letter_list);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(language_expand);

    mainLayout->addStretch(1);
}

QFrame* Keyboard::getContent()
{
    return m_frame;
}
