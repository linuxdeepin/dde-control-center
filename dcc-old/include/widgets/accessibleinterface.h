//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ACCESSIBLEINTERFACE_H
#define ACCESSIBLEINTERFACE_H

#include <QAccessible>
#include <QAccessibleWidget>
#include <QEvent>
#include <QMap>
#include <QString>
#include <QWidget>
#include <QObject>
#include <QMetaEnum>
#include <QMouseEvent>
#include <QApplication>
#include <QRect>

#define SEPARATOR "_"

inline QString getAccessibleName(QWidget *w, QAccessible::Role r, const QString &fallback)
{
    const QString lowerFallback = fallback.toLower();
    // 避免重复生成
    static QMap< QObject *, QString > objnameMap;
    if (!objnameMap[w].isEmpty())
        return objnameMap[w];

    static QMap< QAccessible::Role, QList< QString > > accessibleMap;
    QString oldAccessName = w->accessibleName().toLower();
    oldAccessName.replace(SEPARATOR, "");

    // 按照类型添加固定前缀
    QMetaEnum metaEnum = QMetaEnum::fromType<QAccessible::Role>();
    QByteArray prefix = metaEnum.valueToKeys(r);
    switch (r) {
    case QAccessible::Form:             prefix = "Form";        break;
    case QAccessible::Button:           prefix = "Btn";         break;
    case QAccessible::StaticText:       prefix = "Label";       break;
    case QAccessible::EditableText:     prefix = "Editable";    break;
    case QAccessible::Slider:           prefix = "Slider";      break;
    default:                        break;
    }

    // 再加上标识
    QString accessibleName = QString::fromLatin1(prefix) + SEPARATOR;
    accessibleName += oldAccessName.isEmpty() ? lowerFallback : oldAccessName;
    // 检查名称是否唯一
    if (accessibleMap[r].contains(accessibleName)) {
        // 获取编号，然后+1
        int pos = accessibleName.indexOf(SEPARATOR);
        int id = accessibleName.mid(pos + 1).toInt();

        QString newAccessibleName;
        do {
            // 一直找到一个不重复的名字
            newAccessibleName = accessibleName + SEPARATOR + QString::number(++id);
        } while (accessibleMap[r].contains(newAccessibleName));

        accessibleMap[r].append(newAccessibleName);
        objnameMap.insert(w, newAccessibleName);

        QObject::connect(w, &QWidget::destroyed, [ = ] (QObject *obj) { // 对象销毁后移除占用名称
            objnameMap.remove(obj);
            accessibleMap[r].removeOne(newAccessibleName);
        });
        return newAccessibleName;
    } else {
        accessibleMap[r].append(accessibleName);
        objnameMap.insert(w, accessibleName);

        QObject::connect(w, &QWidget::destroyed, [ = ] (QObject *obj) { // 对象销毁后移除占用名称
            objnameMap.remove(obj);
            accessibleMap[r].removeOne(accessibleName);
        });
        return accessibleName;
    }
}

// 公共的功能
#define FUNC_CREATE(classname,accessibletype,accessdescription)    Accessible##classname(classname *w) \
        : QAccessibleWidget(w,accessibletype,#classname)\
        , m_w(w)\
        , m_description(accessdescription)\
    {}\

#define FUNC_TEXT(classname,accessiblename) inline QString Accessible##classname::text(QAccessible::Text t) const{\
        switch (t) {\
        case QAccessible::Name:\
            return getAccessibleName(m_w, this->role(), accessiblename);\
        case QAccessible::Description:\
            return m_description;\
        default:\
            return QString();\
        }\
    }\

// button控件特有功能
#define FUNC_ACTIONNAMES(classname) inline QStringList Accessible##classname::actionNames() const{\
        if(!m_w->isEnabled())\
            return QStringList();\
        return QStringList() << pressAction()<< showMenuAction();\
    }\

