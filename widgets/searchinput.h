#ifndef SEARCHINPUT_H
#define SEARCHINPUT_H

#include <QLineEdit>

namespace dcc {

class SearchInput : public QLineEdit
{
    Q_OBJECT

public:
    explicit SearchInput(QWidget* parent = 0);
    void setSearchText(const QString& text);

protected:
    void paintEvent(QPaintEvent *);

private:
    QString m_search;
    QPixmap m_icon;
};

}

#endif // SEARCHINPUT_H
