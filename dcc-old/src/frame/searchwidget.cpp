//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "searchwidget.h"
#include "completerview.h"
#include "interface/moduleobject.h"

#include <DPinyin>
#include <DSizeMode>
#include <DFontSizeManager>

#include <QKeyEvent>
#include <QAbstractItemView>
#include <QPainter>
#include <QStandardItemModel>
#include <QToolTip>
#include <QDebug>

#if DTK_VERSION >= DTK_VERSION_CHECK(5, 6, 0, 0)
#    define USE_DCIICON
#endif

#ifdef USE_DCIICON
#    include <DDciIcon>
#    include <DGuiApplicationHelper>
#    include <DIconTheme>
DGUI_USE_NAMESPACE
#endif

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

const QStringList &FilterText{ "-", "--", "-->", "->", ">", "/" };
enum CompleterRole {
    UrlRole = Qt::UserRole + 1,
    SearchRole,
    DisplayNameRole,
    ModuleRole,
};

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
            ? QPalette::Normal
            : QPalette::Disabled;
    if (cg == QPalette::Normal && !(option.state & QStyle::State_Active)) {
        cg = QPalette::Inactive;
    }

    if (option.showDecorationSelected && (option.state & (QStyle::State_Selected | QStyle::State_MouseOver))) {
        painter->fillRect(option.rect, option.palette.brush(cg, QPalette::Highlight));
    }

    ModuleObject *p = index.data(CompleterRole::ModuleRole).value<ModuleObject *>();
    if (p->getParent()) {
        while (p->getParent()->getParent()) {
            p = p->getParent();
        }
    }
    QVariant iconVar = p->icon();
    QRect iconRect = calculateIconRect(option.rect);
#ifdef USE_DCIICON
    DDciIcon dciIcon;
    if (iconVar.canConvert<DDciIcon>()) {
        dciIcon = iconVar.value<DDciIcon>();
    } else if (iconVar.type() == QVariant::String) {
        QString iconstr = iconVar.toString();
        if (!iconstr.isEmpty()) {
            dciIcon = DDciIcon::fromTheme(iconstr);
            if (dciIcon.isNull())
                dciIcon = DDciIcon(iconstr);
        }
    }
    if (!dciIcon.isNull()) {
        DDciIcon::Mode dciMode = DDciIcon::Normal;
        if (option.state & QStyle::State_Enabled) {
            if (option.state & (QStyle::State_Sunken | QStyle::State_Selected)) {
                dciMode = DDciIcon::Pressed;
            } else if (option.state & QStyle::State_MouseOver) {
                dciMode = DDciIcon::Hover;
            }
        } else {
            dciMode = DDciIcon::Disabled;
        }

        DDciIcon::Theme theme = DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType ? DDciIcon::Dark : DDciIcon::Light;

        painter->save();
        painter->setBrush(Qt::NoBrush);
        dciIcon.paint(painter, iconRect, painter->device() ? painter->device()->devicePixelRatioF() : qApp->devicePixelRatio(), theme, dciMode, option.decorationAlignment);
        painter->restore();
        iconVar.clear();
    }
#endif
    if (iconVar.isValid()) {
        QIcon icon;
        if (iconVar.type() == QVariant::Icon) {
            icon = iconVar.value<QIcon>();
        } else if (iconVar.type() == QVariant::String) {
            const QString &iconstr = iconVar.toString();
            icon = DIconTheme::findQIcon(iconstr);
            if (icon.isNull())
                icon = QIcon(iconstr);
        }
        if (!icon.isNull()) {
            QIcon::Mode mode = QIcon::Normal;
            if (!(option.state & QStyle::State_Enabled))
                mode = QIcon::Disabled;
            else if (option.state & QStyle::State_Selected)
                mode = QIcon::Selected;

            QIcon::State state = (option.state & QStyle::State_Open) ? QIcon::On : QIcon::Off;
            icon.paint(painter, iconRect, option.decorationAlignment, mode, state);
        }
    }
    // draw text
    QRect textRect = calculateTextRect(option.rect);
    auto font = option.font;
    font.setPixelSize(DFontSizeManager::instance()->fontPixelSize(DFontSizeManager::T6));
    const QFontMetrics fontMetrics(font);
    QString newText = fontMetrics.elidedText(index.data(Qt::DisplayRole).toString(), Qt::ElideRight, textRect.width());

    if (option.state & (QStyle::State_Selected | QStyle::State_MouseOver)) {
        painter->setPen(option.palette.color(cg, QPalette::HighlightedText));
    } else {
        painter->setPen(option.palette.color(cg, QPalette::Text));
    }
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignVCenter, newText);
}

