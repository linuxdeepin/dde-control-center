#ifndef DYNAMICLABEL_H
#define DYNAMICLABEL_H

#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>

class DynamicLabel : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(int duration READ duration WRITE setDuration)
    Q_PROPERTY(QEasingCurve::Type easingType READ easingType WRITE setEasingType)

public:
    explicit DynamicLabel(QWidget *parent = 0);

    QString text() const;
    QLabel *label() const;
    QColor color() const;
    int duration() const;
    QEasingCurve::Type easingType() const;
    QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
    void setText(const QString &text);
    void showLabel();
    void hideLabel();
    void delayShowLabel(int duration);
    void delayHideLabel(int duration);
    void setColor(QColor color);
    void setDuration(int duration);
    void setEasingType(QEasingCurve::Type easingType);
    void setDisappearDuration(int duration);

signals:
    void hideFinished();

protected:
    void resizeEvent(QResizeEvent *e);

private:
    QLabel *m_label;
    QPropertyAnimation *m_animation;
    int m_timeout;
    QTimer m_delayTimer;
};

#endif // DYNAMICLABEL_H
