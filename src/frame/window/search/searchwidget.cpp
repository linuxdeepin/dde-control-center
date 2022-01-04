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
#include "window/utils.h"
#include "window/gsettingwatcher.h"
#include "interface/moduleinterface.h"

#include <DPinyin>
#include <QDebug>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QXmlStreamReader>
#include <QCompleter>
#include <QRegularExpression>
#include <QPainter>
#include <QRect>
#include <QApplication>
#include <QKeyEvent>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QAbstractItemView>

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

SearchModel::SearchModel(QObject *parent)
    : QStandardItemModel(parent)
    , m_bIsChinese(false)
    , m_bIstextEdited(false)
    , m_bIsContensServerType(false)
{
    //是否是服务器判断,这个判断与下面可移除设备不同,只能"是"或者"不是"(不是插拔型)
    m_bIsServerType = IsServerSystem;

    //是否是contens服务器
    if (DSysInfo::uosEditionType() == DSysInfo::UosEuler || DSysInfo::uosEditionType() == DSysInfo::UosEnterpriseC) {
        m_bIsContensServerType = true;
    } else {

        m_bIsContensServerType = false;
    }

    //first存储和服务器/桌面版有关的文言
    //second : true 用于记录"服务器"才有的搜索数据
    //second : false用于记录"桌面版"才有的搜索数据
    m_serverTxtList = {
        {tr("Window Effect"), false},
        {tr("Developer Mode"), false},
        {tr("User Experience Program"), false},
        {tr("Join User Experience Program"), false},
        {tr("Display Scaling"), false},
        {tr("Night Shift"), false},
        {tr("Login Without Password"), false},
        {tr("Auto Brightness"), false},
        {tr("General"), false},
        {tr("Password is required to wake up the monitor"), false},
        {tr("Password is required to wake up the computer"), false},
        {tr("Power Saving Mode"), false},
        {tr("Auto Mode Switch"), false},
        {tr("Transparency"), false},
        {tr("Create PPPoE Connection"), false},
        {tr("Disable the touchpad while typing"), false},
        {tr("Disable the touchpad when inserting the mouse"), false},
        {tr("Computer will suspend after"), false},
        {tr("Sign In"), false},
        {tr("Auto power saving on battery"), false},
        {tr("Touch Screen"), false},
        {tr("Power Plans"), false},
        {tr("Power Saving Settings"), false},
        {tr("Wakeup Settings"), false},
        {tr("Battery"), false},
    };

    m_contensServerTxtList = {
        {tr("Developer Mode"), true},
    };

    m_contensServerTxtList = {
        {tr("Developer Mode"), true},
    };

    //first : 可移除设备名称
    //second : 可以除设备具体的页面名称(该页面必须与搜索的页面对应)
    //通过在 loadXml() 301行，使用 “qDebug() << searchBoxStruct.fullPagePath.section('/', 2, -1);”解析
    m_removedefaultWidgetList = {
        {tr("Touchpad"), "Touchpad"},
        {tr("TrackPoint"), "TrackPoint"},
        {tr("Personal Hotspot"), "Personal Hotspot"},
        {tr("On Battery"), "On Battery"},
        {tr("Wired Network"), "Wired Network"},
        {tr("Wireless Network"), "WirelessPage"},
        {tr("Multiple Displays"), "Multiple Displays"},
        {tr("Boot Menu"), "Boot Menu"},
        {tr("When the lid is closed"), "When the lid is closed"},
        {tr("Display remaining using and charging time"), "Display remaining using and charging time"},
        {tr("Maximum capacity"), "Maximum capacity"},
        {tr("Auto power saving on battery"), "Auto power saving on battery"},
    };

    //用于区分可移除设备数据，和常驻设备数据（记录页面信息）
    for (auto data : m_removedefaultWidgetList) {
        m_defaultRemoveableList << data.second;
    }
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

int SearchModel::getDataNum(QString source, char value)
{
    char *data = source.toLatin1().data();
    int retNum = 0;
    int place = 0;
    while (data[place]) {
        if (data[place] == value)
            retNum++;
        place++;
    }
    return retNum;
}

void SearchModel::getJumpPath(QString &moduleName, QString &pageName, const QString &searchName)
{
    if (m_EnterNewPagelist.count() > 0) {
        SearchBoxStruct::Ptr data = getModuleBtnString(searchName);
        if (data->translateContent != "" && data->fullPagePath != "") {
            for (int i = 0; i < m_EnterNewPagelist.count(); i++) {
                if (m_EnterNewPagelist[i]->translateContent == data->fullPagePath
                        && m_EnterNewPagelist[i]->childPageName.isEmpty()
                        && m_EnterNewPagelist[i]->actualModuleName == data->translateContent) {
                    moduleName = data->actualModuleName;
                    pageName = m_EnterNewPagelist[i]->fullPagePath.section('/', 2, -1);
                    break;
                }
            }
        }
    }
}

bool SearchModel::jumpContentPathWidget(const QString &path)
{
    qDebug() << Q_FUNC_INFO << path;

    //网络详情 / 接口
    QString searchDetailData = path.section('>', 1, -1).remove('>').trimmed();

    //无法解析出来的数据表示为不满足标准格式的搜索数据
    if (searchDetailData == "")
        return false;

    //其他语言的模块内子页面信息 : 网络详情
    QString searchChildWidget = searchDetailData.section('/', 0, 0).remove('/').trimmed();

    //解析出：接口
    QString searchEndData = searchDetailData.section('/', 1, -1).remove('/').trimmed();
    if (m_bIsChinese) {
        searchEndData = searchEndData.trimmed();
    }

    //searchEndData为空表示没有子页面, 需要往下继续执行
    bool isContinue = false;

    //searchEndData为空表示没有子页面或直接输入的子页面名称的搜索, 需要往下继续执行
    if (searchEndData == "") {
        //TODO：需要存储子页面的列表，如果searchChildWidget在列表中，而searchEndData为空也不能继续执行
        //否则已经搜索模块，通过回退就直接可以进搜索页面，不能在搜索框选其他搜索项
        if (!m_childWidgetList.contains(searchChildWidget) || m_childeHideWidgetList.contains(searchChildWidget)) {
            isContinue = true;
        }
    } else {
        //searchEndData不为空，且搜索数据包含在m_TxtListAll可以执行
        //对非xml添加的搜索数据内容不作处理, 不在list里面就停止
        if (m_TxtListAll.contains(searchEndData))
            isContinue = true;
    }

    if (!isContinue)
        return false;

    //其他语言的模块数据 : 网络
    QString searchModule = path.section('-', 0, 1).remove('-').trimmed();

    for (int i = 0; i < m_EnterNewPagelist.count(); i++) {
        //SearchBoxStruct::Ptr data = m_EnterNewPagelist[i]; //used to debug
        /* m_EnterNewPagelist[i] 数据例子
           source : Interface
           translateContent : 接口
           actualModuleName : 网络
           childPageName : 网络详情
           fullPagePath : /network/Network Details
        */
        QString childPageName = m_EnterNewPagelist[i]->childPageName;
        QString fullPagePath = m_EnterNewPagelist[i]->fullPagePath;

        //用于区分类似 "默认程序 -> 网页 / 添加默认程序" 和 "默认程序 -> 网页"
        if (childPageName == "" && searchChildWidget != searchDetailData) {
            continue;
        }

        //搜索数据需要匹配的数据：模块，子页面，详细数据
        if (m_EnterNewPagelist[i]->actualModuleName == searchModule
                //匹配子页面和详细数据
                && ((childPageName == searchChildWidget && m_EnterNewPagelist[i]->translateContent == searchEndData)
                    //子页面为空，详细数据要相等
                    || (childPageName == "" && m_EnterNewPagelist[i]->translateContent == searchChildWidget)
                    )
                ) {
            //module必须有，childPageName可以为空
            QString module = fullPagePath.section('/', 1, 1);
            QString childWidget = "";

            if (getDataNum(fullPagePath, '/') == 3) {
                //在第3级子页面再打开第4级页面
                childWidget = fullPagePath.section('/', 2, -1);
                qInfo() << " [Search] childWidgetCount : 3 , childWidget :" << childWidget;
            } else {
                //插件的m_EnterNewPagelist[i]->childPageName为空
                if (childPageName == "" || childPageName == searchChildWidget) {
                    childWidget = fullPagePath.section('/', 2, -1);
                }

                if (childWidget == "") {
                    childWidget = m_EnterNewPagelist[i]->source;
                }
            }
            qInfo() << " search result > module : " << module << " , widget : " << childWidget;
            Q_EMIT notifyModuleSearch(module, childWidget);
            return true;
        }
    }

    return false;
}

void SearchModel::loadxml(const QString module)
{
    Q_UNUSED(module)

    clear(); // It doesn't seem to leak memory
    m_EnterNewPagelist.clear();
    m_inputList.clear();
    m_TxtListAll.clear();

    //添加一项空数据，为了防止使用setText输入错误数据时直接跳转到list中正确的第一个页面
    m_EnterNewPagelist.append(std::make_shared<SearchBoxStruct>());
    m_inputList.append(SearchDataStruct());
    appendRow(new QStandardItem(""));

    bool bIsContinue = false;
    bool bIsTwoLevel = false;

    for (SearchBoxStruct::Ptr searchBoxStrcut : m_originList) {
        bool moduleVisible = getModuleVisible(searchBoxStrcut->actualModuleName);
        if (!moduleVisible) {
            continue;
        }

        bIsContinue = true;
        bIsTwoLevel = false;
        do {
            if (searchBoxStrcut->childPageName == "") {
                //这样的数据都是只有二级菜单，需要用到source再进行判断，且source需要进行多语言翻译
                searchBoxStrcut->childPageName = m_transChildPageName.value(searchBoxStrcut->source);
                bIsTwoLevel = true;
            }

            bool widgetVisible = getWidgetVisible(searchBoxStrcut->actualModuleName, searchBoxStrcut->childPageName);

            if (!widgetVisible) {
                bIsContinue = false;
                if (bIsTwoLevel && searchBoxStrcut->childPageName != "")
                    searchBoxStrcut->childPageName = "";
                break;
            }

            if (bIsTwoLevel) {
                //数据用完后需要还原, 此时表示只有两级目录需要退出判断
                searchBoxStrcut->childPageName = "";
                bIsContinue = true;
                break;
            }

            bool detailVisible = getDetailVisible(searchBoxStrcut->actualModuleName, searchBoxStrcut->childPageName, searchBoxStrcut->translateContent);
            if (!detailVisible) {
                bIsContinue = false;
                break;
            }

        } while (0);

        if (!bIsContinue) {
            continue;
        }

        m_EnterNewPagelist.append(searchBoxStrcut);

        m_TxtListAll.append(searchBoxStrcut->translateContent);

        // Add search result content
        if (!m_bIsChinese) {
            auto icon = m_iconMap.find(searchBoxStrcut->fullPagePath.section('/', 1, 1));
            if (icon == m_iconMap.end()) {
                continue;
            }

            if ("" == searchBoxStrcut->childPageName) {
                appendRow(new QStandardItem(icon.value(), QString("%1 --> %2").arg(searchBoxStrcut->actualModuleName).arg(searchBoxStrcut->translateContent)));
            }
            else {
                appendRow(new QStandardItem(
                    icon.value(), QString("%1 --> %2 / %3").arg(searchBoxStrcut->actualModuleName).arg(searchBoxStrcut->childPageName).arg(searchBoxStrcut->translateContent)));
            }
        }
        else {
            appendChineseData(searchBoxStrcut);
        }

        m_TxtListAll.append(searchBoxStrcut->translateContent.remove('/').trimmed());
    }
}

//Follow display content to Analysis SearchBoxStruct data
SearchBoxStruct::Ptr SearchModel::getModuleBtnString(QString value)
{
    SearchBoxStruct::Ptr data = std::make_shared<SearchBoxStruct>();

    data->translateContent = value.section('-', 0, 1).remove('-').trimmed();
    //follow actual module name to get path module name
    data->actualModuleName = getModulesName(data->translateContent, false);
    data->fullPagePath = value.section('>', 1, -1).remove('>').trimmed();

    if (data->fullPagePath.contains('/', Qt::CaseInsensitive)) {
        QString strTemp = data->fullPagePath.section('/', 0, 0).remove('/').trimmed();
        //修复最终字段存在'/'无法跳转的问题
        if (this->m_TxtListAll.contains(strTemp)) {
            data->fullPagePath = strTemp;
        }
    }

#if DEBUG_XML_SWITCH
    qDebug() << Q_FUNC_INFO << " [SearchWidget] data.translateContent : " << data.translateContent << "   ,  data.fullPagePath : " << data.fullPagePath;
#endif

    return data;
}

//tranlate the path name to tr("name")
QString SearchModel::getModulesName(const QString &name, bool state)
{
    QString strResult = "";

    for (auto it : m_moduleNameList) {
        if (state) { //true : follow first search second (use pathName translate to actual moduleName)
            if (it.first == name) {
                strResult = it.second;
                break;
            }
        } else { //false : follow second search first (use actual moduleName translate to pathName)
            if (it.second == name) {
                strResult = it.first;
                break;
            }
        }
    }

    return strResult;
}

QString SearchModel::removeDigital(QString input)
{
    if ("" == input)
        return "";

    QString value = "";
    QByteArray ba = input.toLocal8Bit();
    char *data = nullptr;
    data = ba.data();
    while (*data) {
        if (!(*data >= '0' && *data <= '9')) {
            value += *data;
        }
        data++;
    }

    return value;
}

QString SearchModel::transPinyinToChinese(const QString &pinyin)
{
    QString value = pinyin;

    //遍历"汉字-拼音"列表,将存在的"拼音"转换为"汉字"
    auto res = std::find_if(m_inputList.begin(), m_inputList.end(), [=] (const SearchDataStruct data)->bool{
            return value == data.pinyin;
        });

        if (res != m_inputList.end()) {
            value =(*res).chiese;
        }

    return value;
}

QString SearchModel::containTxtData(QString txt)
{
    QString value = txt;

    //遍历"汉字-拼音"列表,将存在的"拼音"转换为"汉字"
    auto res = std::find_if(m_inputList.begin(), m_inputList.end(), [=] (const SearchDataStruct data)->bool{
                return (data.chiese.contains(txt, Qt::CaseInsensitive) ||
                        data.pinyin.contains(txt, Qt::CaseInsensitive));
            });

            if (res != m_inputList.end()) {
                value = (*res).chiese;
            }

    return value;
}

void SearchModel::appendChineseData(SearchBoxStruct::Ptr data)
{
    auto icon = m_iconMap.find(data->fullPagePath.section('/', 1, 1));
    if (icon == m_iconMap.end()) {
        return;
    }

    // 其他函数存在修改智能指针的数值，复制一份解决。
    SearchBoxStruct::Ptr dataBackup(new SearchBoxStruct(*data));

    if ("" == dataBackup->childPageName) {
        //先添加使用appenRow添加Qt::EditRole数据(用于下拉框显示),然后添加Qt::UserRole数据(用于输入框搜索)
        //Qt::EditRole数据用于显示搜索到的结果(汉字)
        //Qt::UserRole数据用于输入框输入的数据(拼音/汉字 均可)
        //即在输入框搜索Qt::UserRole的数据,就会在下拉框显示Qt::EditRole的数据
        appendRow(new QStandardItem(icon.value(),
                                    QString("%1 --> %2").arg(dataBackup->actualModuleName).arg(dataBackup->translateContent)));

        //设置汉字的Qt::UserRole数据
        setData(index(rowCount() - 1, 0),
                         QString("%1 --> %2")
                         .arg(dataBackup->actualModuleName)
                         .arg(dataBackup->translateContent),
                         Qt::UserRole);
        setData(index(rowCount() - 1, 0), icon->name(), Qt::UserRole + 1);

        QString hanziTxt = QString("%1 --> %2").arg(dataBackup->actualModuleName).arg(dataBackup->translateContent);

        QString pinyinTxt = QString("%1 --> %2")
                            .arg(removeDigital(DTK_CORE_NAMESPACE::Chinese2Pinyin(dataBackup->actualModuleName.remove(QRegularExpression(R"([a-zA-Z]+)")))))
                            .arg(removeDigital(DTK_CORE_NAMESPACE::Chinese2Pinyin(dataBackup->translateContent.remove(QRegularExpression(R"([a-zA-Z]+)")))));

        // 如果模块名称中英文相同则不继续添加拼音搜索显示,否则会重复索引
        if (dataBackup->actualModuleName == DTK_CORE_NAMESPACE::Chinese2Pinyin(dataBackup->actualModuleName)) return;

        //添加显示的汉字(用于拼音搜索显示)
        appendRow(new QStandardItem(icon.value(), hanziTxt));
        //设置Qt::UserRole搜索的拼音(即搜索拼音会显示上面的汉字)
        setData(index(rowCount() - 1, 0), pinyinTxt, Qt::UserRole);
        setData(index(rowCount() - 1, 0), icon->name(), Qt::UserRole + 1);
        SearchDataStruct transdata;
        transdata.chiese = hanziTxt;
        transdata.pinyin = pinyinTxt;
        //存储 汉字和拼音 : 在选择对应的下拉框数据后,会将Qt::UserRole数据设置到输入框(即pinyin)
        //而在输入框发送 DSearchEdit::textChanged 信号时,会遍历m_inputList,根据pinyin获取到对应汉字,再将汉字设置到输入框
        m_inputList.append(transdata);
    } else {
        //先添加使用appenRow添加Qt::EditRole数据(用于下拉框显示),然后添加Qt::UserRole数据(用于输入框搜索)
        //Qt::EditRole数据用于显示搜索到的结果(汉字)
        //Qt::UserRole数据用于输入框输入的数据(拼音/汉字 均可)
        //即在输入框搜索Qt::UserRole的数据,就会在下拉框显示Qt::EditRole的数据
        appendRow(new QStandardItem(icon.value(),
                                    QString("%1 --> %2 / %3").arg(dataBackup->actualModuleName).arg(dataBackup->childPageName).arg(dataBackup->translateContent)));

        //设置汉字的Qt::UserRole数据
        setData(index(rowCount() - 1, 0),
                         QString("%1 --> %2 / %3")
                         .arg(dataBackup->actualModuleName)
                         .arg(dataBackup->childPageName)
                         .arg(dataBackup->translateContent),
                         Qt::UserRole);
        setData(index(rowCount() - 1, 0), icon->name(), Qt::UserRole + 1);

        QString hanziTxt = QString("%1 --> %2 / %3").arg(dataBackup->actualModuleName).arg(dataBackup->childPageName).arg(dataBackup->translateContent);
        QString pinyinTxt = QString("%1 --> %2 / %3")
                            .arg(removeDigital(DTK_CORE_NAMESPACE::Chinese2Pinyin(dataBackup->actualModuleName.remove(QRegularExpression(R"([a-zA-Z]+)")))))
                            .arg(removeDigital(DTK_CORE_NAMESPACE::Chinese2Pinyin(dataBackup->childPageName.remove(QRegularExpression(R"([a-zA-Z]+)")))))
                            .arg(removeDigital(DTK_CORE_NAMESPACE::Chinese2Pinyin(dataBackup->translateContent.remove(QRegularExpression(R"([a-zA-Z]+)")))));
        //添加显示的汉字(用于拼音搜索显示)
        auto icons = m_iconMap.find(dataBackup->fullPagePath.section('/', 1, 1));
        if (icons == m_iconMap.end()) {
            return;
        }
        appendRow(new QStandardItem(icons.value(), hanziTxt));
        //设置Qt::UserRole搜索的拼音(即搜索拼音会显示上面的汉字)
        setData(index(rowCount() - 1, 0), pinyinTxt, Qt::UserRole);
        setData(index(rowCount() - 1, 0), icons->name(), Qt::UserRole + 1);
        SearchDataStruct transdata;
        transdata.chiese = hanziTxt;
        transdata.pinyin = pinyinTxt;
        //存储 汉字和拼音 : 在选择对应的下拉框数据后,会将Qt::UserRole数据设置到输入框(即pinyin)
        //而在输入框发送 DSearchEdit::textChanged 信号时,会遍历m_inputList,根据pinyin获取到对应汉字,再将汉字设置到输入框
        m_inputList.append(transdata);
    }
}

//返回值:true,不加载该搜索数据
bool SearchModel::isLoadText(const QString &txt)
{
    for (auto data : m_serverTxtList) {
        //有first数据继续判断second
        if (data.first == txt) {
            //second: true,需要是服务器才显示
            //second:false,需要不是服务器才显示
            //m_bIsServerType当前是否为服务器,true是服务器(此处要取反,需要根据返回值判断)
            if (data.second == !m_bIsServerType) {
                return true;
            } else {
                break;
            }
        }
    }

    return false;
}

bool SearchModel::isLoadContensText(const QString &text)
{
    for (auto data : m_contensServerTxtList) {
        //有first数据继续判断second
        if (data.first == text) {
            //second: true,需要是contens服务器不显示
            //m_bIsContensServerType当前是否为服务器,true是contens服务器(此处要取反,需要根据返回值判断)
            if (data.second == m_bIsContensServerType) {
                return true;
            } else {
                break;
            }
        }
    }

    return false;
}

void SearchModel::setLanguage(const QString &type)
{
    m_lang = type;

    if (type == "zh_CN" || type == "zh_HK" || type == "zh_TW") {
        m_bIsChinese = true;
    }

    QFutureWatcher<QList<SearchBoxStruct::Ptr>>* watcher = new QFutureWatcher<QList<SearchBoxStruct::Ptr>>();
    connect(watcher, &QFutureWatcher<QList<SearchBoxStruct::Ptr>>::finished, this, [=] {
        m_originList = watcher->result();
        watcher->deleteLater();
        return loadxml();
    });

    m_childeHideWidgetList.clear();

    watcher->setFuture(QtConcurrent::run([=] {
        QList<SearchBoxStruct::Ptr> list;
        //左边是从从xml解析出来的数据，右边是需要被翻译成的数据；
        //后续若还有相同模块还有一样的翻译文言，也可在此处添加类似处理，并在注释处添加　//~ child_page xxx
        m_transChildPageName = {
            //account
            { "Create Account", QObject::tr("Create Account") },
            { "Auto Login", QObject::tr("Auto Login")},
            { "Login Without Password", QObject::tr("Login Without Password")},
            { "Change Password", QObject::tr("Change Password")},
            { "Delete Account", QObject::tr("Delete Account")},

            //union id
            { "Sign In", /*QObject::tr("Sign In")*/"登 录" },

            //bluetooth
            { "My Devices", QObject::tr("My Devices") },
            { "Other Devices", QObject::tr("Other Devices") },
            { "Enable Bluetooth to find nearby devices (speakers, keyboard, mouse)", QObject::tr("Enable Bluetooth to find nearby devices (speakers, keyboard, mouse)") },

            //commoninfo
            { "Boot Menu", QObject::tr("Boot Menu") },
            { "Developer Mode", QObject::tr("Developer Mode") },
            { "User Experience Program", QObject::tr("User Experience Program") },

            //datetime
            { "Format Settings", QObject::tr("Format Settings") },
            { "Timezone List", QObject::tr("Timezone List") },
            { "Time Settings", QObject::tr("Time Settings") },
            { "Timezone List/Change System Timezone", QObject::tr("Change System Timezone") },

            //defapp
            { "Webpage", QObject::tr("Webpage") },
            { "Mail", QObject::tr("Mail") },
            { "Text", QObject::tr("Text") },
            { "Music", QObject::tr("Music") },
            { "Video", QObject::tr("Video") },
            { "Picture", QObject::tr("Picture") },
            { "Terminal", QObject::tr("Terminal") },

            //display
            { "Brightness", QObject::tr("Brightness") },
            { "Auto Brightness", QObject::tr("Auto Brightness") },
            { "Night Shift", QObject::tr("Night Shift") },
            { "Change Color Temperature", QObject::tr("Change Color Temperature") },
            { "Multiple Displays", QObject::tr("Multiple Displays") },
            { "Mode", QObject::tr("Mode") },
            { "Main Screen", QObject::tr("Main Screen") },
            { "Refresh Rate", QObject::tr("Refresh Rate") },
            { "Resolution", QObject::tr("Resolution") },
            { "Rotation", QObject::tr("Rotation") },
            { "Display Scaling", QObject::tr("Display Scaling") },

            //keyboard
            { "Shortcuts", QObject::tr("Shortcuts") },
            { "Keyboard Layout", QObject::tr("Keyboard Layout") },
            { "System Language", QObject::tr("System Language") },

            //network
            { "Personal Hotspot", QObject::tr("Personal Hotspot") },
            { "DSL", QObject::tr("DSL") },
            { "VPN", QObject::tr("VPN") },
            { "Wired Network", QObject::tr("Wired Network") },
            { "Wireless Network", QObject::tr("Wireless Network") },
            { "Network Details", QObject::tr("Network Details") },
            { "Application Proxy", QObject::tr("Application Proxy") },
            { "System Proxy", QObject::tr("System Proxy") },

            //notification
            { "System Notifications", QObject::tr("System Notifications") },

            //personalization
            { "Font", QObject::tr("Font") },
            { "Icon Theme", QObject::tr("Icon Theme") },
            { "Cursor Theme", QObject::tr("Cursor Theme") },

            //sound
            { "Sound Effects", QObject::tr("Sound Effects") },
            { "Devices", QObject::tr("Devices") },
            { "Input", QObject::tr("Input") },
            { "Output", QObject::tr("Output") },

            //systeminfo
            { "About This PC", QObject::tr("About This PC") },
            { "Edition License", QObject::tr("Edition License") },
            { "End User License Agreement", QObject::tr("End User License Agreement") },
            { "Privacy Policy", QObject::tr("Privacy Policy") },

            //update
            { "Check for Updates", QObject::tr("Check for Updates") },
            { "Updates", /*QObject::tr("Updates")*/"检查更新" },
            { "Update Settings", QObject::tr("Update Settings") },

            //wacom
            { "Pressure Sensitivity", QObject::tr("Pressure Sensitivity") },
            { "Mode", QObject::tr("Mode") },

            //power
            { "On Battery", QObject::tr("On Battery") },
            { "Plugged In", QObject::tr("Plugged In") },

            //mouse
            { "General", QObject::tr("General") },
            { "Mouse", QObject::tr("Mouse") },
            { "Touchpad", QObject::tr("Touchpad") },
            { "TrackPoint", QObject::tr("TrackPoint") },

            //plugin : input
            { "Input Methods", QObject::tr("Input Methods") },
            { "Backup and Restore", QObject::tr("Backup and Restore") },
            { "Domain Management", QObject::tr("Domain Management") },
#ifdef QT_DEBUG
            { "Backup and Restore", "备份还原" },
            { "Domain Management", "域管理" },
#endif
        };
#if DEBUG_XML_SWITCH
        qDebug() << " [SearchWidget] " << Q_FUNC_INFO;
#endif
        for (const QString &i : m_xmlFilePath) {
            QString xmlPath = i.arg(m_lang);
            QFile   file(xmlPath);

            if (!file.exists()) {
                qDebug() << " [SearchWidget] File not exist";
                continue;
            }

            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << " [SearchWidget] File open failed";
                continue;
            }

            QXmlStreamReader            xmlRead(&file);
            QStringRef                  dataName;
            SearchBoxStruct::Ptr searchBoxStrcut = std::make_shared<SearchBoxStruct>();
            QString xmlExplain;

            /*
            <message>
                <source>Update Setting</source>
                <translation>更新设置</translation>
                <extra-contents_path>/update/Update Setting</extra-contents_path>
        </message>

        +::StartElement:  "message"

            +::StartElement:  "source"
                xmlRead.text :  "Update Setting"
            -::EndElement:  "source"

            +::StartElement:  "translation"
                xmlRead.text :  "更新设置"
            -::EndElement:  "translation"

            +::StartElement:  "extra-contents_path"
                xmlRead.text :  "/update/Update Setting"
            -::EndElement:  "extra-contents_path"

        -::EndElement:  "message"

        //以上 <>  </> 一一对应,
        //在xml的 <> 时进入 StartElement ,
        //在xml 显示中间内容时进入 Characters,
        //在xml的 </>时进入EndElement

        */
            //遍历XML文件,读取每一行的xml数据都会
            //先进入StartElement读取出<>中的内容;
            //再进入Characters读取出中间数据部分;
            //最后进入时进入EndElement读取出</>中的内容
            QString strSource = "";
            while (!xmlRead.atEnd()) {
                switch (xmlRead.readNext()) {
                    case QXmlStreamReader::StartElement:
#if DEBUG_XML_SWITCH
                        qDebug() << " [SearchWidget] +::StartElement: " << xmlRead.name() << xmlRead.text();
#endif
                        xmlExplain = xmlRead.name().toString();
                        break;
                    case QXmlStreamReader::Characters:
                        if (!xmlRead.isWhitespace()) {
#if DEBUG_XML_SWITCH
                            qDebug() << " [SearchWidget]  xmlRead.text : " << xmlRead.text().toString();
#endif
                            if (xmlExplain == XML_Source) {  // get xml source date
                                searchBoxStrcut->translateContent = xmlRead.text().toString().remove('/').trimmed();
                                searchBoxStrcut->source = xmlRead.text().toString();
                                strSource = xmlRead.text().toString();
                            }
                            else if (xmlExplain == XML_Title) {
                                if (xmlRead.text().toString() != "")  // translation not nullptr can set it
                                    searchBoxStrcut->translateContent = xmlRead.text().toString().remove('/').trimmed();
#if DEBUG_XML_SWITCH
                                qDebug() << " [SearchWidget] searchBoxStrcut->translateContent : " << searchBoxStrcut->translateContent;
#endif
                            }
                            else if (xmlExplain == XML_Numerusform) {
                                if (xmlRead.text().toString() != "")  // translation not nullptr can set it
                                    searchBoxStrcut->translateContent = xmlRead.text().toString().remove('/').trimmed();
                            }
                            else if (XML_Child_Path == xmlExplain) {
                                QString childPage = m_transChildPageName.value(xmlRead.text().toString());
                                if (childPage == "") {
                                    childPage = xmlRead.text().toString();
                                    qWarning() << " [SearchWidget]  child page can't translate. childPage : " << childPage;
                                }
                                searchBoxStrcut->childPageName = childPage;
                                if (!m_childWidgetList.contains(childPage))
                                    m_childWidgetList.append(childPage);
                            }
                            else if (XML_ChildHide_Path == xmlExplain) {
                                //添加二级页面和三级页面都要进入的搜索数据，类似 ： "默认程序 -> 网页 / 添加默认程序" 和 "默认程序 -> 网页"
                                //以上两种数据都需要搜索，因此需要保存一个特殊的子页面list
                                QString hideChildPage = m_transChildPageName.value(xmlRead.text().toString());
                                if (!m_childeHideWidgetList.contains(hideChildPage)) {
                                    m_childeHideWidgetList.append(hideChildPage);
                                }
                            }
                            else if (xmlExplain == XML_Explain_Path) {
                                searchBoxStrcut->fullPagePath = xmlRead.text().toString();
                                // follow path module name to get actual module name  ->  Left module dispaly can support
                                // mulLanguages
                                searchBoxStrcut->actualModuleName = getModulesName(searchBoxStrcut->fullPagePath.section('/', 1, 1));

                                if ("" == searchBoxStrcut->actualModuleName || "" == searchBoxStrcut->translateContent) {
                                    searchBoxStrcut = std::make_shared<SearchBoxStruct>();
                                    continue;
                                }

                                //判断是否为服务器,是服务器时,若当前不是服务器就不添加"Server"
                                if (isLoadText(searchBoxStrcut->translateContent)) {
                                    searchBoxStrcut = std::make_shared<SearchBoxStruct>();
                                    continue;
                                }

                                //判断是否为contens服务器,是contens服务器时,若当前不是服务器就不添加"Server"
                                if (isLoadContensText(searchBoxStrcut->translateContent)) {
                                    searchBoxStrcut = std::make_shared<SearchBoxStruct>();
                                    continue;
                                }

                                //判断是否为服务器，如果是服务器状态下搜索不到网络账户相关（所有界面）
                                if (m_bIsServerType && tr("Cloud Account") == searchBoxStrcut->actualModuleName) {
                                    searchBoxStrcut = std::make_shared<SearchBoxStruct>();
                                    continue;
                                }

                                //判断是否非社区版，如果是非社区版本，屏蔽镜像源列表
                                if (!IsCommunitySystem) {
                                    if("Smart Mirror Switch" == strSource
                                            || "Switch it on to connect to the quickest mirror site automatically" == strSource
                                            || "System Repository Detection" == strSource
                                            || "Mirror List" == strSource) {
                                        searchBoxStrcut = std::make_shared<SearchBoxStruct>();
                                        continue;
                                    }
                                }

                                list << searchBoxStrcut;
                                searchBoxStrcut = std::make_shared<SearchBoxStruct>();
                            }
                        }
                        break;
                    case QXmlStreamReader::EndElement:
#if DEBUG_XML_SWITCH
                        qDebug() << " [SearchWidget] -::EndElement: " << xmlRead.name();
#endif
                        break;
                    default: break;
                }
            }

            qDebug() << " [SearchWidget] m_EnterNewPagelist.count : " << m_EnterNewPagelist.count();

            file.close();
        }

        return list;
    }));
}

