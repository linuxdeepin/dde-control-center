/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QHBoxLayout>
#include <QPointer>
#include <QDebug>

#include <libdui/libdui_global.h>
#include <libdui/dconstants.h>

#include "constants.h"

#include "networkglobal.h"
#include "networkbaseeditline.h"

static AvailableKeyMap availableKeys;
static QStringList availableSections;
static ErrorInfo errors;
static QPointer<NetworkBaseEditLine> firstBaseEditLine;

// Convert normal string to JSON escaped string. The code is copy from
// qtbase/src/corelib/json/qjsonwriter.cpp

struct QUtf8BaseTraits
{
    static const bool isTrusted = false;
    static const bool allowNonCharacters = true;
    static const bool skipAsciiHandling = false;
    static const int Error = -1;
    static const int EndOfString = -2;

    static bool isValidCharacter(uint u)
    { return int(u) >= 0; }

    static void appendByte(uchar *&ptr, uchar b)
    { *ptr++ = b; }

    static uchar peekByte(const uchar *ptr, int n = 0)
    { return ptr[n]; }

    static qptrdiff availableBytes(const uchar *ptr, const uchar *end)
    { return end - ptr; }

    static void advanceByte(const uchar *&ptr, int n = 1)
    { ptr += n; }

    static void appendUtf16(ushort *&ptr, ushort uc)
    { *ptr++ = uc; }

    static void appendUcs4(ushort *&ptr, uint uc)
    {
        appendUtf16(ptr, QChar::highSurrogate(uc));
        appendUtf16(ptr, QChar::lowSurrogate(uc));
    }

    static ushort peekUtf16(const ushort *ptr, int n = 0)
    { return ptr[n]; }

    static qptrdiff availableUtf16(const ushort *ptr, const ushort *end)
    { return end - ptr; }

    static void advanceUtf16(const ushort *&ptr, int n = 1)
    { ptr += n; }

    // it's possible to output to UCS-4 too
    static void appendUtf16(uint *&ptr, ushort uc)
    { *ptr++ = uc; }

    static void appendUcs4(uint *&ptr, uint uc)
    { *ptr++ = uc; }
};

static inline uchar hexdig(uint u)
{
    return (u < 0xa ? '0' + u : 'a' + u - 0xa);
}

/// returns 0 on success; errors can only happen if \a u is a surrogate:
/// Error if \a u is a low surrogate;
/// if \a u is a high surrogate, Error if the next isn't a low one,
/// EndOfString if we run into the end of the string.
template <typename Traits, typename OutputPtr, typename InputPtr> static inline
int toUtf8(ushort u, OutputPtr &dst, InputPtr &src, InputPtr end)
{
    if (!Traits::skipAsciiHandling && u < 0x80) {
        // U+0000 to U+007F (US-ASCII) - one byte
        Traits::appendByte(dst, uchar(u));
        return 0;
    } else if (u < 0x0800) {
        // U+0080 to U+07FF - two bytes
        // first of two bytes
        Traits::appendByte(dst, 0xc0 | uchar(u >> 6));
    } else {
        if (!QChar::isSurrogate(u)) {
            // U+0800 to U+FFFF (except U+D800-U+DFFF) - three bytes
            if (!Traits::allowNonCharacters && QChar::isNonCharacter(u))
                return Traits::Error;

            // first of three bytes
            Traits::appendByte(dst, 0xe0 | uchar(u >> 12));
        } else {
            // U+10000 to U+10FFFF - four bytes
            // need to get one extra codepoint
            if (Traits::availableUtf16(src, end) == 0)
                return Traits::EndOfString;

            ushort low = Traits::peekUtf16(src);
            if (!QChar::isHighSurrogate(u))
                return Traits::Error;
            if (!QChar::isLowSurrogate(low))
                return Traits::Error;

            Traits::advanceUtf16(src);
            uint ucs4 = QChar::surrogateToUcs4(u, low);

            if (!Traits::allowNonCharacters && QChar::isNonCharacter(ucs4))
                return Traits::Error;

            // first byte
            Traits::appendByte(dst, 0xf0 | (uchar(ucs4 >> 18) & 0xf));

            // second of four bytes
            Traits::appendByte(dst, 0x80 | (uchar(ucs4 >> 12) & 0x3f));

            // for the rest of the bytes
            u = ushort(ucs4);
        }

        // second to last byte
        Traits::appendByte(dst, 0x80 | (uchar(u >> 6) & 0x3f));
    }

    // last byte
    Traits::appendByte(dst, 0x80 | (u & 0x3f));
    return 0;
}