#define FUNC_DOACTION(classname) inline void Accessible##classname::doAction(const QString &actionName){\
        if(actionName == pressAction())\
        {\
            QPointF localPos = m_w->geometry().center();\
            QMouseEvent event(QEvent::MouseButtonPress,localPos,Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);\
            qApp->sendEvent(m_w,&event);\
            QMouseEvent eventr(QEvent::MouseButtonRelease,localPos,Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);\
            qApp->sendEvent(m_w,&eventr);\
        }\
        else if(actionName == showMenuAction())\
        {\
            QPointF localPos = m_w->geometry().center();\
            QMouseEvent event(QEvent::MouseButtonPress,localPos,Qt::RightButton,Qt::RightButton,Qt::NoModifier);\
            qApp->sendEvent(m_w,&event);\
        }\
    }\

// QLABEL控件特有功能
#define FUNC_TEXT_(classname) inline QString Accessible##classname::text(int startOffset, int endOffset) const{\
        Q_UNUSED(startOffset)\
        Q_UNUSED(endOffset)\
        return m_w->text();\
    }\

#define FUNC_CHILD_LABLE(classname) inline QAccessibleInterface *Accessible##classname::child(int index) const{\
        return QAccessibleWidget::child(index);\
    }\

// EDITABLE控件特有功能
#define FUNC_DELETETEXT(classname) inline void Accessible##classname::deleteText(int startOffset, int endOffset) {\
        m_w->setText(m_w->text().remove(startOffset, endOffset - startOffset));\
    }\

#define FUNC_INSERTTEXT(classname) inline void Accessible##classname::insertText(int startOffset, const QString &text) {\
        m_w->setText(m_w->text().insert(startOffset, text));\
    }\

#define FUNC_REPLACETEXT(classname) inline void Accessible##classname::replaceText(int startOffset, int endOffset, const QString &text) {\
        m_w->setText(m_w->text().replace(startOffset, endOffset - startOffset, text));\
    }\

#define FUNC_CHILD_EDITABLE(classname) QAccessibleInterface *Accessible##classname::child(int index) const{\
        return nullptr;\
    }\

#define FUNC_SELECTION(classname) inline void Accessible##classname::selection(int selectionIndex, int *startOffset, int *endOffset) const {\
        m_w->setSelection(*startOffset, *endOffset);\
        *startOffset = *endOffset = 0;\
        if (selectionIndex != 0)\
            return;\
        *startOffset = m_w->selectionStart();\
        *endOffset = *startOffset + m_w->selectedText().count();\
    }\

#define FUNC_SELECTIONCOUNT(classname) inline int Accessible##classname::selectionCount() const {\
        return m_w->hasSelectedText() ? 1 : 0;\
    }\

#define FUNC_ADDSELECTION(classname) inline void Accessible##classname::addSelection(int startOffset, int endOffset) {\
        setSelection(0, startOffset, endOffset);\
    }\

#define FUNC_REMOVESELECTION(classname) inline void Accessible##classname::removeSelection(int selectionIndex) {\
        if (selectionIndex != 0)\
            return;\
        m_w->deselect();\
    }\

#define FUNC_SETSELECTION(classname) inline void Accessible##classname::setSelection(int selectionIndex, int startOffset, int endOffset) {\
        if (selectionIndex != 0)\
            return;\
        m_w->setSelection(startOffset, endOffset - startOffset);\
    }\

#define FUNC_CURORPOSITION(classname) inline int Accessible##classname::cursorPosition() const {\
        return m_w->cursorPosition();\
    }\

#define FUNC_SETCURORPOSITION(classname) inline void Accessible##classname::setCursorPosition(int position) {\
        m_w->setCursorPosition(position);\
    }\

#define FUNC_TEXT_LineEdit(classname) inline QString Accessible##classname::text(int startOffset, int endOffset) const{\
        if (startOffset > endOffset)\
            return QString();\
        if (m_w->echoMode() != QLineEdit::Normal)\
            return QString();\
        return m_w->text().mid(startOffset, endOffset - startOffset);\
    }\

