// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "categorymodel.h"
#include "category.h"

#include <gtest/gtest.h>
#include <QSignalSpy>
#include <QFileInfo>
#include <QUrl>
#include <QHash>
#include <QMetaType>
#include <QList>
#include <QVariant>
#include <QModelIndex>

// App and QFileInfo are used as Qt signal arguments. QSignalSpy requires
// the types be declared as metatypes and registered before the first
// connection.
Q_DECLARE_METATYPE(App)

namespace {

struct AppMetaTypeRegistration {
    AppMetaTypeRegistration() {
        qRegisterMetaType<App>("App");
        qRegisterMetaType<QFileInfo>("QFileInfo");
    }
};
const AppMetaTypeRegistration g_registerAppMetaType;

// Role constants matching the anonymous enum in categorymodel.cpp:
//   DefAppIsUserRole = Qt::UserRole << (2 + 1)  →  Qt::UserRole << 3
constexpr int kDefAppIsUserRole    = Qt::UserRole << 3;
constexpr int kDefAppIdRole        = kDefAppIsUserRole + 1;
constexpr int kDefAppCanDeleteRole = kDefAppIsUserRole + 2;
constexpr int kDefAppNameRole      = kDefAppIsUserRole + 3;
constexpr int kDefAppIconRole      = kDefAppIsUserRole + 4;
constexpr int kDefAppIsDefaultRole = kDefAppIsUserRole + 5;

App makeApp(const QString &id, bool isUser, const QString &name = QString(),
            bool canDelete = false, const QString &displayName = QString(),
            const QString &icon = QString())
{
    App a;
    a.Id = id;
    a.Name = name;
    a.DisplayName = displayName;
    a.isUser = isUser;
    a.CanDelete = canDelete;
    a.Icon = icon;
    return a;
}

} // namespace

// ---- Constructor ----

TEST(CategoryModel, ConstructorWithEmptyCategory)
{
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    EXPECT_EQ(model->rowCount(), 0);
    EXPECT_EQ(model->columnCount(), 1);
    // model is parented to cat and deleted by cat's destructor
}

TEST(CategoryModel, ConstructorPopulatesFromCategory)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("App1")));
    cat.addUserItem(makeApp(QStringLiteral("a2"), false, QStringLiteral("App2")));
    CategoryModel *model = new CategoryModel(&cat);
    EXPECT_EQ(model->rowCount(), 2);
}

TEST(CategoryModel, CategoryAccessor)
{
    Category cat;
    cat.setCategory(QStringLiteral("Browser"));
    CategoryModel *model = new CategoryModel(&cat);
    EXPECT_EQ(model->category(), &cat);
}

// ---- roleNames ----

TEST(CategoryModel, RoleNamesContainsCustomRoles)
{
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    QHash<int, QByteArray> names = model->roleNames();
    EXPECT_STREQ(names.value(kDefAppIsUserRole).constData(), "isUser");
    EXPECT_STREQ(names.value(kDefAppIdRole).constData(), "id");
    EXPECT_STREQ(names.value(kDefAppCanDeleteRole).constData(), "canDelete");
    EXPECT_STREQ(names.value(kDefAppNameRole).constData(), "name");
    EXPECT_STREQ(names.value(kDefAppIconRole).constData(), "icon");
    EXPECT_STREQ(names.value(kDefAppIsDefaultRole).constData(), "isDefault");
    // Base roles preserved.
    EXPECT_STREQ(names.value(Qt::DisplayRole).constData(), "display");
}

// ---- index ----

TEST(CategoryModel, IndexValidRow)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("B")));
    cat.addUserItem(makeApp(QStringLiteral("a2"), false, QStringLiteral("A")));
    CategoryModel *model = new CategoryModel(&cat);
    QModelIndex idx = model->index(0, 0);
    EXPECT_TRUE(idx.isValid());
    EXPECT_EQ(idx.row(), 0);
    EXPECT_EQ(idx.column(), 0);
    QModelIndex idx1 = model->index(1, 0);
    EXPECT_TRUE(idx1.isValid());
    EXPECT_EQ(idx1.row(), 1);
}

