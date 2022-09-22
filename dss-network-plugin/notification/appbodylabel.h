// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPBODYLABEL_H
#define APPBODYLABEL_H

#include <QFrame>
#include <DLabel>

DWIDGET_USE_NAMESPACE
/*!
 * \~chinese \class AppBodyLabel
 * \~chinese \brief 此类用于通知中心气泡Body中的Label控件的实现
 */
class AppBodyLabel : public DLabel
{
    Q_OBJECT

public:
    explicit AppBodyLabel(QWidget *parent = nullptr);
    void setText(const QString &text);
    void setOpacity(qreal opacity) { m_opacity = opacity; }
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void setAlignment(Qt::Alignment alignment);

#ifdef GTEST
    QString labelText() const { return m_text; }
    qreal bodyOpacity() const { return m_opacity; }
    Qt::Alignment bodyAlignment() const { return m_alignment; }
#endif

private:
    const QString holdTextInRect(const QFontMetrics &fm, const QString &text, const QRect &rect) const;
    void resizeEvent(QResizeEvent *e) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void updateLineCount();

    qreal m_opacity = 1.0;
    QString m_text;
    int m_lineCount = 0;
    Qt::Alignment m_alignment;
};

#endif // APPBODYLABEL_H