#define FUNC_TEXTBEFOREOFFSET(classname) QString Accessible##classname::textBeforeOffset(int offset, QAccessible::TextBoundaryType boundaryType, int *startOffset, int *endOffset) const {\
        if (m_w->echoMode() != QLineEdit::Normal) {\
            *startOffset = *endOffset = -1;\
            return QString();\
        }\
        if (offset == -2)\
            offset = cursorPosition();\
        return QAccessibleTextInterface::textBeforeOffset(offset, boundaryType, startOffset, endOffset);\
    }\

#define FUNC_TEXTAFTEROFFSET(classname) QString Accessible##classname::textAfterOffset(int offset, QAccessible::TextBoundaryType boundaryType, int *startOffset, int *endOffset) const {\
        if (lineEdit()->echoMode() != QLineEdit::Normal) {\
            *startOffset = *endOffset = -1;\
            return QString();\
        }\
        if (offset == -2)\
            offset = cursorPosition();\
        return QAccessibleTextInterface::textAfterOffset(offset, boundaryType, startOffset, endOffset);\
    }\

#define FUNC_TEXTATOFFSET(classname) QString Accessible##classname::textAtOffset(int offset, QAccessible::TextBoundaryType boundaryType, int *startOffset, int *endOffset) const {\
        if (m_w->echoMode() != QLineEdit::Normal) {\
            *startOffset = *endOffset = -1;\
            return QString();\
        }\
        if (offset == -2)\
            offset = cursorPosition();\
        return QAccessibleTextInterface::textAtOffset(offset, boundaryType, startOffset, endOffset);\
    }\

#define FUNC_CHARACTERCOUNT(classname) inline int Accessible##classname::characterCount() const {\
        return m_w->text().count();\
    }\

#define FUNC_CHARACTERRECT(classname) inline QRect Accessible##classname::characterRect(int offset) const {\
        Q_UNUSED(offset)\
        return QRect();\
    }\

#define FUNC_OFFSETATPOINT(classname) inline int Accessible##classname::offsetAtPoint(const QPoint &point) const {\
        QPoint p = m_w->mapFromGlobal(point);\
        return m_w->cursorPositionAt(p);\
    }\

#define FUNC_SCROLLTOSUBSTRING(classname) inline void Accessible##classname::scrollToSubstring(int startIndex, int endIndex) {\
        m_w->setCursorPosition(endIndex);\
        m_w->setCursorPosition(startIndex);\
    }\

#define FUNC_ATTRIBUTES(classname) inline QString Accessible##classname::attributes(int offset, int *startOffset, int *endOffset) const {\
        *startOffset = *endOffset = offset;\
        return QString();\
    }\

// Slider控件特有功能
#define FUNC_CURRENTVALUE(classname) inline QVariant Accessible##classname::currentValue() const{\
        return m_w->value();\
    }\

#define FUNC_SETCURRENTVALUE(classname) inline void Accessible##classname::setCurrentValue(const QVariant &value){\
        return m_w->setValue(value.toInt());\
    }\

#define FUNC_MAXMUMVALUE(classname) inline QVariant Accessible##classname::maximumValue() const{\
        return QVariant(m_w->maximum());\
    }\

#define FUNC_FUNC_MINIMUMVALUE(classname) inline QVariant Accessible##classname::minimumValue() const{\
        return QVariant(m_w->minimum());\
    }\

#define FUNC_FUNC_MINIMUMSTEPSIZE(classname) inline QVariant Accessible##classname::minimumStepSize() const{\
        return QVariant(m_w->singleStep());\
    }\

