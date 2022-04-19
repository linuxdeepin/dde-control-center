/*
* Copyright (C) 2019 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>

* Maintainer: guoyao <guoyao@uniontech.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "searchwidget.h"
#include "interface/moduleobject.h"

#include <DPinyin>

#include <QKeyEvent>
#include <QAbstractItemView>
#include <QPainter>
#include <QStandardItemModel>

DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

const QStringList &FilterText{"-", "--", "-->", "->", ">", "/"};

SearchData::SearchData(QObject *parent)
    : QObject(parent) {}

SearchData::SearchData(const QString &url, const QString &searchUrl, const QString &pinYin, const QList<ModuleObject *> &moduleUrl, QObject *parent)
    : QObject(parent), Url(url), SearchUrl(searchUrl), PinYin(pinYin), ModuleUrl(moduleUrl) {}

SearchData::SearchData(const SearchData &m)
{
    Url = m.Url;
    SearchUrl = m.SearchUrl;
    PinYin = m.PinYin;
    ModuleUrl = m.ModuleUrl;
}

SearchData &SearchData::operator=(const SearchData &other)
{
    if (this != &other) {
        Url = other.Url;
        SearchUrl = other.SearchUrl;
        PinYin = other.PinYin;
        ModuleUrl = other.ModuleUrl;
    }
    return *this;
}

bool SearchData::operator==(const SearchData &other) const
{
    return Url == other.Url && SearchUrl == other.SearchUrl && PinYin == other.PinYin;
}

QDebug SearchData::operator<<(QDebug d) const
{
    d << QString("SearchData(Url:%1, SearchUrl:%2, PinYin:%3)")
            .arg(Url).arg(SearchUrl).arg(PinYin);
    return d;
}


DccCompleter::DccCompleter(QObject *parent)
    : QCompleter(parent)
{

}

DccCompleter::DccCompleter(QAbstractItemModel *model, QObject *parent)
    : QCompleter(model, parent)
{

}

bool DccCompleter::eventFilter(QObject *o, QEvent *e)
{
    // 匹配列表隐藏时，将滑动条恢复到顶部
    if (e->type() == QEvent::Hide) {
        popup()->scrollToTop();
    }

    if (e->type() == QEvent::FocusOut) {
        return QCompleter::eventFilter(o, e);
    }

    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        QModelIndex keyIndex;
        switch (ke->key()) {
        case Qt::Key_Up: {
            if (popup()->currentIndex().row() == 0) {
                keyIndex = popup()->model()->index(popup()->model()->rowCount() - 1, 0);
                popup()->setCurrentIndex(keyIndex);
            } else {
                keyIndex = popup()->model()->index(popup()->currentIndex().row() - 1, 0);
                popup()->setCurrentIndex(keyIndex);
            }
            return true;
        }
        case Qt::Key_Down: {
            if (popup()->currentIndex().row() == popup()->model()->rowCount() - 1) {
                keyIndex = popup()->model()->index(0, 0);
                popup()->setCurrentIndex(keyIndex);
            } else {
                keyIndex = popup()->model()->index(popup()->currentIndex().row() + 1, 0);
                popup()->setCurrentIndex(keyIndex);
            }
            return true;
        }
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (popup()->isVisible() && !popup()->currentIndex().isValid()) {
                keyIndex = popup()->model()->index(0, 0);
                popup()->setCurrentIndex(keyIndex);
            }
            popup()->hide();
            //原因：在QCompleter上执行回车操作会触发它的activated()信号，回车也会触发DLineEdit的returnPressed()信号
            //导致一个操作触发两次执行，模块load()会被执行两次，导致程序崩溃
            //方法:在QCompleter的eventFilter()函数中对回车操作做过滤，直接触发DLineEdit的returnPressed()信号，
            //不继续将事件传给QCompleter，保证回车只触发一次操作
            Q_EMIT static_cast<SearchWidget *>(this->parent())->returnPressed();
            return true;
        }
    }
    return QCompleter::eventFilter(o, e);
}


DccCompleterStyledItemDelegate::DccCompleterStyledItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

void DccCompleterStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled)
                              ? QPalette::Normal : QPalette::Disabled;
    if (cg == QPalette::Normal && !(option.state & QStyle::State_Active)) {
        cg = QPalette::Inactive;
    }

    if (option.showDecorationSelected && (option.state & (QStyle::State_Selected | QStyle::State_MouseOver))) {
        painter->fillRect(option.rect, option.palette.brush(cg, QPalette::Highlight));
    }
    QIcon itemIcon = QIcon::fromTheme(index.data(Qt::UserRole + 1).toString());
    QSize iconSize = QSize(option.rect.height() - 2, option.rect.height() - 2);
    painter->drawPixmap(QRect(0, option.rect.y(), option.rect.height() - 0, option.rect.height() - 2), itemIcon.pixmap(iconSize));

    // draw text
    if (option.state & (QStyle::State_Selected | QStyle::State_MouseOver)) {
        painter->setPen(option.palette.color(cg, QPalette::HighlightedText));
    }
    else {
        painter->setPen(option.palette.color(cg, QPalette::Text));
    }
    painter->setFont(option.font);
    painter->drawText(option.rect.adjusted(option.rect.height() + 8, 0, 0, 0), Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());
}

QSize DccCompleterStyledItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize s = QStyledItemDelegate::sizeHint(option, index);
    s.setHeight(24);
    return s;
}


SearchWidget::SearchWidget(QWidget *parent)
    : DSearchEdit(parent)
    , m_model(new QStandardItemModel(this))
    , m_completer(new DccCompleter(m_model, this))
    , m_insertIndex(0)
    , m_bIsChinese(false)
{
    const QString &language = QLocale::system().name();
    m_bIsChinese = language == "zh_CN" || language == "zh_HK" || language == "zh_TW";

    DccCompleterStyledItemDelegate *delegate = new DccCompleterStyledItemDelegate(m_completer);
    m_completer->popup()->setItemDelegate(delegate);
    m_completer->popup()->setAttribute(Qt::WA_InputMethodEnabled);

    m_completer->setFilterMode(Qt::MatchContains);//设置QCompleter支持匹配字符搜索
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);//这个属性可设置进行匹配时的大小写敏感性
    m_completer->setWrapAround(false);
    m_completer->installEventFilter(this);
    m_completer->setWidget(lineEdit());  //设置自动补全时弹出时相应位置的widget

    if (m_bIsChinese)
        m_completer->setCompletionRole(Qt::UserRole);//设置Pinyin参与搜索
    else
        m_completer->setCompletionRole(Qt::DisplayRole);

    connect(this, &DSearchEdit::textChanged, this, &SearchWidget::onSearchTextChange);

    connect(this, &DSearchEdit::returnPressed, this, &SearchWidget::onReturnPressed);

    //鼠标点击后直接页面跳转(存在同名信号)
    connect(m_completer, qOverload<const QString &>(&QCompleter::activated), this, &SearchWidget::onReturnPressed);
}

void SearchWidget::setModuleObject(ModuleObject *const module)
{
    m_rootModule = module;
    addUrl(module);
    refreshModel();
    configConnect(module);
}

void SearchWidget::refreshModel()
{
    for (auto &&data : m_searchData) {
        auto res = std::find_if(m_rootModule->childrens().cbegin(), m_rootModule->childrens().cend()
            , [data] (ModuleObject *child) ->bool {
            return child->moduleData()->DisplayName == data.Url.split('/').first();
        });

        if (res != m_rootModule->childrens().cend()) {
            QStandardItem *item1 = new QStandardItem;
            item1->setIcon((*res)->moduleData()->Icon);
            item1->setText(data.SearchUrl);
            item1->setData((*res)->moduleData()->Icon.name(), Qt::UserRole + 1);
            if (m_bIsChinese) {
                item1->setData(data.SearchUrl, Qt::UserRole);
            }
            m_model->appendRow(item1);
            if (m_bIsChinese) {// 中文环境添加拼音数据
                QStandardItem *item2 = new QStandardItem;
                item2->setIcon((*res)->moduleData()->Icon);
                item2->setText(data.SearchUrl);
                item2->setData(data.PinYin, Qt::UserRole);
                item2->setData((*res)->moduleData()->Icon.name(), Qt::UserRole + 1);
                m_model->appendRow(item2);
            }
        }
    }
}

void SearchWidget::addUrl(ModuleObject * module, const QString &prefix, QList<ModuleObject *> moduleUrl)
{
    QString tempStr;
    QStringList strlist;
    if (!prefix.isEmpty())
        strlist.append(prefix);
    if (!module->moduleData()->DisplayName.isEmpty())
        strlist.append(module->moduleData()->DisplayName);
    if (!strlist.isEmpty())
        tempStr = strlist.join('/');
    moduleUrl.append(module);
    if (module->hasChildrens()) {
        for (auto child : module->childrens()) {
            addUrl(child, tempStr, moduleUrl);
        }
        return;
    }
    if (!tempStr.isEmpty()) {
        auto &&data = SearchData(tempStr, convertUrl(tempStr), convertUrl(convertPinyin(tempStr)), moduleUrl);
        if (!m_searchData.contains(data))
            m_searchData.insert(m_insertIndex++, data);
    }
    for (auto &&text : module->moduleData()->ContentText) {
        tempStr = tempStr + '/' + text;
        auto &&data = SearchData(tempStr, convertUrl(tempStr), convertUrl(convertPinyin(tempStr)), moduleUrl);
        if (!m_searchData.contains(data))
            m_searchData.insert(m_insertIndex++, data);
    }
}

void SearchWidget::replaceUrl(ModuleObject *const module)
{
    int urlIndex = -1;
    QString prefix;
    for (auto &&data : m_searchData) {
        if (!data.ModuleUrl.contains(module))
            return;
        if (urlIndex < 0)
            urlIndex = m_searchData.indexOf(data);
        if (prefix.isEmpty()) {
            prefix = getPrefix(module, m_rootModule);
        }
        m_searchData.removeOne(data);
    }
    if (urlIndex >= 0 && !prefix.isEmpty()) {
        m_insertIndex = urlIndex;
        addUrl(module, prefix);
        refreshModel();
    }
}

QString SearchWidget::getPrefix(ModuleObject *const module, ModuleObject *const child, const QString &prefix)
{
    QString tempStr;
    QStringList strlist;
    if (!prefix.isEmpty())
        strlist.append(prefix);
    if (!module->moduleData()->DisplayName.isEmpty())
        strlist.append(module->moduleData()->DisplayName);
    if (!strlist.isEmpty())
        tempStr = strlist.join('/');
    for (auto ch : child->childrens()) {
        if (ch->findChild(module) > 0) {
            getPrefix(module, ch, tempStr);
        }
    }
    return tempStr;
}

void SearchWidget::configConnect(ModuleObject *const module)
{
    connect(module, &ModuleObject::moduleDataChanged, this, [this, module] {
        replaceUrl(module);
    });
    connect(module, &ModuleObject::childrenSizeChanged, this, [this, module] {
        replaceUrl(module);
    });
    for (auto child : module->childrens()) {
        configConnect(child);
    }
}

QString SearchWidget::convertUrl(const QString& url)
{
    QStringList sections = url.split('/');
    QString result = sections.takeAt(0);
    if (!sections.isEmpty()) {
        result += " --> ";
        result += sections.join(" / ");
    }
    return result;
}

QString SearchWidget::convertPinyin(const QString& url)
{
    const QStringList &sections = url.split('/');
    QStringList newSections;
    for (auto section : sections) {
        section = section.remove(QRegularExpression(R"([a-zA-Z]+)"));
        section = Dtk::Core::Chinese2Pinyin(section);
        // section = removeDigital(section);
        newSections.append(section);
    }
    return newSections.join('/');
}

QString SearchWidget::removeDigital(const QString &word)
{
    if (word.isEmpty())
        return word;

    QString value = "";
    char *data = word.toLocal8Bit().data();
    while (*data) {
        if (!(*data >= '0' && *data <= '9')) {
            value += *data;
        }
        data++;
    }

    return value;
}

void SearchWidget::onReturnPressed()
{
    if (!text().isEmpty()) {
        //enter defalt set first
        const QString &completion = m_completer->popup()->currentIndex().data().toString();
        auto res = std::find_if(m_searchData.cbegin(), m_searchData.cend(), [&completion] (const SearchData &data) ->bool {
            return data.SearchUrl == completion || data.PinYin == completion;
        });
        if (res != m_searchData.cend()) {
            blockSignals(true);
            setText(res->SearchUrl);
            blockSignals(false);
            Q_EMIT notifySearchUrl(res->Url);
        }
    }
}

void SearchWidget::onSearchTextChange(const QString &text)
{
    const QString &t = text.simplified();
    if (FilterText.contains(t))
        return;
    //发送该信号，用于解决外部直接setText的时候，搜索的图标不消失的问题
    Q_EMIT focusChanged(true);
    //实现自动补全
    onAutoComplete(t);
}

void SearchWidget::onAutoComplete(const QString &text)
{
    auto *widget = m_completer->popup();
    if (widget && text.isEmpty()) {
        widget->hide();
    } else {
        m_completer->setCompletionPrefix(text);
        m_completer->complete();
    }
}
