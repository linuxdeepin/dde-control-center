#ifndef SEARCHINPUT_H
#define SEARCHINPUT_H

#include <QLineEdit>

namespace dcc {
namespace widgets {

class SearchInput : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)
public:
    explicit SearchInput(QWidget* parent = 0);
    void setSearchText(const QString& text);
    void setIconVisible(bool visible);
    QPixmap pixmap() const;
    void setPixmap(const QPixmap &pixmap);

protected:
    void paintEvent(QPaintEvent *);

private:
    bool m_iconVisible;
    QString m_search;
    QPixmap m_icon;
};

}
}

#endif // SEARCHINPUT_H