#define SET_FORM_ACCESSIBLE_WITH_DESCRIPTION(classname,accessiblename,accessdescription)  class Accessible##classname : public QAccessibleWidget\
    {\
    public:\
        FUNC_CREATE(classname,QAccessible::Form,accessdescription)\
        QString text(QAccessible::Text t) const override;\
        void *interface_cast(QAccessible::InterfaceType t) override{\
                switch (t) {\
                case QAccessible::ActionInterface:\
                    return static_cast<QAccessibleActionInterface*>(this);\
                default:\
                    return nullptr;\
                }\
            }\
    private:\
    classname *m_w;\
    QString m_description;\
    };\
    FUNC_TEXT(classname,accessiblename)\

#define SET_BUTTON_ACCESSIBLE_WITH_DESCRIPTION(classname,accessiblename,accessdescription)  class Accessible##classname : public QAccessibleWidget\
    {\
    public:\
        FUNC_CREATE(classname,QAccessible::Button,accessdescription)\
        QString text(QAccessible::Text t) const override;\
        void *interface_cast(QAccessible::InterfaceType t) override{\
                switch (t) {\
                case QAccessible::ActionInterface:\
                    return static_cast<QAccessibleActionInterface*>(this);\
                default:\
                    return nullptr;\
                }\
            }\
        QStringList actionNames() const override;\
        void doAction(const QString &actionName) override;\
    private:\
    classname *m_w;\
    QString m_description;\
    };\
    FUNC_TEXT(classname,accessiblename)\
    FUNC_ACTIONNAMES(classname)\
    FUNC_DOACTION(classname)\

#define SET_LABEL_ACCESSIBLE_WITH_DESCRIPTION(classname,accessiblename,type,accessdescription)  class Accessible##classname : public QAccessibleWidget, public QAccessibleTextInterface\
    {\
    public:\
        FUNC_CREATE(classname,type,accessdescription)\
        QString text(QAccessible::Text t) const override;\
        void *interface_cast(QAccessible::InterfaceType t) override{\
                switch (t) {\
                case QAccessible::ActionInterface:\
                    return static_cast<QAccessibleActionInterface*>(this);\
                case QAccessible::TextInterface:\
                    return static_cast<QAccessibleTextInterface*>(this);\
                default:\
                    return nullptr;\
                }\
            }\
        QAccessibleInterface *child(int index) const override;\
        QString text(int startOffset, int endOffset) const override;\
        void selection(int selectionIndex, int *startOffset, int *endOffset) const override {\
            Q_UNUSED(selectionIndex)\
            Q_UNUSED(startOffset)\
            Q_UNUSED(endOffset)\
        }\
        int selectionCount() const override { return 0; }\
        void addSelection(int startOffset, int endOffset) override {\
            Q_UNUSED(startOffset)\
            Q_UNUSED(endOffset)\
        }\
        void removeSelection(int selectionIndex) override {\
            Q_UNUSED(selectionIndex)\
        }\
        void setSelection(int selectionIndex, int startOffset, int endOffset) override {\
            Q_UNUSED(selectionIndex)\
            Q_UNUSED(startOffset)\
            Q_UNUSED(endOffset)\
        }\
        int cursorPosition() const override { return 0; }\
        void setCursorPosition(int position) override {\
            Q_UNUSED(position)\
        }\
        int characterCount() const override { return 0; }\
        QRect characterRect(int offset) const override { \
            Q_UNUSED(offset)\
            return QRect();\
        }\
        int offsetAtPoint(const QPoint &point) const override { \
            Q_UNUSED(point)\
            return 0; \
        }\
        void scrollToSubstring(int startIndex, int endIndex) override {\
            Q_UNUSED(startIndex)\
            Q_UNUSED(endIndex)\
        }\
        QString attributes(int offset, int *startOffset, int *endOffset) const override {\
            Q_UNUSED(startOffset)\
            Q_UNUSED(offset)\
            Q_UNUSED(endOffset)\
            return QString(); \
        }\
    private:\
    classname *m_w;\
    QString m_description;\
    };\
    FUNC_TEXT(classname,accessiblename)\
    FUNC_TEXT_(classname)\

