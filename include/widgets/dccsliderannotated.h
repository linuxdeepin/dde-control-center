// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DCCSLIDERANNOTATED_H
#define DCCSLIDERANNOTATED_H

#include <QFrame>
#include <QSlider>


QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {

class DCCSlider;

class SliderAnnotation : public QWidget
{
    Q_OBJECT
public:
    explicit SliderAnnotation(QWidget *parent = nullptr);
    virtual ~SliderAnnotation() {}

    void setContents(const QStringList &contents);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};

class DCCSliderAnnotated : public QFrame
{
    Q_OBJECT
public:
    explicit DCCSliderAnnotated(QWidget *parent = nullptr);
    virtual ~DCCSliderAnnotated();

    void setAnnotations(const QStringList &annotations);

#ifdef WINDOW_MODE
    DCCSlider *slider() const;
    void setSlider(DCCSlider *slider);
#else
    DCCSlider *slider() const;
    void setSlider(DCCSlider *slider);
#endif
    QString leftIcon() const;
    void setLeftIcon(const QString &leftIcon);
    void setLeftIcon(const QIcon &leftIcon, const QSize &iconSize);

    QString rightIcon() const;
    void setRightIcon(const QString &rightIcon);
    void setRightIcon(const QIcon &leftIcon, const QSize &iconSize);


Q_SIGNALS:
    void leftIconChanged();
    void rightIconChanged();

protected:
    bool event(QEvent *evnet) override;
private:
    DCCSlider *m_slider;
    SliderAnnotation *m_annotation;

    QLabel *m_leftIconLabel;
    QLabel *m_rightIconLabel;

    QString m_leftIcon;
    QString m_rightIcon;
};

} // namespace widgets
} // namespace dcc

#endif // DCCSLIDERANNOTATED_H