TEST(CategoryModel, IndexNegativeRowReturnsInvalid)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false));
    CategoryModel *model = new CategoryModel(&cat);
    EXPECT_FALSE(model->index(-1, 0).isValid());
}

TEST(CategoryModel, IndexOutOfRangeRowReturnsInvalid)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false));
    CategoryModel *model = new CategoryModel(&cat);
    EXPECT_FALSE(model->index(1, 0).isValid());
    EXPECT_FALSE(model->index(100, 0).isValid());
}

// ---- parent ----

TEST(CategoryModel, ParentAlwaysReturnsInvalid)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false));
    CategoryModel *model = new CategoryModel(&cat);
    QModelIndex idx = model->index(0, 0);
    EXPECT_FALSE(model->parent(idx).isValid());
    EXPECT_FALSE(model->parent(QModelIndex()).isValid());
}

// ---- rowCount / columnCount ----

TEST(CategoryModel, RowCountMatchesAppList)
{
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    EXPECT_EQ(model->rowCount(), 0);
    cat.addUserItem(makeApp(QStringLiteral("a1"), false));
    EXPECT_EQ(model->rowCount(), 1);
    cat.addUserItem(makeApp(QStringLiteral("a2"), false));
    EXPECT_EQ(model->rowCount(), 2);
}

TEST(CategoryModel, ColumnCountAlwaysOne)
{
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    EXPECT_EQ(model->columnCount(), 1);
    EXPECT_EQ(model->columnCount(QModelIndex()), 1);
    cat.addUserItem(makeApp(QStringLiteral("a1"), false));
    EXPECT_EQ(model->columnCount(model->index(0, 0)), 1);
}

// ---- data ----

TEST(CategoryModel, DataInvalidIndexReturnsInvalid)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false));
    CategoryModel *model = new CategoryModel(&cat);
    QVariant result = model->data(QModelIndex());
    // Source returns QModelIndex() which converts to QVariant wrapping an
    // invalid model index.
    EXPECT_FALSE(result.toModelIndex().isValid());
}

TEST(CategoryModel, DataOutOfRangeRowReturnsInvalid)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false));
    CategoryModel *model = new CategoryModel(&cat);
    QModelIndex outOfRange = model->index(5, 0); // invalid per index()
    QVariant result = model->data(outOfRange);
    EXPECT_FALSE(result.toModelIndex().isValid());
}

TEST(CategoryModel, DataDisplayRoleReturnsDisplayName)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("Name"),
                            false, QStringLiteral("Display Name")));
    CategoryModel *model = new CategoryModel(&cat);
    QModelIndex idx = model->index(0, 0);
    EXPECT_EQ(model->data(idx, Qt::DisplayRole).toString(), QStringLiteral("Display Name"));
}

TEST(CategoryModel, DataIsUserRoleReturnsIsUser)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), true));
    cat.addUserItem(makeApp(QStringLiteral("a2"), false));
    CategoryModel *model = new CategoryModel(&cat);
    // Find the user app and the system app in the model.
    bool foundUser = false, foundSystem = false;
    for (int i = 0; i < model->rowCount(); ++i) {
        QModelIndex idx = model->index(i, 0);
        if (model->data(idx, kDefAppIdRole).toString() == QStringLiteral("a1")) {
            EXPECT_TRUE(model->data(idx, kDefAppIsUserRole).toBool());
            foundUser = true;
        }
        if (model->data(idx, kDefAppIdRole).toString() == QStringLiteral("a2")) {
            EXPECT_FALSE(model->data(idx, kDefAppIsUserRole).toBool());
            foundSystem = true;
        }
    }
    EXPECT_TRUE(foundUser);
    EXPECT_TRUE(foundSystem);
}

TEST(CategoryModel, DataIdRoleReturnsId)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("my-id"), false));
    CategoryModel *model = new CategoryModel(&cat);
    QModelIndex idx = model->index(0, 0);
    EXPECT_EQ(model->data(idx, kDefAppIdRole).toString(), QStringLiteral("my-id"));
}

