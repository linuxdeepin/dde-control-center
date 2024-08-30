//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef FONTMODEL_H
#define FONTMODEL_H

#include <QObject>
#include <QList>
#include <QJsonObject>


class FontModel : public QObject
{
    Q_OBJECT
public:
    explicit FontModel(QObject *parent = 0);
    void setFontList(const QList<QJsonObject> &list);
    void setFontName(const QString &name);
    inline const QList<QJsonObject> getFontList() const {return m_list;}
    inline const QString getFontName() const {return m_fontName;}

Q_SIGNALS:
    void listChanged(const QList<QJsonObject> &list);
    void defaultFontChanged(const QString &name);

private:
    QList<QJsonObject> m_list;
    QString m_fontName;
};

Q_DECLARE_METATYPE(FontModel*)
#endif // FONTMODEL_H
