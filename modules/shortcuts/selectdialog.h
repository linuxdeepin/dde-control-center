#ifndef SELECTDIALOG_H
#define SELECTDIALOG_H

#include <QFrame>
#include <QLabel>
#include <QPropertyAnimation>

#include "searchlist.h"

class SelectDialog : public QFrame, public SearchItem
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE setText)

public:
    explicit SelectDialog(QWidget *parent = 0);

    QStringList keyWords() const Q_DECL_OVERRIDE;
    void setData(const QVariant &datas) Q_DECL_OVERRIDE;
    QVariant getData() Q_DECL_OVERRIDE;
    QWidget *widget() const Q_DECL_OVERRIDE;
    QString text() const;

public slots:
    void setText(const QString &text);
    void expansion();
    void contraction();

signals:
    void expanded();
    void contracted();
    void replace();
    void cancel();

private:
    QWidget *m_widget;
    QLabel *m_label;
    QPropertyAnimation *m_animation;
};

#endif // SELECTDIALOG_H