TEST(CategoryModel, DataCanDeleteRoleReturnsCanDelete)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QString(), true));
    cat.addUserItem(makeApp(QStringLiteral("a2"), false, QString(), false));
    CategoryModel *model = new CategoryModel(&cat);
    bool foundTrue = false, foundFalse = false;
    for (int i = 0; i < model->rowCount(); ++i) {
        QModelIndex idx = model->index(i, 0);
        QString id = model->data(idx, kDefAppIdRole).toString();
        bool canDelete = model->data(idx, kDefAppCanDeleteRole).toBool();
        if (id == QStringLiteral("a1")) {
            EXPECT_TRUE(canDelete);
            foundTrue = true;
        }
        if (id == QStringLiteral("a2")) {
            EXPECT_FALSE(canDelete);
            foundFalse = true;
        }
    }
    EXPECT_TRUE(foundTrue);
    EXPECT_TRUE(foundFalse);
}

TEST(CategoryModel, DataNameRoleReturnsName)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("MyApp")));
    CategoryModel *model = new CategoryModel(&cat);
    QModelIndex idx = model->index(0, 0);
    EXPECT_EQ(model->data(idx, kDefAppNameRole).toString(), QStringLiteral("MyApp"));
}

TEST(CategoryModel, DataIconRoleReturnsIconWhenSet)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("A"),
                            false, QString(), QStringLiteral("my-icon")));
    CategoryModel *model = new CategoryModel(&cat);
    QModelIndex idx = model->index(0, 0);
    EXPECT_EQ(model->data(idx, kDefAppIconRole).toString(), QStringLiteral("my-icon"));
}

TEST(CategoryModel, DataIconRoleReturnsDefaultWhenEmpty)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("A"),
                            false, QString(), QString()));
    CategoryModel *model = new CategoryModel(&cat);
    QModelIndex idx = model->index(0, 0);
    EXPECT_EQ(model->data(idx, kDefAppIconRole).toString(), QStringLiteral("application-default-icon"));
}

TEST(CategoryModel, DataIconRoleReturnsDefaultWhenWhitespace)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("A"),
                            false, QString(), QStringLiteral("   ")));
    CategoryModel *model = new CategoryModel(&cat);
    QModelIndex idx = model->index(0, 0);
    EXPECT_EQ(model->data(idx, kDefAppIconRole).toString(), QStringLiteral("application-default-icon"));
}

TEST(CategoryModel, DataIsDefaultRoleWhenAppIsDefault)
{
    Category cat;
    App def = makeApp(QStringLiteral("a1"), false, QStringLiteral("A"));
    cat.addUserItem(def);
    cat.setDefault(def);
    CategoryModel *model = new CategoryModel(&cat);
    QModelIndex idx = model->index(0, 0);
    EXPECT_TRUE(model->data(idx, kDefAppIsDefaultRole).toBool());
}

TEST(CategoryModel, DataIsDefaultRoleWhenAppIsNotDefault)
{
    Category cat;
    App def = makeApp(QStringLiteral("a1"), false, QStringLiteral("A"));
    cat.addUserItem(def);
    cat.addUserItem(makeApp(QStringLiteral("a2"), false, QStringLiteral("B")));
    cat.setDefault(def);
    CategoryModel *model = new CategoryModel(&cat);
    bool foundDefault = false, foundNonDefault = false;
    for (int i = 0; i < model->rowCount(); ++i) {
        QModelIndex idx = model->index(i, 0);
        QString id = model->data(idx, kDefAppIdRole).toString();
        bool isDefault = model->data(idx, kDefAppIsDefaultRole).toBool();
        if (id == QStringLiteral("a1")) {
            EXPECT_TRUE(isDefault);
            foundDefault = true;
        }
        if (id == QStringLiteral("a2")) {
            EXPECT_FALSE(isDefault);
            foundNonDefault = true;
        }
    }
    EXPECT_TRUE(foundDefault);
    EXPECT_TRUE(foundNonDefault);
}

TEST(CategoryModel, DataUnknownRoleReturnsInvalidVariant)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false));
    CategoryModel *model = new CategoryModel(&cat);
    QModelIndex idx = model->index(0, 0);
    QVariant result = model->data(idx, Qt::UserRole + 9999);
    EXPECT_FALSE(result.isValid());
}

// ---- addApp ----

