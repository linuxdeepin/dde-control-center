#include <libdui/dconstants.h>
#include <libdui/libdui_global.h>
#include <libdui/dthememanager.h>

#include "imagenamebutton.h"
#include "multiaddcheckbutton.h"

#include "firstletterclassify.h"

DUI_USE_NAMESPACE

KeyboardLayoutDelegate::KeyboardLayoutDelegate(const QString &title, QWidget *parent):
    QFrame(parent),
    m_widget(this),
    m_layout(new QHBoxLayout),
    m_label(new QLabel),
    m_checkButton(new MultiAddCheckButton)
{
    D_THEME_INIT_WIDGET(KeyboardLayoutDelegate);

    setTitle(title);

    m_checkButton->setCheckable(true);
    m_checkButton->setChecked(false);
    m_checked = false;

    m_layout->setMargin(0);
    m_layout->addSpacing(HEADER_LEFT_MARGIN);
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_checkButton, 0, Qt::AlignVCenter|Qt::AlignRight);
    m_layout->addSpacing(HEADER_RIGHT_MARGIN);

    setLayout(m_layout);

    connect(m_checkButton, &MultiAddCheckButton::stateChanged, [&]{
        if(m_checked != m_checkButton->isChecked()){
            m_checked = m_checkButton->isChecked();
            emit checkedChanged(m_checked);
        }
    });
}

QStringList KeyboardLayoutDelegate::keyWords() const
{
    if(!title().isEmpty()){
        QDBusInterface dbus_pinyin( "com.deepin.api.Pinyin", "/com/deepin/api/Pinyin",
                                  "com.deepin.api.Pinyin" );

        QDBusMessage result = dbus_pinyin.call("Query", QString(title()[0]));

        QStringList list;
        foreach(const QString &str, result.arguments()[0].toStringList())
            list << str[0].toUpper();
        return list;
    }

    return QStringList();
}

void KeyboardLayoutDelegate::setData(const QVariant &datas)
{
    if(datas.type() == QVariant::String){
        setTitle(datas.toString());
    }
}

QVariant KeyboardLayoutDelegate::getData()
{
    return m_label->text();
}

QWidget *KeyboardLayoutDelegate::widget() const
{
    return m_widget;
}

QString KeyboardLayoutDelegate::title() const
{
    return m_label->text();
}

bool KeyboardLayoutDelegate::checked() const
{
    return m_checkButton->isChecked();
}

void KeyboardLayoutDelegate::setChecked(bool checked)
{
    m_checkButton->setChecked(checked);
}

void KeyboardLayoutDelegate::setTitle(const QString &title)
{
    m_label->setText(title);
}

FirstLetterClassify::FirstLetterClassify(QWidget *parent) :
    QFrame(parent),
    m_layout(new QVBoxLayout),
    m_letterList(new DSegmentedControl),
    m_searchList(new SearchList)
{
    D_THEME_INIT_WIDGET(FirstLetterClassify);

    m_searchList->setItemSize(310, EXPAND_HEADER_HEIGHT);
    m_searchList->setCheckable(false);
    m_searchList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_letterList->setObjectName("LetterList");

    m_layout->setMargin(0);
    m_layout->addWidget(m_letterList);
    m_layout->addWidget(m_searchList);
    m_layout->addStretch(1);
    setLayout(m_layout);
}

SearchList *FirstLetterClassify::searchList() const
{
    return m_searchList;
}

DSegmentedControl *FirstLetterClassify::letterList() const
{
    return m_letterList;
}

void FirstLetterClassify::addItem(SearchItem *data)
{
    foreach (QString pinyin, data->keyWords()) {
        int ch = pinyin[0].toLatin1()-65;

        if(ch>=0)
            ++m_mapLetters[ch];
    }

    m_searchList->addItem(data);
}

void FirstLetterClassify::addEnd()
{
    if(m_searchList->isSearching())
        m_searchList->updateKeyWords();
    else
        m_searchList->beginSearch();

    for(int ch = 0; ch<26; ch++){
        if(m_mapLetters[ch] > 0){
            if(m_letterList->indexByTitle(QString(ch+65)) < 0)
                m_letterList->addSegmented(QString(ch+65));
        }
    }

    m_letterList->setCurrentIndex(1);

    connect(m_letterList, &DSegmentedControl::currentTitleChanged, [&](const QString& key){
        m_searchList->setKeyWord(key);
        emit currentLetterChanged(key);
    });

    foreach (QObject *obj, m_letterList->children()) {
        QWidget *w = qobject_cast<QWidget*>(obj);
        if(w){
            w->setObjectName("");
        }
    }
}

void FirstLetterClassify::removeItems(QList<SearchItem *> datas)
{
    foreach (SearchItem* item, datas) {
        foreach (QString pinyin, item->keyWords()) {
            int ch = pinyin[0].toLatin1()-65;

            if(ch >= 0){
                --m_mapLetters[ch];

                if(m_mapLetters[ch] <= 0)
                    m_letterList->removeSegmented(m_letterList->indexByTitle(QString(ch+65)));
            }
        }

        m_searchList->removeItem(m_searchList->indexOf(item));
    }

    m_searchList->updateKeyWords();
}

QString FirstLetterClassify::currentLetter() const
{
    return m_letterList->getText(m_letterList->currentIndex());
}

void FirstLetterClassify::setCurrentLetter(QString currentLetter)
{
    m_letterList->setCurrentIndexByTitle(currentLetter);
}