//save all modules moduleInteface name and actual moduleName
//moduleName : moduleInteface name  (used to path module to translate searchName)
//searchName : actual module
void SearchModel::addModulesName(QString moduleName, const QString &searchName, QIcon icon, QString translation)
{
    QPair<QString, QString> data;
    data.first = moduleName;
    data.second = searchName;
    m_moduleNameList.append(data);

    if (!translation.isEmpty()) {
        m_xmlFilePath.insert(translation);
    }

    m_iconMap.insert(moduleName, icon);

#if DEBUG_XML_SWITCH
    qDebug() << " [SearchWidget] moduleName : " << moduleName << " , searchName : " << searchName;
#endif
}

void SearchModel::addUnExsitData(const QString &module, const QString &datail)
{
    auto res = std::any_of(m_unexsitList.begin(), m_unexsitList.end(), [=](const UnexsitStruct &date) {
        return date.module == module;
    });

    if (res) {
        return;
    }

    UnexsitStruct data;
    data.module = module;
    data.datail = datail;
    m_unexsitList.append(data);
}

void SearchModel::removeUnExsitData(const QString &module, const QString &datail)
{
    auto find = std::find_if(m_unexsitList.begin(), m_unexsitList.end(), [=](const UnexsitStruct& item) {
        return (item.module == module) && (item.datail == datail);
    });

    if (find != m_unexsitList.end()) {
        m_unexsitList.erase(find);
    }
}

