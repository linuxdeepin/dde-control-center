/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#include "searchwidget.h"
#include "searchmodel.h"
#include "window/gsettingwatcher.h"
#include "interface/moduleinterface.h"

#include <QCompleter>
#include <QPainter>
#include <QRect>
#include <QKeyEvent>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::search;
#define GSETTINGS_CONTENS_SERVER "iscontens-server"

class ddeCompleter : public QCompleter
{
public:
    ddeCompleter(QAbstractItemModel *model, QObject *parent = nullptr)
        : QCompleter(model, parent)
    {
    }

public:
    bool eventFilter(QObject *o, QEvent *e) override;
};

DCompleterStyledItemDelegate::DCompleterStyledItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

void DCompleterStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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

QSize DCompleterStyledItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize s = QStyledItemDelegate::sizeHint(option, index);
    s.setHeight(24);
    return s;
}

SearchWidget::SearchWidget(QWidget *parent)
    : DTK_WIDGET_NAMESPACE::DSearchEdit(parent)
{
    m_model = new SearchModel(this);
    m_completer = new ddeCompleter(m_model, this);
    m_completer->popup()->setItemDelegate(&styledItemDelegate);
    m_completer->popup()->setAttribute(Qt::WA_InputMethodEnabled);

    m_completer->setFilterMode(Qt::MatchContains);//设置QCompleter支持匹配字符搜索
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);//这个属性可设置进行匹配时的大小写敏感性
    m_completer->setCompletionRole(Qt::UserRole); //设置ItemDataRole
    m_completer->setWrapAround(false);
    m_completer->installEventFilter(this);
    m_completer->setWidget(lineEdit());  //设置自动补全时弹出时相应位置的widget

    m_forbidTextList << " " << " -" << " --" << " -->" << " --> "
         << "-" << "--" << "-->" << "--> "
         << "->" << "-> "
         << ">" << "> "
         << "/" << " /" << "/ " << " / ";

    connect(m_model, &SearchModel::notifyModuleSearch, this, &SearchWidget::notifyModuleSearch);

    connect(this, &DTK_WIDGET_NAMESPACE::DSearchEdit::textEdited, this, [ = ] {
        //m_bIstextEdited，　true : 用户输入　，　false : 直接调用setText
        //text(). ""　：　表示使用清除按钮删除数据，发送的信号；　非空　：　表示用户输入数据发送的信号
        if (text() != "") {
            m_model->m_bIstextEdited = true;
        } else {
            m_model->m_bIstextEdited = false;
        }
    });

    //语音输入不会触发DSearchEdit的textEdited信号，从而不会触发自动补全功能，需要根据textChanged信号手动触发下自动补全
    connect(this, &DTK_WIDGET_NAMESPACE::DSearchEdit::textChanged, this, &SearchWidget::onSearchTextChange);

    connect(this, &DTK_WIDGET_NAMESPACE::DSearchEdit::returnPressed, this, [ = ] {
        if (!text().isEmpty()) {
            //enter defalt set first
            if (!jumpContentPathWidget(text())) {
                //m_completer未关联部件时，currentCompletion只会获取到第一个选项并且不会在Edit中补全内容，需要通过popup()获取当前选择项并手动补全edit内容
                QString currentCompletion = m_completer->popup()->currentIndex().data().toString();
                //如果通过popup()未获取当前选择项,再通过currentCompletion获取第一个选项
                if (m_completer->completionCount() > 0 && currentCompletion.isEmpty()) {
                    currentCompletion = m_completer->currentCompletion();
                }
                qDebug() << Q_FUNC_INFO << " [SearchWidget] currentCompletion : " << currentCompletion;

                //若未获取到任何补全项直接退出,以免将已输入内容清空
                if (currentCompletion.isEmpty()) {
                    return ;
                }

                //中文遍历一遍,若没有匹配再遍历将拼音转化为中文再遍历
                //解决输入拼音时,有配置数据后,直接回车无法进入第一个匹配数据页面的问题
                if (!jumpContentPathWidget(currentCompletion)) {
                    jumpContentPathWidget(m_model->transPinyinToChinese(currentCompletion));
                }

                //根据匹配的信息补全DSearchEdit的内容，block信号避免重新触发自动补全
                this->blockSignals(true);
                this->setText(currentCompletion);
                this->blockSignals(false);
            }
        }
    });

    //鼠标点击后直接页面跳转(存在同名信号)
    connect(m_completer, SIGNAL(activated(QString)), this, SLOT(onCompleterActivated(QString)));
}