QSize DccCompleterStyledItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize s = QStyledItemDelegate::sizeHint(option, index);
    s.setHeight(DSizeModeHelper::element(24, 36));
    return s;
}

bool DccCompleterStyledItemDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    switch (event->type()) {
    case QEvent::WhatsThis:
    case QEvent::ToolTip: {
        QString text = index.data(Qt::DisplayRole).toString();
        auto font = option.font;
        font.setPixelSize(DFontSizeManager::instance()->fontPixelSize(DFontSizeManager::T6));
        const QFontMetrics fontMetrics(font);
        if (fontMetrics.size(0, text).width() <= calculateTextRect(option.rect).width()) {
            QToolTip::showText(event->globalPos(), QString());
            event->setAccepted(true);
            return true;
        }
    } break;
    default:
        break;
    }
    return QStyledItemDelegate::helpEvent(event, view, option, index);
}

QRect DccCompleterStyledItemDelegate::calculateIconRect(const QRect &rect) const
{
    return QRect(8, rect.y() + (rect.height() - 24) / 2, 24, 24);
}

QRect DccCompleterStyledItemDelegate::calculateTextRect(const QRect &rect) const
{
    return rect.adjusted(calculateIconRect(rect).right() + 8, 0, -8, 0);
}

SearchWidget::SearchWidget(QWidget *parent)
    : DSearchEdit(parent)
    , m_model(new QStandardItemModel(this))
    , m_completer(new DccCompleter(m_model, this))
    , m_completerView(new CompleterView(this))
    , m_bIsChinese(false)
{
    const QString &language = QLocale::system().name();
    m_bIsChinese = language == "zh_CN" || language == "zh_HK" || language == "zh_TW";

    DccCompleterStyledItemDelegate *delegate = new DccCompleterStyledItemDelegate(m_completer);
    m_completer->setPopup(m_completerView);
    m_completer->popup()->setItemDelegate(delegate);
    m_completer->popup()->setAttribute(Qt::WA_InputMethodEnabled);

    m_completer->setFilterMode(Qt::MatchContains);        //设置QCompleter支持匹配字符搜索
    m_completer->setCaseSensitivity(Qt::CaseInsensitive); //这个属性可设置进行匹配时的大小写敏感性
    m_completer->setWrapAround(false);
    m_completer->installEventFilter(this);
    m_completer->setWidget(lineEdit()); //设置自动补全时弹出时相应位置的widget

    m_completer->setCompletionRole(CompleterRole::SearchRole);

    connect(this, &DSearchEdit::textChanged, this, &SearchWidget::onSearchTextChange);

    connect(this, &DSearchEdit::returnPressed, this, &SearchWidget::onReturnPressed);

    //鼠标点击后直接页面跳转(存在同名信号)
    connect(m_completer, qOverload<const QString &>(&QCompleter::activated), this, &SearchWidget::onReturnPressed);
}

void SearchWidget::setModuleObject(ModuleObject *const module)
{
    m_rootModule = module;
}

QList<QPair<QString, QString>> SearchWidget::searchResults(const QString text)
{
    QList<QPair<QString, QString>> result;
    m_completer->setCompletionPrefix(text);
    QAbstractItemModel *model = m_completer->completionModel();
    for (int i = 0; i < model->rowCount(); i++) {
        const QModelIndex &index = model->index(i, 0);
        result.append({ index.data(CompleterRole::UrlRole).toString(), index.data().toString() });
    }
    return result;
}