void SearchModel::addSpecialThreeMenuMap(const QString &name, bool flag)
{
    m_specialThreeMenuMap.insert(name, flag);
    loadxml();
}

//获取模块是否显示
bool SearchModel::getModuleVisible(const QString module)
{
    if (module == "") {
        return false;
    }

    return m_hideModuleList.value(module);
}

//获取模块内子页面是否显示
bool SearchModel::getWidgetVisible(const QString module, QString widget)
{
    if (module == "") {//widget可以为空
        return false;
    }

    HideChildWidgetStruct value;
    auto findModule = std::find_if(m_hideWidgetList.begin(), m_hideWidgetList.end(), [&module](const HideChildWidgetStruct& item) {
        return (item.module == module);
    });

    if (findModule != m_hideWidgetList.end()) {
        value = (*findModule);
        return value.childWidgetMap.value(widget);
    }

    return false;
}

//获取模块内子页面的搜索数据是否显示
bool SearchModel::getDetailVisible(const QString module, QString widget, QString detail)
{
    if (module == "" || widget == "" || detail == "") {
        return false;
    }

    HideChildWidgetDetailStruct value;
    auto findWidget = std::find_if(m_hideWidgetDetailList.begin(), m_hideWidgetDetailList.end(), [&module, &widget](const HideChildWidgetDetailStruct& item) {
        return (item.module == module && item.childWidget == widget);
    });

    if (findWidget != m_hideWidgetDetailList.end()) {
        value = (*findWidget);
        return value.detailMap.value(detail);
    }

    return false;
}

