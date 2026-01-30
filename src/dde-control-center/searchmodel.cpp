// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "searchmodel.h"

#include "dccobject.h"
#include "dccobject_p.h"

#include <DPinyin>

#include <QIcon>
#include <QTextDocument>

namespace dccV25 {

struct SearchData
{
    QString display;
    QString text;
    QString plainText;
    QString url;
    QList<QStringList> searchTexts;
    const DccObject *obj;
    const DccObject *ancestors;
    QList<int> weight;

    explicit SearchData(const DccObject *o)
        : obj(o)
    {
        ancestors = o;
        while (ancestors) {
            weight.prepend(ancestors->weight());
            const DccObject *objParent = DccObject::Private::FromObject(ancestors)->getParent();
            if (!objParent || objParent->name() == "root") {
                break;
            }
            ancestors = objParent;
        }
    }

    inline const QString sourceText() const { return text.isEmpty() ? obj->displayName() : text; }

    inline const QString sourceUrl() const { return url.isEmpty() ? obj->parentName() + "/" + obj->name() : url; }
};

//////////////////////////////////////////////////////
class SearchSourceModel : public QAbstractItemModel
{
public:
    explicit SearchSourceModel(QObject *parent = nullptr);
    ~SearchSourceModel() override;

    void addSearchData(DccObject *obj, const QString &text, const QString &url);
    void removeSearchData(const DccObject *obj, const QString &text);

protected:
    void addObject(DccObject *obj, const QString &text, const QString &url);
    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parentIndex = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

private:
    QList<SearchData *> m_data;
    QTextDocument m_doc;
};

SearchSourceModel::SearchSourceModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

SearchSourceModel::~SearchSourceModel()
{
    qDeleteAll(m_data);
}

void SearchSourceModel::addSearchData(DccObject *obj, const QString &text, const QString &url)
{
    if (!obj) {
        return;
    }
    addObject(obj, text, url);
    if (text.isEmpty() && url.isEmpty()) {
        QVector<DccObject *> objs(obj->getChildren());
        while (!objs.isEmpty()) {
            DccObject *o = objs.takeFirst();
            addObject(o, QString(), QString());
            objs.append(o->getChildren());
        }
    }
}

void SearchSourceModel::removeSearchData(const DccObject *obj, const QString &text)
{
    if (!obj) {
        return;
    }
    auto beginIt = m_data.begin();
    for (auto it = beginIt; it != m_data.end();) {
        if ((*it)->obj == obj && (text.isEmpty() || (*it)->text == text)) {
            int i = it - beginIt;
            beginRemoveRows(QModelIndex(), i, i);
            delete (*it);
            it = m_data.erase(it);
            endRemoveRows();
        } else {
            ++it;
        }
    }
}

void SearchSourceModel::addObject(DccObject *obj, const QString &text, const QString &url)
{
    if (!obj || !obj->canSearch()) {
        return;
    }
    m_doc.setHtml(text.isEmpty() ? obj->displayName() : text);
    const QString &sText = m_doc.toPlainText().toLower();
    if (sText.isEmpty()) {
        return;
    }
    SearchData *data = new SearchData(obj);
    data->text = sText;
    data->url = url;
    bool ok = false;
    for (auto &&c : sText) {
        data->searchTexts << Dtk::Core::pinyin(c, Dtk::Core::TS_NoneTone, &ok);
    }
    if (!ok) {
        data->searchTexts.clear();
    }
    // 排序规则不会变，添加时排序，避免在显示时处理
    int index = 0;
    bool findOk = false;
    for (auto d : m_data) {
        for (int i = 0; i < d->weight.size() && i < data->weight.size(); ++i) {
            if (d->weight.at(i) < data->weight.at(i)) {
                break;
            } else if (d->weight.at(i) > data->weight.at(i)) {
                findOk = true;
                break;
            }
        }
        if (findOk) {
            break;
        }
        index++;
    }
    beginInsertRows(QModelIndex(), index, index);
    m_data.insert(index, data);
    endInsertRows();
}

QModelIndex SearchSourceModel::index(int row, int column, const QModelIndex &) const
{
    if (row < 0 || row >= rowCount()) {
        return QModelIndex();
    }
    return createIndex(row, column);
}

QModelIndex SearchSourceModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int SearchSourceModel::rowCount(const QModelIndex &) const
{
    return m_data.size();
}

int SearchSourceModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant SearchSourceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    int i = index.row();
    if (i < 0 || i >= m_data.size()) {
        return QVariant();
    }
    const SearchData *data = m_data.at(i);
    switch (role) {
    case Qt::DisplayRole:
        return data->display.isEmpty() ? data->sourceText() : data->display;
    case Qt::DecorationRole: {
        return data->ancestors->icon();
    } break;
    case SearchModel::SearchPlainTextRole:
        return data->plainText;
    case SearchModel::SearchDataRole:
        return QVariant::fromValue(data);
    case SearchModel::SearchUrlRole:
        return data->sourceUrl() + "?indicator=true";
    case SearchModel::SearchTextRole:
        return data->sourceText();
    case SearchModel::SearchWeightRole:
        return QVariant::fromValue(data->weight);
    default:
        break;
    }
    return QVariant();
}

