#ifndef FONTMODEL_H
#define FONTMODEL_H

#include <QObject>
#include <QList>
#include <QJsonObject>

namespace dcc
{
namespace personalization
{
class FontModel : public QObject
{
    Q_OBJECT
public:
    explicit FontModel(QObject *parent = 0);
    void setFontList(const QList<QJsonObject> &list);
    void setFontName(const QString &name);
    inline const QList<QJsonObject> getFontList() const {return m_list;}
    inline const QString getFontName() const {return m_fontName;}

signals:
    void listChanged(const QList<QJsonObject> &list);
    void defaultFontChanged(const QString &name);

private:
    QList<QJsonObject> m_list;
    QString m_fontName;
};
}
}


#endif // FONTMODEL_H
