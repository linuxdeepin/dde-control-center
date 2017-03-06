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

    bool checked() const { return m_checked; }

signals:
    void hovered(const int index) const;
    void clicked(const int index) const;
    void checkedChanged(const bool checked) const;

public slots:
    void setChecked(const bool checked);
    void setCheckable(const bool checkable);
    void setSelected(const bool selected);
    void setBackgroundVisible(const bool visible);

protected:
    void mouseReleaseEvent(QMouseEvent *e);
    void enterEvent(QEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    QPixmap normalPixmap() const;
    QPixmap hoverPixmap() const;
    QPixmap pressedPixmap() const;

private:
    const int m_index;

    bool m_selected;
    bool m_checked;
    bool m_checkable;
    bool m_showBackground;
    QString m_themeName;
    const QString m_iconName;
};

}

#endif //
