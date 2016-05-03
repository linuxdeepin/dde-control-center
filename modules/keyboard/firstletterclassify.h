/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef FIRSTLETTERCLASSIFY_H
#define FIRSTLETTERCLASSIFY_H

#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>

#include <dwidget_global.h>
#include <dsegmentedcontrol.h>

#include "listwidget.h"

DWIDGET_USE_NAMESPACE

class MultiAddCheckButton;

class KeyboardLayoutDelegate: public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)

public:
    explicit KeyboardLayoutDelegate(const QString &title, QWidget *parent = 0);

    QStringList keyWords();
    void setKeyWords(const QStringList &keywords);
    QString title() const;
    bool checked() const;

public slots:
    void setTitle(const QString &title);
    void setChecked(bool checked);

protected:
    void mouseReleaseEvent(QMouseEvent *e);

signals:
    void checkedChanged(bool checked);
    void getKeyWordsFinished();

private:
    QHBoxLayout *m_layout;
    QLabel *m_label;
    MultiAddCheckButton *m_checkButton;
    bool m_checked;
    QStringList m_pinyinFirstLetterList;
};

class FirstLetterClassify : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QString currentLetter READ currentLetter WRITE setCurrentLetter NOTIFY currentLetterChanged)
public:
    explicit FirstLetterClassify(QWidget *parent = 0);
    ~FirstLetterClassify();

    ListWidget *searchList() const;
    DSegmentedControl *letterList() const;
    void addItem(KeyboardLayoutDelegate *item, const QChar letterFirst);
    void removeItems(QList<KeyboardLayoutDelegate *> datas);
    QString currentLetter() const;

public slots:
    void setCurrentLetter(QString currentLetter);
    void show();

signals:
    void currentLetterChanged(QString currentLetter);

private:
    QVBoxLayout *m_layout;
    DSegmentedControl *m_letterList;
    ListWidget *m_currentList = NULL;
    QString m_currentLetter;
    QList<ListWidget *> m_listWidgetList;
};

#endif // FIRSTLETTERCLASSIFY_H
