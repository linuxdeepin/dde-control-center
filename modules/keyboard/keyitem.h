#ifndef KEYITEM_H
#define KEYITEM_H

#include <QObject>
#include <QRect>
#include <QPainter>

namespace dcc {
namespace keyboard{

class KeyItem
{
public:
    explicit KeyItem(int row, int col, const QString& main, const QString& vice = QString());
    explicit KeyItem(int row, int col, int key, const QString& main, const QString& vice = QString());
    virtual ~KeyItem();
    void setMainKey(const QString& key);
    QString mainKey() const;

    void setViceKey(const QString& key);
    QString viceKey() const;

    void setPress(bool press);
    bool press() const { return m_press; }

    void setConflict(bool conflict);

    QRect rect() const { return m_rect; }
    quint32 keycode() const { return m_keycode;}
    bool modifies() const { return m_modifies;}

    static QList<KeyItem*> keyboards();
    static void deleteItems();
public:
    virtual void paint(QPainter* painter, const QRect& rect);

private:
    QString m_mainKey;
    QString m_viceKey;
    QPixmap m_mainPix;
    int m_row;
    int m_col;
    quint32 m_keycode;
    QRect m_rect;
    bool m_modifies;
    bool m_press;
    static QList<KeyItem*> m_keys;
    bool m_conflict;
};
}
}

#endif // KEYITEM_H