#define SET_SLIDER_ACCESSIBLE_WITH_DESCRIPTION(classname,accessiblename,accessdescription)  class Accessible##classname : public QAccessibleWidget, public QAccessibleValueInterface\
    {\
    public:\
        FUNC_CREATE(classname,QAccessible::Slider,accessdescription)\
        QString text(QAccessible::Text t) const override;\
        void *interface_cast(QAccessible::InterfaceType t) override{\
                switch (t) {\
                case QAccessible::ActionInterface:\
                    return static_cast<QAccessibleActionInterface*>(this);\
                case QAccessible::ValueInterface:\
                    return static_cast<QAccessibleValueInterface*>(this);\
                default:\
                    return nullptr;\
                }\
            }\
        QVariant currentValue() const override;\
        void setCurrentValue(const QVariant &value) override;\
        QVariant maximumValue() const override;\
        QVariant minimumValue() const override;\
        QVariant minimumStepSize() const override { return QVariant(); }\
    private:\
    classname *m_w;\
    QString m_description;\
    };\
    FUNC_TEXT(classname,accessiblename)\
    FUNC_CURRENTVALUE(classname)\
    FUNC_SETCURRENTVALUE(classname)\
    FUNC_MAXMUMVALUE(classname)\
    FUNC_FUNC_MINIMUMVALUE(classname)\

#define SET_EDITABLE_ACCESSIBLE_WITH_DESCRIPTION(classname,accessiblename,accessdescription)  class Accessible##classname : public QAccessibleWidget, public QAccessibleEditableTextInterface, public QAccessibleTextInterface\
    {\
    public:\
        FUNC_CREATE(classname,QAccessible::EditableText,accessdescription)\
        QString text(QAccessible::Text t) const override;\
        QAccessibleInterface *child(int index) const override { \
            Q_UNUSED(index) \
            return nullptr; \
        }\
        void *interface_cast(QAccessible::InterfaceType t) override{\
                switch (t) {\
                case QAccessible::ActionInterface:\
                    return static_cast<QAccessibleActionInterface*>(this);\
                case QAccessible::TextInterface:\
                    return static_cast<QAccessibleTextInterface*>(this);\
                case QAccessible::EditableTextInterface:\
                    return static_cast<QAccessibleEditableTextInterface*>(this);\
                default:\
                    return nullptr;\
                }\
            }\
        QAccessible::State state() const override {\
                QAccessible::State state;\
                state.editable = 1;\
                return state;\
            }\
        QString text(int startOffset, int endOffset) const override;\
        void selection(int selectionIndex, int *startOffset, int *endOffset) const override;\
        int selectionCount() const override;\
        void addSelection(int startOffset, int endOffset) override;\
        void removeSelection(int selectionIndex) override;\
        void setSelection(int selectionIndex, int startOffset, int endOffset) override;\
        int cursorPosition() const override;\
        void setCursorPosition(int position) override;\
        int characterCount() const override;\
        QRect characterRect(int offset) const override;\
        int offsetAtPoint(const QPoint &point) const override;\
        void scrollToSubstring(int startIndex, int endIndex) override;\
        QString attributes(int offset, int *startOffset, int *endOffset) const override;\
        void insertText(int offset, const QString &text) override;\
        void deleteText(int startOffset, int endOffset) override;\
        void replaceText(int startOffset, int endOffset, const QString &text) override;\
    private:\
    classname *m_w;\
    QString m_description;\
    };\
    FUNC_TEXT(classname,accessiblename)\
    FUNC_TEXT_LineEdit(classname)\
    FUNC_DELETETEXT(classname)\
    FUNC_INSERTTEXT(classname)\
    FUNC_REPLACETEXT(classname)\
    FUNC_SELECTION(classname)\
    FUNC_SELECTIONCOUNT(classname)\
    FUNC_ADDSELECTION(classname)\
    FUNC_REMOVESELECTION(classname)\
    FUNC_SETSELECTION(classname)\
    FUNC_CURORPOSITION(classname)\
    FUNC_SETCURORPOSITION(classname)\
    FUNC_CHARACTERCOUNT(classname)\
    FUNC_CHARACTERRECT(classname)\
    FUNC_OFFSETATPOINT(classname)\
    FUNC_SCROLLTOSUBSTRING(classname)\
    FUNC_ATTRIBUTES(classname)\

