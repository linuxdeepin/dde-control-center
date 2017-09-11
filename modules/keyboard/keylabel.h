#ifndef KEYLABEL_H
#define KEYLABEL_H

#include <QLabel>

class KeyLabel : public QLabel
{
    Q_OBJECT
public:
    explicit KeyLabel(QWidget *parent = nullptr);
    explicit KeyLabel(const QString &text, QWidget *parent = nullptr);

    void setEnter(const bool enter);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    bool m_isEnter;
};

#endif // KEYLABEL_H