void SearchWidget::addModule(ModuleObject *const module)
{
    if (ModuleObject::IsHidden(module) || module->noSearch() || (module->displayName().isEmpty() && module->contentText().isEmpty()))
        return;

    QList<ModuleObject *> moduleurl;
    QStringList urls;
    QStringList displayNames;
    ModuleObject *p = module;
    while (p->getParent()) {
        if (ModuleObject::IsHidden(p))
            return;
        moduleurl.prepend(p);
        urls.prepend(p->name());
        const QString &&displayName = p->displayName();
        if (!displayName.isEmpty()) {
            displayNames.prepend(displayName);
        }
        p = p->getParent();
    }

    QString text = convertUrl(displayNames);
    if (m_allText.contains(text)) {
        return;
    }
    m_allText.insert(text);
    QStandardItem *item = new QStandardItem;
    item->setText(text);
    item->setToolTip(text);
    item->setData(urls.join("/"), CompleterRole::UrlRole);
    item->setData(QVariant::fromValue((ModuleObject *)(module)), CompleterRole::ModuleRole);
    QString searchStr = module->displayName();
    item->setData(searchStr, CompleterRole::DisplayNameRole);

    searchStr.remove(' ');
    QStringList searchList(searchStr);
    if (m_bIsChinese) {
        searchStr.remove(QRegularExpression(R"([a-zA-Z\d]+)"));
        QStringList displaynameList = Dtk::Core::Chinese2Pinyin(searchStr).split(QRegularExpression(R"(\d+)"));
        if (!displaynameList.isEmpty()) {
            searchList << displaynameList.join(QString());
            QString initial;
            for (auto &&str : displaynameList) {
                if (!str.isEmpty())
                    initial.append(str.at(0));
            }
            searchList << initial;
        }
    }
    item->setData(searchList.join("\n"), CompleterRole::SearchRole);
    m_model->appendRow(item);
}

void SearchWidget::removeModule(ModuleObject *const module)
{
    for (int i = 0; i < m_model->rowCount(); i++) {
        if (m_model->index(i, 0).data(CompleterRole::ModuleRole).value<ModuleObject *>() == module) {
            m_allText.remove(m_model->index(i, 0).data().toString());
            m_model->removeRow(i);
            break;
        }
    }
}

QString SearchWidget::convertUrl(const QStringList &displayNames)
{
    return displayNames.join(" / ");
}

void SearchWidget::onReturnPressed()
{
    if (!text().isEmpty()) {
        // enter defalt set first
        const QString &url = m_completer->popup()->currentIndex().data(CompleterRole::UrlRole).toString();
        if (!url.isEmpty()) {
            blockSignals(true);
            setText(m_completer->popup()->currentIndex().data(CompleterRole::DisplayNameRole).toString());
            blockSignals(false);
            Q_EMIT notifySearchUrl(url);
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

    m_completerView->resize(m_completerView->width(), m_completerView->height() + m_completerView->margin());
    // 当搜索popup弹窗在搜索框上方时，因为resize导致popup的高度增加了margin()，所以需要上移margin()，否则会遮盖搜索框，再增加6x与搜索框间隔
    if (this->mapToGlobal(this->geometry().topLeft()).y() > m_completerView->geometry().y()) {
        m_completerView->move(m_completerView->x(), m_completerView->y() - m_completerView->margin() - 6);
    }
}

void SearchWidget::onAutoComplete(const QString &text)
{
    auto *widget = m_completer->popup();
    if (widget && text.isEmpty()) {
        widget->hide();
    } else {
        QString str = text;
        str.remove(' ');
        m_completer->setCompletionPrefix(str);
        m_completer->complete(rect().adjusted(0, 0, 0, 8));
    }
}