#define USE_ACCESSIBLE(classnamestring,classname)    if (classnamestring == QLatin1String(#classname) && object && object->isWidgetType())\
    {\
        interface = new Accessible##classname(static_cast<classname *>(object));\
    }\

// [指定objectname]---适用同一个类，但objectname不同的情况
#define USE_ACCESSIBLE_BY_OBJECTNAME(classnamestring,classname,objectname)    if (classnamestring == QLatin1String(#classname) && object && (object->objectName() == objectname) && object->isWidgetType())\
    {\
        interface = new Accessible##classname(static_cast<classname *>(object));\
    }\

/*******************************************简化使用*******************************************/

class AccessibleFactoryBase
{
public:
    explicit AccessibleFactoryBase() {}
    virtual ~AccessibleFactoryBase() {}
    virtual QAccessibleInterface* createObject(QObject *object) = 0;
};

class AccessibleFactoryManager
{
public:
    static AccessibleFactoryBase * RegisterAccessibleFactory(const char *factoryName,AccessibleFactoryBase *factory);
};

#define FactoryMacro(Key,ClassName)\
class FactoryAccessible##ClassName :public AccessibleFactoryBase\
{\
    private:\
    static AccessibleFactoryBase* s_Accessible##ClassName##instance;\
    public:\
    virtual QAccessibleInterface* createObject(QObject *object) override\
    { return new Accessible##ClassName(static_cast<ClassName *>(object));}\
};\
inline AccessibleFactoryBase* FactoryAccessible##ClassName::s_Accessible##ClassName##instance = AccessibleFactoryManager::RegisterAccessibleFactory(Key, new FactoryAccessible##ClassName());

/////////////////////////////////////////////////
#define SET_FORM_ACCESSIBLE(classname,accessiblename)                          SET_FORM_ACCESSIBLE_WITH_DESCRIPTION(classname,accessiblename,"")\
FactoryMacro(#classname, classname)

#define SET_BUTTON_ACCESSIBLE(classname,accessiblename)                        SET_BUTTON_ACCESSIBLE_WITH_DESCRIPTION(classname,accessiblename,"")\
FactoryMacro(#classname, classname)

#define SET_LABEL_ACCESSIBLE(classname,accessiblename)                         SET_LABEL_ACCESSIBLE_WITH_DESCRIPTION(classname,accessiblename,QAccessible::StaticText,"") \
FUNC_CHILD_LABLE(classname)\
FactoryMacro(#classname, classname)

#define SET_DTK_EDITABLE_ACCESSIBLE(classname,accessiblename)                  SET_LABEL_ACCESSIBLE_WITH_DESCRIPTION(classname,accessiblename,QAccessible::EditableText,"") \
FUNC_CHILD_EDITABLE(classname)\
FactoryMacro(#classname, classname)

#define SET_SLIDER_ACCESSIBLE(classname,accessiblename)                        SET_SLIDER_ACCESSIBLE_WITH_DESCRIPTION(classname,accessiblename,"")\
FactoryMacro(#classname, classname)

#define SET_EDITABLE_ACCESSIBLE(classname,accessiblename)                      SET_EDITABLE_ACCESSIBLE_WITH_DESCRIPTION(classname,accessiblename,"")\
FactoryMacro(#classname, classname)
/////////////////////////////////////////////////

#endif // ACCESSIBLEINTERFACE_H