static QByteArray jsonEscapedString(const QString &s)
{
    const uchar replacement = '?';
    QByteArray ba(s.length(), Qt::Uninitialized);

    uchar *cursor = reinterpret_cast<uchar *>(const_cast<char *>(ba.constData()));
    const uchar *ba_end = cursor + ba.length();
    const ushort *src = reinterpret_cast<const ushort *>(s.constBegin());
    const ushort *const end = reinterpret_cast<const ushort *>(s.constEnd());

    while (src != end) {
        if (cursor >= ba_end - 6) {
            // ensure we have enough space
            int pos = cursor - (const uchar *)ba.constData();
            ba.resize(ba.size()*2);
            cursor = (uchar *)ba.data() + pos;
            ba_end = (const uchar *)ba.constData() + ba.length();
        }

        uint u = *src++;
        if (u < 0x80) {
            if (u < 0x20 || u == 0x22 || u == 0x5c) {
                *cursor++ = '\\';
                switch (u) {
                case 0x22:
                    *cursor++ = '"';
                    break;
                case 0x5c:
                    *cursor++ = '\\';
                    break;
                case 0x8:
                    *cursor++ = 'b';
                    break;
                case 0xc:
                    *cursor++ = 'f';
                    break;
                case 0xa:
                    *cursor++ = 'n';
                    break;
                case 0xd:
                    *cursor++ = 'r';
                    break;
                case 0x9:
                    *cursor++ = 't';
                    break;
                default:
                    *cursor++ = 'u';
                    *cursor++ = '0';
                    *cursor++ = '0';
                    *cursor++ = hexdig(u>>4);
                    *cursor++ = hexdig(u & 0xf);
               }
            } else {
                *cursor++ = (uchar)u;
            }
        } else {
            if (toUtf8<QUtf8BaseTraits>(u, cursor, src, end) < 0)
                *cursor++ = replacement;
        }
    }

    ba.resize(cursor - (const uchar *)ba.constData());
    return ba;
}

void updateAllEditLineVisible(NetworkBaseEditLine *editLine)
{
    for(NetworkBaseEditLine *line : editLine->parent()->findChildren<NetworkBaseEditLine*>()) {
        line->updateVisible();
    }
}

NetworkBaseEditLine::NetworkBaseEditLine(const QString &section, const QString &key,
                                         DBusConnectionSession *dbus, const QString &title,
                                         bool alwaysUpdate,
                                         QWidget *parent):
    QWidget(parent),
    m_dbus(dbus),
    m_section(section),
    m_key(key),
    m_alwaysUpdate(alwaysUpdate)
{
    QHBoxLayout *layout = new QHBoxLayout;
    titleLabel = new DLabel(title);

    titleLabel->setWordWrap(true);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    layout->setContentsMargins(15, 0, 15, 0);
    layout->setSpacing(15);
    layout->addWidget(titleLabel);

    setLayout(layout);
    updateVisible();

    if(!firstBaseEditLine) {
        firstBaseEditLine = this;

        availableSections = m_dbus->availableSections();
        availableKeys = m_dbus->availableKeys();
        errors = m_dbus->errors();

        connect(dbus, &DBusConnectionSession::AvailableSectionsChanged, this, [this] {
            availableSections = m_dbus->availableSections();
            updateAllEditLineVisible(this);
        });

        connect(dbus, &DBusConnectionSession::AvailableKeysChanged, this, [this] {
            availableKeys = m_dbus->availableKeys();
            updateAllEditLineVisible(this);
        });

        connect(dbus, &DBusConnectionSession::ErrorsChanged, this, [this] {
            errors = m_dbus->errors();
        });
    }

    connect(this, &NetworkBaseEditLine::setTitle, titleLabel, &DLabel::setText);
    connect(dbus, &DBusConnectionSession::ConnectionDataChanged, this, [this]{
        if(isVisible() && this->alwaysUpdate()) {
            setCacheValue(dbusKey());
        }
    });

    setFixedWidth(DCC::ModuleContentWidth);
}

QString NetworkBaseEditLine::section() const
{
    return m_section;
}

QString NetworkBaseEditLine::key() const
{
    return m_key;
}

QJsonValue NetworkBaseEditLine::cacheValue() const
{
    return m_cacheValue;
}

bool NetworkBaseEditLine::alwaysUpdate() const
{
    return m_alwaysUpdate;
}

bool NetworkBaseEditLine::setKeyAlways() const
{
    return m_setKeyAlways;
}

bool NetworkBaseEditLine::readOnly() const
{
    return m_readOnly;
}

void NetworkBaseEditLine::setDBusKey(const QJsonValue &key)
{
    if (key != cacheValue() || isValueError() || setKeyAlways()) {
        QString json;

        if(key.isString()) {
            json.append("\"" + jsonEscapedString(key.toString()) + "\"");
        } else if(key.isDouble()) {
            json.append(QString::number(key.toDouble(-1)));
        } else if(key.isBool()) {
            json.append(key.toBool() ? "true" : "false");
        }
        /// TODO 此处将key转换成json的value类型，由于Qt json目前并不支持此类型，故需手动转换
        /// 目前只处理了String int double bool类型，以后有新类型的需求记得在此处添加
        qDebug() << "setDBusKey: " << json;

        setCacheValue(key);

        m_tempValue = key;

        if(m_updateKeying) {
            return;
        }

        m_updateKeying = true;

        ASYN_CALL(m_dbus->SetKey(section(), this->key(), json), {
                      m_updateKeying = false;

                      if(key != m_tempValue) {
                          bool key_always = setKeyAlways();

                          setSetKeyAlways(true);
                          setDBusKey(m_tempValue);
                          setSetKeyAlways(key_always);
                      }
                  }, this, key)
    }
}

