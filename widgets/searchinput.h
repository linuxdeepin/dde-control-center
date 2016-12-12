#ifndef SEARCHINPUT_H
#define SEARCHINPUT_H

#include <QLineEdit>

namespace dcc {
namespace widgets {

class SearchInput : public QLineEdit
{
    Q_OBJECT

public:
    explicit SearchInput(QWidget* parent = 0);
    void setSearchText(const QString& text);
    void setIconVisible(bool visible);

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
