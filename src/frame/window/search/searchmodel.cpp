/*
* Copyright (C) 2019 ~ 2021 Deepin Technology Co., Ltd.
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
#include "searchmodel.h"
#include "window/utils.h"

#include <DPinyin>
#include <QDebug>
#include <QRegularExpression>
#include <QFutureWatcher>
#include <QtConcurrent>

#define DEBUG_XML_SWITCH 0

const QString XML_Source = "source";
const QString XML_Title = "translation";
const QString XML_Numerusform = "numerusform";
const QString XML_Explain_Path = "extra-contents_path";
const QString XML_Child_Path = "extra-child_page";
const QString XML_ChildHide_Path = "extra-child_page_hide";

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::search;

SearchModel::SearchModel(QObject *parent)
    : QStandardItemModel(parent)
    , m_bIsChinese(false)
    , m_bIstextEdited(false)
    , m_dataUpdateCompleted(false)
{
    //左边是从从xml解析出来的数据，右边是需要被翻译成的数据；
    //后续若还有相同模块还有一样的翻译文言，也可在此处添加类似处理，并在注释处添加　//~ child_page xxx
    m_transChildPageName = {
        { "Manage Input Methods", QObject::tr("Input Methods") },
    };
}

QString SearchModel::formatSearchData(QString data1, QString data2, QString data3)
{
    QString ret = "";

    if (data3 != "") {
        ret = QString("%1 --> %2 / %3").arg(data1).arg(data2).arg(data3);
    } else {
        ret = QString("%1 --> %2").arg(data1).arg(data2);
    }

    return ret;
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

    bool isPlugins = false;
    bool bIsContinue = false;
    bool bIsTwoLevel = false;
    QString searchModule = "";
    QString searchData = "";

    for (SearchBoxStruct::Ptr searchBoxStrcut : m_originList) {

        searchModule = searchBoxStrcut->fullPagePath.section('/', 0, 1).remove('/').trimmed();
        searchData = searchBoxStrcut->fullPagePath.section('/', 2, -1).remove('/').trimmed();

        //插件数据默认都显示 --> 未解决历史遗留问题，修改接口后依旧兼容旧接口； 如果统一使用setxxxVisible可以删除这里
        if (!m_transPlusData.value(searchModule).isEmpty() || !m_transPlusData.value(searchData).isEmpty()) {
            qInfo() << " [loadxml] Plugins data :"
                       << searchBoxStrcut->actualModuleName
                       << searchBoxStrcut->childPageName
                       <<searchBoxStrcut->fullPagePath
                       <<searchBoxStrcut->source
                       <<searchBoxStrcut->translateContent;
            if (!specialProcessData(searchBoxStrcut)) {
                isPlugins = true;
            }
        }

        if (!isPlugins) {
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
        }

        m_EnterNewPagelist.append(searchBoxStrcut);
        isPlugins = false;

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
        // guoyao：针对Union ID（中文环境使用英文模块名），dataBackup->actualModuleName将被过滤为空，所以会return掉；但是data数据并不会更改，所以用data数据进行判断即可
        if (data->actualModuleName == DTK_CORE_NAMESPACE::Chinese2Pinyin(dataBackup->actualModuleName)) return;

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

//主要用于解决一些特殊数据，比如同时加载了二级和三级页面搜索数据，而要删除二级页面数据； true : 不加载
bool SearchModel::specialProcessData(SearchBoxStruct::Ptr data)
{
    bool ret = false;
    if (data->fullPagePath == "/keyboard/Manage Input Methods" && data->childPageName == "") {
        return true;
    }
    return ret;
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
        loadxml();
        m_dataUpdateCompleted = true;

    });

    m_childeHideWidgetList.clear();

    watcher->setFuture(QtConcurrent::run([=] {
        QList<SearchBoxStruct::Ptr> list;

        //解决历史遗留问题，适配已经存在的插件搜索数据(不需要翻译,只要第二个字符串不为空即可)
        m_transPlusData = {
            //plugin : input
            { "Input Methods", "Input Methods" },
            { "Manage Input Methods", "Manage Input Methods" },

            { "Backup and Restore", "Backup and Restore" },
            { "Domain Management", "Domain Management" },

            { "translation", "Translation" },
            { "assistant", "Desktop AI Assistant" },
            { "iat", "Speech to Text" },
            { "tts", "Text to Speech" },
        };
#if DEBUG_XML_SWITCH
        qDebug() << " [SearchWidget] " << Q_FUNC_INFO;
#endif
        for (const QString &i : m_xmlFilePath) {
            QString xmlPath = i.arg(m_lang);
            QFile   file(xmlPath);

            if (!file.exists()) {
                qDebug() << " [SearchWidget] File not exist:" << xmlPath;
                continue;
            }

            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << " [SearchWidget] File open failed:" << xmlPath;
                continue;
            }

            QXmlStreamReader            xmlRead(&file);
            QStringRef                  dataName;
            SearchBoxStruct::Ptr searchBoxStrcut = std::make_shared<SearchBoxStruct>();
            QString xmlExplain;

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
    qDebug() << "updateSearchData:" << module;
    loadxml(module);
}

void SearchModel::addChildPageTrans(const QString &menu, const QString &tran)
{
    if (!m_transChildPageName.contains(menu)) {
        m_transChildPageName.insert(menu, tran);
    }
}
