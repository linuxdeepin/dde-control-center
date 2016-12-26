#ifndef QUICKSWITCHBUTTON_H
#define QUICKSWITCHBUTTON_H

#include <QLabel>

namespace dcc {

class QuickSwitchButton : public QLabel
{
    Q_OBJECT
public:
    explicit QuickSwitchButton(const int index, const QString &iconName, QWidget *parent = 0);

    Q_PROPERTY(QString themeName READ themeName WRITE setThemeName)

    QString themeName() const;
    void setThemeName(const QString &themeName);

signals:
    void hovered(const int index) const;
    void clicked(const int index) const;

protected:
    void mouseReleaseEvent(QMouseEvent *e);

private:
    QPixmap normalPixmap() const;
    QPixmap hoverPixmap() const;
    QPixmap pressedPixmap() const;

private:
    const int m_index;

    bool m_checked;
    QString m_themeName;
    const QString m_iconName;
};

}

#endif // QUICKSWITCHBUTTON_H