const QJsonValue NetworkBaseEditLine::dbusKey()
{
    const QString &json = m_dbus->GetKey(section(), key());
    const QJsonDocument &json_doc = QJsonDocument::fromJson("{\"value\":" + json.toUtf8() + "}");

    return json_doc.object()["value"];
}

const QJsonArray NetworkBaseEditLine::getAvailableValues()
{
    const QString &json = m_dbus->GetAvailableValues(section(), key());
    const QJsonDocument &json_doc = QJsonDocument::fromJson(json.toUtf8());

    return json_doc.array();
}

const QList<QJsonValue> NetworkBaseEditLine::getAvailableValuesValue()
{
    QList<QJsonValue> list;
    const QJsonArray &array = getAvailableValues();

    if(array.isEmpty()) {
        return list;
    }

    for(const QJsonValue &value : array) {
        list << value.toObject()["Value"];
    }

    return list;
}

const QString NetworkBaseEditLine::getAvailableValuesTextByValue()
{
    const QJsonArray &array = getAvailableValues();

    if(!array.isEmpty() && !cacheValue().isNull()) {
        for(const QJsonValue &value : array) {
            const QJsonObject &obj = value.toObject();
            if(obj["Value"] == cacheValue()) {
                return obj["Text"].toString();
            }
        }
    }

    return "";
}

const QStringList NetworkBaseEditLine::getAvailableValuesText()
{
    const QJsonArray &array = getAvailableValues();

    QStringList list;

    if(array.isEmpty()) {
        return list;
    }

    for(const QJsonValue &value : array) {
        list << value.toObject()["Text"].toString();
    }

    return list;
}

int NetworkBaseEditLine::getAvailableValuesIndex()
{
    const QJsonArray &array = getAvailableValues();

    if(!array.isEmpty() && !cacheValue().isNull()) {
        for(int i = 0; i < array.count(); ++i) {
            const QJsonObject &obj = array[i].toObject();
            if(obj["Value"] == cacheValue()) {
                return i;
            }
        }
    }

    return -1;
}

void NetworkBaseEditLine::setRightWidget(QWidget *widget)
{
    QBoxLayout *layout = qobject_cast<QBoxLayout*>(this->layout());
    layout->addWidget(widget, 0, Qt::AlignRight);
}

void NetworkBaseEditLine::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);

    if(m_cacheValue.isNull() || alwaysUpdate()) {
        setCacheValue(dbusKey());
    } else {
        if(m_cacheValue != dbusKey()) {
            bool setKeyAlways = this->setKeyAlways();

            setSetKeyAlways(true);
            setDBusKey(m_cacheValue);
            setSetKeyAlways(setKeyAlways);
        }

        emit widgetShown();
    }

    emit showed();
}

void NetworkBaseEditLine::changeEvent(QEvent *e)
{
    if(!isVisible() && e->type() == QEvent::ParentChange) {
        updateVisible();
    }
}

void NetworkBaseEditLine::setCacheValue(const QJsonValue &value)
{
    if(m_cacheValue == value)
        return;

    m_cacheValue = value;

    emit cacheValueChanged();
}

void NetworkBaseEditLine::updateVisible()
{
    if(parentWidget()) {
        bool visible = availableSections.indexOf(section()) != -1
                &&  availableKeys[section()].indexOf(key()) != -1;
        setVisible(visible);
    }
}

void NetworkBaseEditLine::setDBusKey(const QString &key)
{
    setDBusKey(QJsonValue(key));
}

void NetworkBaseEditLine::setSection(const QString &section)
{
    m_section = section;
}

void NetworkBaseEditLine::setKey(const QString &key)
{
    m_key = key;
}

void NetworkBaseEditLine::setAlwaysUpdate(bool alwaysUpdate)
{
    m_alwaysUpdate = alwaysUpdate;
}

void NetworkBaseEditLine::setSetKeyAlways(bool setKeyAlways)
{
    m_setKeyAlways = setKeyAlways;
}

void NetworkBaseEditLine::setReadOnly(bool readOnly)
{
    if (m_readOnly == readOnly)
        return;

    m_readOnly = readOnly;
    emit readOnlyChanged(readOnly);
}

bool NetworkBaseEditLine::checkKey()
{
    if(isValueError()) {
        emit showErrorAlert();

        qDebug() << "key error: " << section() << key() << cacheValue() << titleLabel->text();

        return false;
    }

    return true;
}

bool NetworkBaseEditLine::isValueError() const
{
    return !errors.isEmpty() && !errors[section()].isEmpty()
            && !errors[section()][key()].isEmpty();
}
