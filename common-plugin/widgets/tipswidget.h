#ifndef TIPSWIDGET_H
#define TIPSWIDGET_H

#include "../utils.h"

#include <QFrame>

NETWORKPLUGIN_BEGIN_NAMESPACE

class TipsWidget : public QFrame
{
    Q_OBJECT

public:
    explicit TipsWidget(QWidget *parent = nullptr);

    void setContext(const QList<QPair<QString, QStringList>> &textList);
    void setSpliter(const QString &spliter);

protected:
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent *event) override;
    int calcValueX();

private:
    QList<QPair<QString, QStringList>> m_textList;
    QString m_spliter;
};

NETWORKPLUGIN_END_NAMESPACE

#endif // TIPSWIDGET_H