//设置模块是否显示
void SearchModel::setModuleVisible(const QString &module, bool visible)
{
    if (module == "" || m_hideModuleList.value(module) == visible) {
        return;
    }

    m_hideModuleList.insert(module, visible);
}

//设置模块内子页面是否显示
void SearchModel::setWidgetVisible(const QString &module, const QString &widget, bool visible)
{
    if (module == "" || widget == "") {
        return;
    }

    for (auto &data : m_hideWidgetList) {
        if (data.module == module && data.childWidgetMap.contains(widget)) {
            data.childWidgetMap.insert(widget, visible);
            return;
        }
    }

    //以下为第一次新增数据
    HideChildWidgetStruct value;
    value.module = module;
    value.childWidgetMap.insert(widget, visible);

    int count = m_hideWidgetList.count();
    if (count == 0) {
        m_hideWidgetList.append(value);
    } else {
        for (int i = 0; i < count; i++) {
            if (m_hideWidgetList[i].module == module) {
                m_hideWidgetList[i].childWidgetMap.insert(widget, visible);
                return;
            }
            if (i == count - 1) {
                m_hideWidgetList.append(value);
            }
        }
    }
}

//设置模块内子页面的详细搜索数据是否显示
void SearchModel::setDetailVisible(const QString &module, const QString &widget, const QString &detail, bool visible)
{
    for (auto &data : m_hideWidgetDetailList) {
        if (data.module == module && data.childWidget == widget && data.detailMap.contains(detail)) {
            data.detailMap.insert(detail, visible);
            return;
        }
    }

    //以下为第一次新增数据
    HideChildWidgetDetailStruct value;
    value.module = module;
    value.childWidget = widget;
    value.detailMap.insert(detail, visible);

    if (!m_hideModuleList.value(module)) {
        return;
    }

    int count = m_hideWidgetDetailList.count();
    if (count == 0) {
        m_hideWidgetDetailList.append(value);
    } else {
        for (int i = 0; i < count; i++) {
            if (m_hideWidgetDetailList[i].module == module && m_hideWidgetDetailList[i].childWidget == widget) {
                m_hideWidgetDetailList[i].detailMap.insert(detail, visible);
                return;
            }
            if (i == count -1) {
                m_hideWidgetDetailList.append(value);
            }
        }
    }
}