TEST(CategoryModel, AddAppEmptyPathEmitsNoSignal)
{
    Category cat;
    cat.setCategory(QStringLiteral("Browser"));
    CategoryModel *model = new CategoryModel(&cat);
    QSignalSpy spy(model, &CategoryModel::requestCreateFile);
    model->addApp(QString());
    EXPECT_EQ(spy.count(), 0);
}

TEST(CategoryModel, AddAppValidPathEmitsRequestCreateFile)
{
    Category cat;
    cat.setCategory(QStringLiteral("Browser"));
    CategoryModel *model = new CategoryModel(&cat);
    QSignalSpy spy(model, &CategoryModel::requestCreateFile);
    model->addApp(QStringLiteral("file:///usr/bin/firefox"));
    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.at(0).toString(), QStringLiteral("Browser"));
    QFileInfo info = qvariant_cast<QFileInfo>(args.at(1));
    EXPECT_EQ(info.filePath(), QStringLiteral("/usr/bin/firefox"));
}

TEST(CategoryModel, AddAppPlainPathEmitsSignalWithEmptyLocalFile)
{
    Category cat;
    cat.setCategory(QStringLiteral("Mail"));
    CategoryModel *model = new CategoryModel(&cat);
    QSignalSpy spy(model, &CategoryModel::requestCreateFile);
    // A plain path (no scheme) → QUrl::toLocalFile() returns empty.
    model->addApp(QStringLiteral("/usr/bin/test"));
    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.at(0).toString(), QStringLiteral("Mail"));
    QFileInfo info = qvariant_cast<QFileInfo>(args.at(1));
    EXPECT_EQ(info.filePath(), QString());
}

// ---- removeApp ----

TEST(CategoryModel, RemoveAppNotFoundEmitsNoSignal)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("A")));
    CategoryModel *model = new CategoryModel(&cat);
    QSignalSpy spy(model, &CategoryModel::requestDelUserApp);
    model->removeApp(QStringLiteral("nonexistent"));
    EXPECT_EQ(spy.count(), 0);
}

TEST(CategoryModel, RemoveAppInvalidIdEmitsNoSignal)
{
    Category cat;
    // Add an app with an empty Id — it will be in the category list but
    // isValid() returns false.
    App emptyId = makeApp(QString(), false, QStringLiteral("A"));
    cat.addUserItem(emptyId);
    CategoryModel *model = new CategoryModel(&cat);
    QSignalSpy spy(model, &CategoryModel::requestDelUserApp);
    model->removeApp(QString());
    EXPECT_EQ(spy.count(), 0);
}

TEST(CategoryModel, RemoveAppFoundEmitsRequestDelUserApp)
{
    Category cat;
    cat.setCategory(QStringLiteral("Browser"));
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("A")));
    CategoryModel *model = new CategoryModel(&cat);
    QSignalSpy spy(model, &CategoryModel::requestDelUserApp);
    model->removeApp(QStringLiteral("a1"));
    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.at(0).toString(), QStringLiteral("Browser"));
    EXPECT_EQ(args.at(1).value<App>().Id, QStringLiteral("a1"));
}

// ---- setDefaultApp ----

TEST(CategoryModel, SetDefaultAppNotFoundEmitsNoSignal)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false));
    CategoryModel *model = new CategoryModel(&cat);
    QSignalSpy spy(model, &CategoryModel::requestSetDefaultApp);
    model->setDefaultApp(QStringLiteral("nonexistent"));
    EXPECT_EQ(spy.count(), 0);
}

TEST(CategoryModel, SetDefaultAppInvalidIdEmitsNoSignal)
{
    Category cat;
    App emptyId = makeApp(QString(), false, QStringLiteral("A"));
    cat.addUserItem(emptyId);
    CategoryModel *model = new CategoryModel(&cat);
    QSignalSpy spy(model, &CategoryModel::requestSetDefaultApp);
    model->setDefaultApp(QString());
    EXPECT_EQ(spy.count(), 0);
}

TEST(CategoryModel, SetDefaultAppFoundEmitsRequestSetDefaultApp)
{
    Category cat;
    cat.setCategory(QStringLiteral("Mail"));
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("A")));
    CategoryModel *model = new CategoryModel(&cat);
    QSignalSpy spy(model, &CategoryModel::requestSetDefaultApp);
    model->setDefaultApp(QStringLiteral("a1"));
    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.at(0).toString(), QStringLiteral("Mail"));
    EXPECT_EQ(args.at(1).value<App>().Id, QStringLiteral("a1"));
}

