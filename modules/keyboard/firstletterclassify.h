#ifndef FIRSTLETTERCLASSIFY_H
#define FIRSTLETTERCLASSIFY_H

#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>

#include <libdui/libdui_global.h>
#include <libdui/dsegmentedcontrol.h>

#include "searchlist.h"

DUI_USE_NAMESPACE

class MultiAddCheckButton;

class KeyboardLayoutDelegate: public QFrame, public SearchItem
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)

public:
    explicit KeyboardLayoutDelegate(const QString &title, QWidget *parent = 0);

    QStringList keyWords() const Q_DECL_OVERRIDE;
    void setData(const QVariant &datas) Q_DECL_OVERRIDE;
    QVariant getData() Q_DECL_OVERRIDE;
    QWidget *widget() const Q_DECL_OVERRIDE;
    QString title() const;
    bool checked() const;

public slots:
    void setTitle(const QString &title);
    void setChecked(bool checked);

signals:
    void checkedChanged(bool checked);

private:
    QWidget *m_widget;
    QHBoxLayout *m_layout;
    QLabel *m_label;
    MultiAddCheckButton *m_checkButton;
};

class FirstLetterClassify : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QString currentLetter READ currentLetter WRITE setCurrentLetter NOTIFY currentLetterChanged)
public:
    explicit FirstLetterClassify(QWidget *parent = 0);
    SearchList *searchList() const;
    DSegmentedControl *letterList() const;
    void addItems(QList<SearchItem*> datas);
    void removeItems(QList<SearchItem*> datas);
    QString currentLetter() const;

public slots:
    void setCurrentLetter(QString currentLetter);

signals:
    void currentLetterChanged(QString currentLetter);

private:
    QVBoxLayout *m_layout;
    DSegmentedControl *m_letterList;
    SearchList *m_searchList;
    QString m_currentLetter;
    QMap<QChar, bool> m_mapLetters;
};

#endif // FIRSTLETTERCLASSIFY_H
