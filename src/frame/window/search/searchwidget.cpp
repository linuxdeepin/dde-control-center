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
#include "window/interface/moduleinterface.h"

#include <QDebug>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QXmlStreamReader>
#include <QCompleter>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::search;

SearchWidget::SearchWidget(QWidget *parent)
    : DTK_WIDGET_NAMESPACE::DSearchEdit(parent)
    , m_xmlExplain("")
    , m_xmlFilePath("")
{
    m_model = new QStandardItemModel(this);
    m_completer = new QCompleter(m_model, this);
    setCompleter(m_completer);

    connect(this, &DTK_WIDGET_NAMESPACE::DSearchEdit::returnPressed, this, [=] {
        if (!text().isEmpty()) {
            //enter defalt set first
            if (!jumpContentPathWidget(text())) {
                const QString& currentCompletion = completer()->currentCompletion();
                qDebug() << Q_FUNC_INFO << " currentCompletion : " << currentCompletion;

                setText(currentCompletion);
                jumpContentPathWidget(currentCompletion);
            }
        }
    });
}

SearchWidget::~SearchWidget()
{

}

bool SearchWidget::jumpContentPathWidget(QString path)
{
    qDebug() << Q_FUNC_INFO << path;

    bool bResult = false;

    if (m_EnterNewPagelist.count() > 0) {
        SearchBoxStruct data = getModuleBtnString(path);

        if (data.title != "" && data.explain != "") {
            bResult = true;
            Q_EMIT notifyModuleSearch(data.title, data.explain);
        }
    } else {
        qWarning() << "QList is nullptr.";
    }

    return bResult;
}

void SearchWidget::loadxml()
{
#if DEBUG_XML_SWITCH
    qDebug() << Q_FUNC_INFO;
#endif
    QString xmlPath = getXmlFilePath();
    QFile file(xmlPath);

    if (!m_EnterNewPagelist.isEmpty()) {
        m_EnterNewPagelist.clear();
    }

    if (m_model->rowCount() > 0) {
        QStandardItem *item = nullptr;
        for (int i = 0; i < m_model->rowCount(); i++) {
            item = m_model->takeItem(i);
            delete item;
            item = nullptr;
        }
        m_model->clear();
    }

    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QXmlStreamReader xmlRead(&file);
            QStringRef dataName;
            QXmlStreamReader::TokenType type = QXmlStreamReader::Invalid;
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
            while (!xmlRead.atEnd()) {
                type = xmlRead.readNext();

                switch (type) {
                case QXmlStreamReader::StartElement:
#if DEBUG_XML_SWITCH
                    qDebug() << "+::StartElement: " << xmlRead.name() << xmlRead.text();
#endif
                    m_xmlExplain = xmlRead.name().toString();
                    break;
                case QXmlStreamReader::Characters:
                    if (!xmlRead.isWhitespace()) {
#if DEBUG_XML_SWITCH
                        qDebug() << "  xmlRead.text : " << xmlRead.text().toString();
#endif
                        if (m_xmlExplain == XML_Title) {
                            m_searchBoxStruct.title = xmlRead.text().toString();
#if DEBUG_XML_SWITCH
                            qDebug() << "---------- m_searchBoxStruct.title : " << m_searchBoxStruct.title;
#endif
                        } else if (m_xmlExplain == XML_Explain_Path) {
                            m_searchBoxStruct.explain = xmlRead.text().toString();
                            m_EnterNewPagelist.append(m_searchBoxStruct);

                            //Add search result content
                            m_model->appendRow(new QStandardItem(ModuleInterface::getIcon(m_searchBoxStruct.explain.section('/', 1, 1)),
                                                                 QString("%1 --> %2").arg(m_searchBoxStruct.title).arg(m_searchBoxStruct.explain)));

                            m_searchBoxStruct.title = "";
                            m_searchBoxStruct.explain = "";
                        } else {
                            //donthing
                        }
                    } else {
                        //qDebug() << "  QXmlStreamReader::Characters with whitespaces.";
                    }
                    break;
                case QXmlStreamReader::EndElement:
#if DEBUG_XML_SWITCH
                    qDebug() << "-::EndElement: " << xmlRead.name();
#endif
//                    if (m_xmlExplain != "") {
//                        m_xmlExplain = "";
//                    }
                    break;
                default:
                    break;
                }
            }

            m_xmlExplain = "";
            m_searchBoxStruct.title = "";
            m_searchBoxStruct.explain = "";
            qDebug() << "   m_EnterNewPagelist.count : " << m_EnterNewPagelist.count();
        } else {
            qWarning() << "-------- File open failed" ;
        }

        file.close();
    } else {
        qWarning() << "-------- File not exist" ;
    }
}

SearchWidget::SearchBoxStruct SearchWidget::getModuleBtnString(QString value)
{
    SearchBoxStruct data;

    data.title = value.section('/', 1, 1).remove('/');
    data.explain = value.section('/', 2, 3);

#if DEBUG_XML_SWITCH
    qDebug() << Q_FUNC_INFO << "data.title : " << data.title << "   ,  data.explain : " << data.explain;
#endif

    return data;
}

QString SearchWidget::getXmlFilePath()
{
    return m_xmlFilePath;
}

void SearchWidget::setLanguage(QString type)
{
    QString xmlPath = RES_TS_PATH + QString("dde-control-center_%1.ts").arg(type);;
    if (m_xmlFilePath != xmlPath) {
        m_xmlFilePath = xmlPath;
    }

    loadxml();
}