bool SearchSourceModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int i = index.row();
    if (i < 0 || i >= m_data.size()) {
        return false;
    }
    SearchData *data = m_data.at(i);
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole: {
        const QString &v = value.toString();
        if (v != data->display) {
            data->display = value.toString();
            // Q_EMIT dataChanged(index, index, { Qt::DisplayRole });
        }
        return true;
    }
    case SearchModel::SearchPlainTextRole:
        data->plainText = value.toString();
        return true;
    default:
        break;
    }
    return false;
}

//////////////////////////////////////////////////////
SearchModel::SearchModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterRole(SearchTextRole);
    setSortRole(SearchWeightRole);
    setSourceModel(new SearchSourceModel(this));
}

QHash<int, QByteArray> SearchModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractItemModel::roleNames();
    names[SearchUrlRole] = "url";
    names[SearchPlainTextRole] = "plainText";
    names[SearchIsEndRole] = "isEnd";
    return names;
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    if (role == SearchIsEndRole) {
        int row = index.row();
        if (row == rowCount() - 1) {
            return false;
        }
        const SearchData *curentData = QSortFilterProxyModel::data(index, SearchDataRole).value<const SearchData *>();
        const SearchData *nextData = QSortFilterProxyModel::data(this->index(row + 1, 0), SearchDataRole).value<const SearchData *>();
        return curentData->ancestors != nextData->ancestors;
    }
    return QSortFilterProxyModel::data(index, role);
}

void SearchModel::addSearchData(DccObject *obj, const QString &text, const QString &url)
{
    static_cast<SearchSourceModel *>(sourceModel())->addSearchData(obj, text, url);
}

void SearchModel::removeSearchData(const DccObject *obj, const QString &text)
{
    static_cast<SearchSourceModel *>(sourceModel())->removeSearchData(obj, text);
}

bool SearchModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
    if (!sourceIndex.isValid())
        return false;

    const SearchData *data = sourceModel()->data(sourceIndex, SearchDataRole).value<const SearchData *>();
    QString text = data->sourceText();
    QString filterText = filterRegularExpression().pattern().trimmed();
    if (filterText.isEmpty()) {
        return false;
    }
    filterText = filterText.toLower();
    QList<int> findIndex;
    int from = 0;
    bool findOk = true;
    for (QChar &c : filterText) {
        from = text.indexOf(c, from, Qt::CaseInsensitive);
        if (from < 0) {
            findOk = false;
            break;
        }
        findIndex.append(from++);
    }
    if (!findOk) {
        bool isAllLetter = true;
        for (QChar &c : filterText) {
            isAllLetter &= c.unicode() < 127;
        }
        if (isAllLetter) {
            QList<QStringList> texts = data->searchTexts;
            findIndex.clear();
            int wordsIndex = -1;

            auto cIt = filterText.cbegin();
            for (auto &&words : texts) {
                ++wordsIndex;
                for (auto &&py : words) {
                    from = 0;
                    while (from < py.size() && cIt != filterText.cend() && py.at(from) == (*cIt)) {
                        ++cIt;
                        ++from;
                    }
                    if (from > 0) {
                        findIndex.append(wordsIndex);
                        break;
                    }
                }
                if (cIt == filterText.cend()) {
                    findOk = true;
                    break;
                }
            }
        }
        if (!findOk) {
            return false;
        }
    }
    QString display;
    int i = 0;
    bool noEnd = false;
    for (auto &&sub : text) {
        if (!findIndex.isEmpty() && i == findIndex.first()) {
            if (!noEnd) {
                display.append("<font color='red'>");
                noEnd = true;
            }
            display.append(sub);
            findIndex.takeFirst();
        } else {
            if (noEnd) {
                display.append("</font>");
                noEnd = false;
            }
            display.append(sub);
        }
        ++i;
    }
    if (noEnd) {
        display.append("</font>");
    }
    QStringList displays(display);
    const DccObject *p = DccObject::Private::FromObject(data->obj)->getParent();
    while (p && p->name() != "root") {
        if (!p->displayName().isEmpty()) {
            displays.prepend(p->displayName());
        }
        p = DccObject::Private::FromObject(p)->getParent();
    }

    sourceModel()->setData(sourceIndex, displays.join("/"));
    displays.takeLast();
    displays.append(text);
    sourceModel()->setData(sourceIndex, displays.join("/"), SearchPlainTextRole);
    auto currentIndex = mapFromSource(sourceIndex);
    if (currentIndex.isValid()) {
        Q_EMIT const_cast<SearchModel *>(this)->dataChanged(currentIndex, currentIndex, { Qt::DisplayRole, SearchIsEndRole });
    }
    return true;
}

} // namespace dccV25