// ---- onAddApp (via Category signal) ----

TEST(CategoryModel, OnAddAppViaCategorySignalUpdatesModel)
{
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    EXPECT_EQ(model->rowCount(), 0);
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("A")));
    EXPECT_EQ(model->rowCount(), 1);
    EXPECT_EQ(model->data(model->index(0, 0), kDefAppIdRole).toString(), QStringLiteral("a1"));
}

TEST(CategoryModel, OnAddAppNotInCategoryListIsIgnored)
{
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    EXPECT_EQ(model->rowCount(), 0);
    // Directly call onAddApp with an app that is NOT in the category's list.
    model->onAddApp(makeApp(QStringLiteral("ghost"), false));
    EXPECT_EQ(model->rowCount(), 0);
}

TEST(CategoryModel, OnAddAppSortsByNameAscending)
{
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    cat.addUserItem(makeApp(QStringLiteral("c1"), false, QStringLiteral("Charlie")));
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("Alpha")));
    cat.addUserItem(makeApp(QStringLiteral("b1"), false, QStringLiteral("Bravo")));
    ASSERT_EQ(model->rowCount(), 3);
    EXPECT_EQ(model->data(model->index(0, 0), kDefAppNameRole).toString(), QStringLiteral("Alpha"));
    EXPECT_EQ(model->data(model->index(1, 0), kDefAppNameRole).toString(), QStringLiteral("Bravo"));
    EXPECT_EQ(model->data(model->index(2, 0), kDefAppNameRole).toString(), QStringLiteral("Charlie"));
}

// The onAddApp sort comparator (categorymodel.cpp:131) is:
//   (item.CanDelete && item.CanDelete != app.CanDelete) || item.Name > app.Name
// The first sub-expression short-circuits to false when item.CanDelete is
// false, so only Name comparison applies in that case. When item.CanDelete is
// true AND differs from app.CanDelete, the new app is inserted before the
// existing item regardless of Name — this is the CanDelete branch that the
// original onAddApp tests (all CanDelete=false) never exercised.

TEST(CategoryModel, OnAddAppCanDeleteFalseInsertedBeforeCanDeleteTrueByName)
{
    // Existing item: CanDelete=true, Name "Alpha".
    // New app: CanDelete=false, Name "Zulu" (sorts AFTER "Alpha" by name).
    // The CanDelete branch must fire: non-deletable app inserted at index 0.
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    cat.addUserItem(makeApp(QStringLiteral("t1"), false, QStringLiteral("Alpha"), true));
    ASSERT_EQ(model->rowCount(), 1);
    cat.addUserItem(makeApp(QStringLiteral("f1"), false, QStringLiteral("Zulu"), false));
    ASSERT_EQ(model->rowCount(), 2);
    // "Zulu" (CanDelete=false) must precede "Alpha" (CanDelete=true).
    EXPECT_EQ(model->data(model->index(0, 0), kDefAppIdRole).toString(), QStringLiteral("f1"));
    EXPECT_EQ(model->data(model->index(0, 0), kDefAppCanDeleteRole).toBool(), false);
    EXPECT_EQ(model->data(model->index(1, 0), kDefAppIdRole).toString(), QStringLiteral("t1"));
    EXPECT_EQ(model->data(model->index(1, 0), kDefAppCanDeleteRole).toBool(), true);
}