SearchWidget::~SearchWidget()
{

}

void SearchWidget::onCompleterActivated(const QString &value)
{
    qDebug() << Q_FUNC_INFO << value;
    Q_EMIT returnPressed();
}

void SearchWidget::onAutoComplete(const QString &text)
{
    //因为QLineEdit只有在触发textEdited信号时才会触发自动补全，而语音识别输入没有触发textEdited,所以需要在触发textChanged后手动触发下自动补全
    auto *widget = m_completer->popup();
    if (widget && text.isEmpty()) {
        widget->hide();
    } else {
        m_completer->setCompletionPrefix(text);
        m_completer->complete();
    }
}

void SearchWidget::onSearchTextChange(const QString &text)
{
    if (m_forbidTextList.contains(text)) {
        return;
    }

    //发送该信号，用于解决外部直接setText的时候，搜索的图标不消失的问题
    Q_EMIT focusChanged(true);
    //实现自动补全
    onAutoComplete(text);
}

bool ddeCompleter::eventFilter(QObject *o, QEvent *e)
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

bool SearchWidget::jumpContentPathWidget(const QString &path)
{
    return m_model->jumpContentPathWidget(path);
}

void SearchWidget::getJumpPath(QString &moduleName, QString &pageName, const QString &searchName)
{
    m_model->getJumpPath(moduleName,pageName,searchName);
}

void SearchWidget::setLanguage(const QString &type)
{
    if (type == "zh_CN" || type == "zh_HK" || type == "zh_TW") {
        m_completer->setCompletionRole(Qt::UserRole); //设置ItemDataRole
    } else {
        m_completer->setCompletionRole(Qt::DisplayRole);
    }

    return m_model->setLanguage(type);
}

void SearchWidget::addModulesName(QString moduleName, const QString &searchName, QIcon icon, QString translation)
{
    return m_model->addModulesName(moduleName, searchName, icon, translation);
}

void SearchWidget::addSpecialThreeMenuMap(const QString &name, bool flag)
{
    return m_model->addSpecialThreeMenuMap(name, flag);
}

void SearchWidget::setModuleVisible(const QString &module, bool visible)
{
    if (!m_model) {
        return;
    }

    m_model->setModuleVisible(module, visible);
}

void SearchWidget::setWidgetVisible(const QString &module, const QString &widget, bool visible)
{
    if (!m_model) {
        return;
    }

    m_model->setWidgetVisible(module, widget, visible);
}

void SearchWidget::setDetailVisible(const QString &module, const QString &widget, const QString &detail, bool visible)
{
    if (!m_model) {
        return;
    }

    m_model->setDetailVisible(module, widget, detail, visible);
}

void SearchWidget::updateSearchdata(const QString &module)
{
    if (!m_model) {
        return;
    }

    m_model->updateSearchData(module);
}

//返回搜索结果
QList<QString> SearchWidget::searchResults(const QString text)
{
    QElapsedTimer et;
    et.start();
    while (!m_model->getDataUpdateCompleted() && et.elapsed() < 5000) {
        QThread::msleep(50);
        QCoreApplication::processEvents();
    }
    QList<QString> lstSearchMsgs;
    for (int row = 0; row < m_model->rowCount(); row++) {
        QString msg = m_model->data(m_model->index(row, 0), m_model->m_bIsChinese? Qt::UserRole: Qt::DisplayRole).toString();
        if(msg.contains(text,Qt::CaseInsensitive))
            lstSearchMsgs.append(msg);
    }
    for (int i = 0; i < lstSearchMsgs.size(); i++) {
        lstSearchMsgs[i] = m_model->transPinyinToChinese(lstSearchMsgs[i]);
    }
    return lstSearchMsgs;
}
void SearchWidget::addChildPageTrans(const QString &menu, const QString &tran)
{
    if (!m_model) {
        return;
    }

    m_model->addChildPageTrans(menu, tran);
}
