#ifndef DYNAMICLABEL_H
#define DYNAMICLABEL_H

#include <QLabel>
#include <QPropertyAnimation>

class DynamicLabel : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(int duration READ duration WRITE setDuration)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout)
    Q_PROPERTY(QEasingCurve::Type easingType READ easingType WRITE setEasingType)
public:
    explicit DynamicLabel(QWidget *parent = 0);

    QLabel *label() const;
    QColor color() const;
    int duration() const;
    int timeout() const;
    QEasingCurve::Type easingType() const;

public slots:
    void setText(const QString &text);
    void showLabel();
    void hideLabel();
    void setColor(QColor color);
    void setDuration(int duration);
    void setTimeout(int timeout);
    void setEasingType(QEasingCurve::Type easingType);

private:
    QLabel *m_label;
    QPropertyAnimation *m_animation;
    int m_timeout;
};

#endif // DYNAMICLABEL_H