void SearchModel::updateSearchData(const QString &module)
{
    loadxml(module);
}

void SearchModel::setRemoveableDeviceStatus(const QString &name, bool isExist)
{
//    setModuleVisible(name, isExist);
//    QPair<QString, QString> value("", "");

//    //判断可移除设备是否在默认list中有记录，有记录才需要继续走后面流程
//    //根据name(模块/小模块名称)从默认list，取出对应的模块和page
//    auto res = std::find_if(m_removedefaultWidgetList.begin(), m_removedefaultWidgetList.end(), [=] (const QPair<QString, QString> data)->bool{
//        return (data.first == name);
//    });

//    if (res != m_removedefaultWidgetList.end()) {
//        value = (*res);
//        if (res->second == "On Battery") {
//            m_bIsOnBattery = isExist;
//        } else if (res->second == "Touchpad") {
//            m_bIsUseTouchpad = isExist;
//        }
//    }

//    if ("" != value.first && "" != value.second) {
//        //值存在，移除设备，list移除该值
//        if (!isExist && m_removeableActualExistList.contains(value)) {
//            m_removeableActualExistList.removeOne(value);
//        } else if (isExist && !m_removeableActualExistList.contains(value)) {
//            //值不存在，现在插入设备，list添加该值
//            m_removeableActualExistList.append(value);
//        }

//        qDebug() << "[setRemoveableDeviceStatus] loadWidget : " << name << " , isExist : " << isExist;
//        loadxml();
//    } else {
//        qDebug() << " Not remember the data , name : " << name;
//        }
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

void SearchWidget::addUnExsitData(const QString &module, const QString &datail)
{
    return m_model->addUnExsitData(module, datail);
}

void SearchWidget::removeUnExsitData(const QString &module, const QString &datail)
{
    return m_model->removeUnExsitData(module, datail);
}

void SearchWidget::setRemoveableDeviceStatus(const QString &name, bool isExist)
{
    return m_model->setRemoveableDeviceStatus(name, isExist);
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