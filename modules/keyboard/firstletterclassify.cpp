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

    m_layout->setMargin(0);
    m_layout->addSpacing(HEADER_LEFT_MARGIN);
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_checkButton, 0, Qt::AlignVCenter|Qt::AlignRight);
    m_layout->addSpacing(HEADER_RIGHT_MARGIN);

    setLayout(m_layout);

    connect(m_checkButton, &MultiAddCheckButton::checkedChanged, this, &KeyboardLayoutDelegate::checkedChanged);
}

QStringList KeyboardLayoutDelegate::keyWords() const
{
    if(!title().isEmpty())
        return QStringList()<<title()[0].toUpper();

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
    return m_checkButton->checked();
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

    m_letterList->setObjectName("LetterList");

    m_layout->setMargin(0);
    m_layout->addWidget(m_letterList);
    m_layout->addWidget(m_searchList);
    m_layout->addStretch(1);
    setLayout(m_layout);

    connect(m_letterList, &DSegmentedControl::currentTitleChanged, [&](const QString& key){
        m_searchList->setKeyWord(key);
        emit currentLetterChanged(key);
        setMinimumHeight(m_searchList->sizeHint().height()+m_letterList->sizeHint().height());
    });
}

SearchList *FirstLetterClassify::searchList() const
{
    return m_searchList;
}

DSegmentedControl *FirstLetterClassify::letterList() const
{
    return m_letterList;
}

void FirstLetterClassify::addItems(QList<SearchItem*> datas)
{
    foreach (SearchItem* data, datas) {
        m_mapLetters[data->keyWords()[0][0].toUpper()] = true;
    }
    m_searchList->addItems(datas);
    m_searchList->beginSearch();

    for(char ch = 'A'; ch<='Z'; ch++){
        if(m_mapLetters[ch]){
            m_letterList->addSegmented(QString(ch));
        }
    }

    m_letterList->setCurrentIndex(1);
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
        m_searchList->removeItem(m_searchList->indexOf(item));
    }
}

QString FirstLetterClassify::currentLetter() const
{
    return m_letterList->getText(m_letterList->currentIndex());
}

void FirstLetterClassify::setCurrentLetter(QString currentLetter)
{
    m_letterList->setCurrentIndexByTitle(currentLetter);
}