TEST(CategoryModel, OnAddAppCanDeleteTrueUsesNameSortWhenItemCanDeleteFalse)
{
    // Existing item: CanDelete=false, Name "Zulu".
    // New app: CanDelete=true, Name "Alpha" (sorts BEFORE "Zulu" by name).
    // item.CanDelete is false → CanDelete sub-expression short-circuits →
    // only Name comparison applies. The deletable app is placed by Name, not
    // pushed after the non-deletable item.
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    cat.addUserItem(makeApp(QStringLiteral("f1"), false, QStringLiteral("Zulu"), false));
    ASSERT_EQ(model->rowCount(), 1);
    cat.addUserItem(makeApp(QStringLiteral("t1"), false, QStringLiteral("Alpha"), true));
    ASSERT_EQ(model->rowCount(), 2);
    // "Alpha" (CanDelete=true) before "Zulu" (CanDelete=false) by Name.
    EXPECT_EQ(model->data(model->index(0, 0), kDefAppIdRole).toString(), QStringLiteral("t1"));
    EXPECT_EQ(model->data(model->index(0, 0), kDefAppCanDeleteRole).toBool(), true);
    EXPECT_EQ(model->data(model->index(1, 0), kDefAppIdRole).toString(), QStringLiteral("f1"));
    EXPECT_EQ(model->data(model->index(1, 0), kDefAppCanDeleteRole).toBool(), false);
}

TEST(CategoryModel, OnAddAppMixedCanDeleteGroupsNonDeletableBeforeDeletable)
{
    // Add two CanDelete=true apps (sorted by Name), then two CanDelete=false
    // apps (sorted by Name among themselves). Final order must be:
    //   [non-deletable by Name] then [deletable by Name].
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    cat.addUserItem(makeApp(QStringLiteral("t1"), false, QStringLiteral("Bravo"), true));
    cat.addUserItem(makeApp(QStringLiteral("t2"), false, QStringLiteral("Alpha"), true));
    cat.addUserItem(makeApp(QStringLiteral("f1"), false, QStringLiteral("Zulu"), false));
    cat.addUserItem(makeApp(QStringLiteral("f2"), false, QStringLiteral("Yankee"), false));
    ASSERT_EQ(model->rowCount(), 4);

    // CanDelete=false group first (sorted by Name): Yankee, Zulu.
    EXPECT_EQ(model->data(model->index(0, 0), kDefAppIdRole).toString(), QStringLiteral("f2"));
    EXPECT_EQ(model->data(model->index(0, 0), kDefAppNameRole).toString(), QStringLiteral("Yankee"));
    EXPECT_EQ(model->data(model->index(0, 0), kDefAppCanDeleteRole).toBool(), false);
    EXPECT_EQ(model->data(model->index(1, 0), kDefAppIdRole).toString(), QStringLiteral("f1"));
    EXPECT_EQ(model->data(model->index(1, 0), kDefAppNameRole).toString(), QStringLiteral("Zulu"));
    EXPECT_EQ(model->data(model->index(1, 0), kDefAppCanDeleteRole).toBool(), false);
    // CanDelete=true group second (sorted by Name): Alpha, Bravo.
    EXPECT_EQ(model->data(model->index(2, 0), kDefAppIdRole).toString(), QStringLiteral("t2"));
    EXPECT_EQ(model->data(model->index(2, 0), kDefAppNameRole).toString(), QStringLiteral("Alpha"));
    EXPECT_EQ(model->data(model->index(2, 0), kDefAppCanDeleteRole).toBool(), true);
    EXPECT_EQ(model->data(model->index(3, 0), kDefAppIdRole).toString(), QStringLiteral("t1"));
    EXPECT_EQ(model->data(model->index(3, 0), kDefAppNameRole).toString(), QStringLiteral("Bravo"));
    EXPECT_EQ(model->data(model->index(3, 0), kDefAppCanDeleteRole).toBool(), true);
}

// ---- onRemoveApp (via Category signal) ----

TEST(CategoryModel, OnRemoveAppViaCategorySignalUpdatesModel)
{
    Category cat;
    App a1 = makeApp(QStringLiteral("a1"), false, QStringLiteral("A"));
    cat.addUserItem(a1);
    CategoryModel *model = new CategoryModel(&cat);
    ASSERT_EQ(model->rowCount(), 1);
    cat.delUserItem(a1);
    EXPECT_EQ(model->rowCount(), 0);
}

TEST(CategoryModel, OnRemoveAppNotInModelDoesNotCrash)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("A")));
    CategoryModel *model = new CategoryModel(&cat);
    // Directly call onRemoveApp with an app not in the model.
    model->onRemoveApp(makeApp(QStringLiteral("ghost"), false));
    EXPECT_EQ(model->rowCount(), 1);
}

// ---- onDefaultChanged ----

