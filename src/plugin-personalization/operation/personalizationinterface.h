//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef PERSIONALIZATIONINTERFACE_H
#define PERSIONALIZATIONINTERFACE_H

#include <QAbstractItemModel>

#include "personalizationworker.h"
#include "personalizationmodel.h"
#include "imagehelper.h"

class ThemeVieweModel : public QAbstractItemModel
{
public:
    enum UserDataRole {
        IdRole = Qt::UserRole + 0x101,
        PicRole
    };
    explicit ThemeVieweModel(QObject *parent = nullptr);
    ~ThemeVieweModel() { }

    void setThemeModel(ThemeModel *model);
    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    void updateData();

private:
    ThemeModel *m_themeModel;
    QStringList m_keys;
};

class PersonalizationInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ThemeVieweModel *globalThemeModel MEMBER m_globalThemeViewModel CONSTANT)
    Q_PROPERTY(ThemeVieweModel *iconThemeViewModel MEMBER m_iconThemeViewModel CONSTANT)
    Q_PROPERTY(ThemeVieweModel *cursorThemeViewModel MEMBER m_cursorThemeViewModel CONSTANT)
    Q_PROPERTY(PersonalizationModel *model MEMBER m_model CONSTANT)
    Q_PROPERTY(PersonalizationWorker *worker MEMBER m_work CONSTANT)
    Q_PROPERTY(ImageHelper *imageHelper MEMBER m_imageHelper CONSTANT)
    Q_PROPERTY(QVariantList appearanceSwitchModel MEMBER m_appearanceSwitchModel NOTIFY appearanceSwitchModelChanged)
    Q_PROPERTY(QString currentAppearance MEMBER m_currentAppearance NOTIFY currentAppearanceChanged)

public:
    explicit PersonalizationInterface(QObject *parent = nullptr);

    void setCurrentAppearance(const QString &appearance);
    QString getCurrentAppearance() const { return m_currentAppearance; };

private:
    void initAppearanceSwitchModel();

signals:
    void currentAppearanceChanged(const QString &appearance);
    void appearanceSwitchModelChanged(const QVariantList &model);

private:
    PersonalizationModel *m_model;
    PersonalizationWorker *m_work;
    ImageHelper *m_imageHelper;
    ThemeVieweModel *m_globalThemeViewModel;
    ThemeVieweModel *m_iconThemeViewModel;
    ThemeVieweModel *m_cursorThemeViewModel;

    QVariantList m_appearanceSwitchModel;
    QString m_currentAppearance;
};

#endif // PERSIONALIZATIONINTERFACE_H