TEST(CategoryModel, OnDefaultChangedEmitsDataChanged)
{
    Category cat;
    App a1 = makeApp(QStringLiteral("a1"), false, QStringLiteral("A"));
    cat.addUserItem(a1);
    cat.addUserItem(makeApp(QStringLiteral("a2"), false, QStringLiteral("B")));
    CategoryModel *model = new CategoryModel(&cat);
    QSignalSpy spy(model, &CategoryModel::dataChanged);
    cat.setDefault(a1);
    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.at(0).toModelIndex().row(), 0);
    EXPECT_EQ(args.at(1).toModelIndex().row(), 1); // last row = size-1 = 1
}

TEST(CategoryModel, OnDefaultChangedWithEmptyListStillEmits)
{
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    QSignalSpy spy(model, &CategoryModel::dataChanged);
    // Trigger defaultChanged with an empty model list.
    // createIndex(0,0) and createIndex(-1,0) are used internally.
    cat.setDefault(makeApp(QStringLiteral("d1"), false));
    ASSERT_EQ(spy.count(), 1);
}

// ---- resetApp ----

TEST(CategoryModel, ResetAppViaClearAllSignal)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("A")));
    cat.addUserItem(makeApp(QStringLiteral("a2"), false, QStringLiteral("B")));
    CategoryModel *model = new CategoryModel(&cat);
    ASSERT_EQ(model->rowCount(), 2);
    // clear() empties the category and emits clearAll → resetApp.
    cat.clear();
    EXPECT_EQ(model->rowCount(), 0);
}

TEST(CategoryModel, ResetAppReplacesModelFromCategory)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("A")));
    CategoryModel *model = new CategoryModel(&cat);
    ASSERT_EQ(model->rowCount(), 1);
    // Clear and add new items.
    cat.clear();
    EXPECT_EQ(model->rowCount(), 0);
    cat.addUserItem(makeApp(QStringLiteral("b1"), false, QStringLiteral("B")));
    cat.addUserItem(makeApp(QStringLiteral("b2"), false, QStringLiteral("C")));
    ASSERT_EQ(model->rowCount(), 2);
    // Manually call resetApp — replaces model list with category's current list.
    model->resetApp();
    EXPECT_EQ(model->rowCount(), 2);
    EXPECT_EQ(model->data(model->index(0, 0), kDefAppIdRole).toString(), QStringLiteral("b1"));
    EXPECT_EQ(model->data(model->index(1, 0), kDefAppIdRole).toString(), QStringLiteral("b2"));
}

// ---- getAppById ----

TEST(CategoryModel, GetAppByIdFoundReturnsNonNull)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false, QStringLiteral("A")));
    CategoryModel *model = new CategoryModel(&cat);
    // NOTE: getAppById searches the category's getappItem() which returns by
    // value.  The returned pointer points into a temporary copy — this is a
    // known source-code defect (dangling pointer).  We only test non-null
    // without dereferencing.
    const App *app = model->getAppById(QStringLiteral("a1"));
    EXPECT_NE(app, nullptr);
}

TEST(CategoryModel, GetAppByIdNotFoundReturnsNull)
{
    Category cat;
    cat.addUserItem(makeApp(QStringLiteral("a1"), false));
    CategoryModel *model = new CategoryModel(&cat);
    EXPECT_EQ(model->getAppById(QStringLiteral("nonexistent")), nullptr);
}

TEST(CategoryModel, GetAppByIdOnEmptyCategoryReturnsNull)
{
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    EXPECT_EQ(model->getAppById(QStringLiteral("any")), nullptr);
}

// ---- isValid ----

TEST(CategoryModel, IsValidWithValidIdReturnsTrue)
{
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    EXPECT_TRUE(model->isValid(makeApp(QStringLiteral("a1"), false)));
}

TEST(CategoryModel, IsValidWithEmptyIdReturnsFalse)
{
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    EXPECT_FALSE(model->isValid(makeApp(QString(), false)));
}

TEST(CategoryModel, IsValidWithNullIdReturnsFalse)
{
    Category cat;
    CategoryModel *model = new CategoryModel(&cat);
    App a;
    a.Id = QString();  // null QString
    EXPECT_FALSE(model->isValid(a));
}
