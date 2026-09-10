// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "searchmodel.h"

#include "dccobject.h"
#include "dccobject_p.h"

#include <DPinyin>

#include <QAbstractListModel>
#include <QHash>
#include <QSet>
#include <QTextDocument>

#include <algorithm>
#include <utility>

namespace dccV25 {

struct SearchData
{
    QString text;
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

    inline QString sourceText() const { return text.isEmpty() ? obj->displayName() : text; }

    inline QString sourceUrl() const
    {
        return url.isEmpty() ? obj->parentName() + "/" + obj->name() : url;
    }
};

struct SearchMatchResult
{
    bool accepted = false;
    QList<int> positions;
    QString displayText;
    QString plainText;
    unsigned int score = 0;
};

class SearchModelPrivate
{
public:
    QString filterText;
    QHash<const SearchData *, SearchMatchResult> results;
};

//////////////////////////////////////////////////////
class SearchSourceModel : public QAbstractListModel
{
public:
    explicit SearchSourceModel(QObject *parent = nullptr);
    ~SearchSourceModel() override;

    void addSearchData(DccObject *obj, const QString &text, const QString &url);
    void removeSearchData(const DccObject *obj, const QString &text);

    void beginBatch();
    void endBatch();
    bool isBatching() const;

protected:
    void addObject(DccObject *obj, const QString &text, const QString &url);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QList<SearchData *> m_data;
    QTextDocument m_doc;
    QVector<SearchData *> m_batchData;
    int m_batchDepth = 0;
};

SearchSourceModel::SearchSourceModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

SearchSourceModel::~SearchSourceModel()
{
    qDeleteAll(m_data);
    qDeleteAll(m_batchData);
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
    for (int row = m_data.size() - 1; row >= 0; --row) {
        SearchData *data = m_data.at(row);
        if (data->obj == obj && (text.isEmpty() || data->text == text)) {
            beginRemoveRows(QModelIndex(), row, row);
            delete m_data.takeAt(row);
            endRemoveRows();
        }
    }
    for (int row = m_batchData.size() - 1; row >= 0; --row) {
        SearchData *data = m_batchData.at(row);
        if (data->obj == obj && (text.isEmpty() || data->text == text)) {
            delete m_batchData.takeAt(row);
        }
    }
}

static bool weightLessThan(const SearchData *a, const SearchData *b)
{
    for (int i = 0; i < a->weight.size() && i < b->weight.size(); ++i) {
        if (a->weight.at(i) < b->weight.at(i))
            return true;
        if (a->weight.at(i) > b->weight.at(i))
            return false;
    }
    return a->weight.size() < b->weight.size();
}

void SearchSourceModel::beginBatch()
{
    ++m_batchDepth;
}

void SearchSourceModel::endBatch()
{
    if (m_batchDepth == 0) {
        return;
    }
    --m_batchDepth;
    if (m_batchDepth > 0) {
        return;
    }
    if (m_batchData.isEmpty()) {
        return;
    }

    // 1. sort batch data by weight
    std::stable_sort(m_batchData.begin(), m_batchData.end(), weightLessThan);

    // 2. two-way merge m_data (sorted) + m_batchData (sorted), record new item indices
    QList<SearchData *> merged;
    merged.reserve(m_data.size() + m_batchData.size());
    QVector<int> newIndices;
    int mi = 0, mj = 0;
    while (mi < m_data.size() && mj < m_batchData.size()) {
        if (weightLessThan(m_batchData.at(mj), m_data.at(mi))) {
            newIndices.append(merged.size());
            merged.append(m_batchData.at(mj++));
        } else {
            merged.append(m_data.at(mi++));
        }
    }
    while (mi < m_data.size()) {
        merged.append(m_data.at(mi++));
    }
    while (mj < m_batchData.size()) {
        newIndices.append(merged.size());
        merged.append(m_batchData.at(mj++));
    }

    // 3. group new indices into consecutive ranges
    QVector<QPair<int, int>> ranges;
    if (!newIndices.isEmpty()) {
        int rangeStart = newIndices.first();
        int rangeEnd = rangeStart;
        for (int k = 1; k < newIndices.size(); ++k) {
            if (newIndices.at(k) == rangeEnd + 1) {
                rangeEnd = newIndices.at(k);
            } else {
                ranges.append({ rangeStart, rangeEnd });
                rangeStart = newIndices.at(k);
                rangeEnd = rangeStart;
            }
        }
        ranges.append({ rangeStart, rangeEnd });
    }

    // 4. restore m_data to old-only state, then apply final indices from front to back
    QSet<SearchData *> batchSet(m_batchData.begin(), m_batchData.end());
    m_data.clear();
    for (auto *d : merged) {
        if (!batchSet.contains(d)) {
            m_data.append(d);
        }
    }
    for (const auto &range : std::as_const(ranges)) {
        int start = range.first;
        int end = range.second;
        beginInsertRows(QModelIndex(), start, end);
        for (int k = end; k >= start; --k) {
            m_data.insert(start, merged.at(k));
        }
        endInsertRows();
    }

    m_batchData.clear();
}

bool SearchSourceModel::isBatching() const
{
    return m_batchDepth > 0;
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
    if (isBatching()) {
        m_batchData.append(data);
        return;
    }
    // 排序规则不会变，添加时排序，避免在显示时处理
    const auto pos = std::upper_bound(m_data.cbegin(),
                                      m_data.cend(),
                                      data,
                                      [](const SearchData *value, const SearchData *item) {
                                          return weightLessThan(value, item);
                                      });
    const int index = std::distance(m_data.cbegin(), pos);
    beginInsertRows(QModelIndex(), index, index);
    m_data.insert(index, data);
    endInsertRows();
}

int SearchSourceModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_data.size();
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
        return data->sourceText();
    case Qt::DecorationRole: {
        return data->ancestors->icon();
    } break;
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

//////////////////////////////////////////////////////
static QList<int> matchPositions(const SearchData *data, const QString &filterText)
{
    const QString text = data->sourceText();
    QList<int> positions;
    int from = 0;
    bool found = true;
    for (const QChar c : filterText) {
        from = text.indexOf(c, from, Qt::CaseInsensitive);
        if (from < 0) {
            found = false;
            break;
        }
        positions.append(from++);
    }
    if (found) {
        return positions;
    }

    bool isAllLetter = true;
    for (const QChar c : filterText) {
        isAllLetter &= c.unicode() < 127;
    }
    if (!isAllLetter) {
        return {};
    }

    positions.clear();
    int wordsIndex = -1;
    auto cIt = filterText.cbegin();
    for (const auto &words : data->searchTexts) {
        ++wordsIndex;
        for (const auto &pinyin : words) {
            from = 0;
            while (from < pinyin.size() && cIt != filterText.cend() && pinyin.at(from) == *cIt) {
                ++cIt;
                ++from;
            }
            if (from > 0) {
                positions.append(wordsIndex);
                break;
            }
        }
        if (cIt == filterText.cend()) {
            return positions;
        }
    }
    return {};
}

static unsigned int matchScore(const QString &text, const QList<int> &positions)
{
    unsigned int leftCnt = positions.first(); // 开头未匹配字符
    unsigned int midCnt = 0;                  // 中间未匹配字符，相关度影响最大
    unsigned int rightCnt = text.length() - positions.last() - 1; // 末尾未匹配字符
    for (auto it = positions.cbegin() + 1; it != positions.cend(); ++it) {
        midCnt = midCnt * 10 + (*it - *(it - 1)) - 1;
    }
    if (midCnt > 0x0000FFFF) {
        midCnt = 0x0000FFFF;
    }
    return rightCnt + leftCnt * 0x00000100 + midCnt * 0x00010000;
}

static QString highlightedText(const QString &text, const QList<int> &positions)
{
    QString display;
    auto position = positions.cbegin();
    bool highlighting = false;
    for (int i = 0; i < text.size(); ++i) {
        if (position != positions.cend() && i == *position) {
            if (!highlighting) {
                display.append("<font color='red'>");
                highlighting = true;
            }
            display.append(text.at(i));
            ++position;
        } else {
            if (highlighting) {
                display.append("</font>");
                highlighting = false;
            }
            display.append(text.at(i));
        }
    }
    if (highlighting) {
        display.append("</font>");
    }
    return display;
}

static QStringList ancestorPath(const SearchData *data)
{
    QStringList path;
    const DccObject *parent = DccObject::Private::FromObject(data->obj)->getParent();
    while (parent && parent->name() != "root") {
        if (!parent->displayName().isEmpty()) {
            path.prepend(parent->displayName());
        }
        parent = DccObject::Private::FromObject(parent)->getParent();
    }
    return path;
}

static SearchMatchResult createMatchResult(const SearchData *data, const QString &filterText)
{
    SearchMatchResult result;
    if (!data || filterText.isEmpty()) {
        return result;
    }

    result.positions = matchPositions(data, filterText);
    if (result.positions.isEmpty()) {
        return result;
    }

    const QString text = data->sourceText();
    result.accepted = true;
    result.score = matchScore(text, result.positions);

    const QStringList path = ancestorPath(data);
    QStringList displayPath = path;
    displayPath.append(highlightedText(text, result.positions));
    result.displayText = displayPath.join('/');

    QStringList plainPath = path;
    plainPath.append(text);
    result.plainText = plainPath.join('/');
    return result;
}

static const SearchData *searchData(const QAbstractItemModel *model, const QModelIndex &index)
{
    return model->data(index, SearchModel::SearchDataRole).value<const SearchData *>();
}

//////////////////////////////////////////////////////
SearchModel::SearchModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , d_ptr(new SearchModelPrivate)
{
    setFilterRole(SearchTextRole);
    setSortRole(SearchMatchScoreRole);
    setDynamicSortFilter(false);
    setSourceModel(new SearchSourceModel(this));
    refreshSearchResults();
    sort(0);
}

SearchModel::~SearchModel() = default;

QHash<int, QByteArray> SearchModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractItemModel::roleNames();
    names[SearchUrlRole] = "url";
    names[SearchPlainTextRole] = "plainText";
    return names;
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    if (role != Qt::DisplayRole && role != SearchPlainTextRole && role != SearchMatchScoreRole) {
        return QSortFilterProxyModel::data(index, role);
    }

    const QModelIndex sourceIndex = mapToSource(index);
    const SearchData *entry = searchData(sourceModel(), sourceIndex);
    const auto result = d_ptr->results.constFind(entry);
    if (result == d_ptr->results.cend()) {
        return QSortFilterProxyModel::data(index, role);
    }
    if (role == Qt::DisplayRole) {
        return result->displayText;
    }
    if (role == SearchPlainTextRole) {
        return result->plainText;
    }
    return result->score;
}

void SearchModel::setFilterRegularExpression(const QString &pattern)
{
    setFilterRegularExpression(QRegularExpression(pattern));
}

void SearchModel::setFilterRegularExpression(const QRegularExpression &regularExpression)
{
    d_ptr->filterText = regularExpression.pattern().trimmed().toLower();
    refreshSearchResults();
    QSortFilterProxyModel::setFilterRegularExpression(regularExpression);
    sort(0);
}

void SearchModel::addSearchData(DccObject *obj, const QString &text, const QString &url)
{
    auto model = static_cast<SearchSourceModel *>(sourceModel());
    model->addSearchData(obj, text, url);
    if (!model->isBatching() && !d_ptr->filterText.isEmpty()) {
        refreshSearchResults();
        invalidateRowsFilter();
        sort(0);
    }
}

void SearchModel::removeSearchData(const DccObject *obj, const QString &text)
{
    static_cast<SearchSourceModel *>(sourceModel())->removeSearchData(obj, text);
    if (d_ptr->filterText.isEmpty()) {
        return;
    }
    refreshSearchResults();
    invalidateRowsFilter();
    sort(0);
}

void SearchModel::beginBatch()
{
    static_cast<SearchSourceModel *>(sourceModel())->beginBatch();
}

void SearchModel::endBatch()
{
    auto model = static_cast<SearchSourceModel *>(sourceModel());
    model->endBatch();
    if (model->isBatching()) {
        return;
    }
    if (d_ptr->filterText.isEmpty()) {
        return;
    }
    refreshSearchResults();
    invalidateRowsFilter();
    sort(0);
}

bool SearchModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (d_ptr->filterText.isEmpty()) {
        return false;
    }
    const QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
    if (!sourceIndex.isValid()) {
        return false;
    }
    const SearchData *entry = searchData(sourceModel(), sourceIndex);
    const auto result = d_ptr->results.constFind(entry);
    return result != d_ptr->results.cend() && result->accepted;
}

bool SearchModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const auto left = d_ptr->results.constFind(searchData(sourceModel(), source_left));
    const auto right = d_ptr->results.constFind(searchData(sourceModel(), source_right));
    if (left == d_ptr->results.cend() || right == d_ptr->results.cend()) {
        return false;
    }
    if (left->score == right->score) {
        return left->plainText < right->plainText;
    }
    return left->score < right->score;
}

void SearchModel::refreshSearchResults()
{
    d_ptr->results.clear();
    if (d_ptr->filterText.isEmpty()) {
        return;
    }
    d_ptr->results.reserve(sourceModel()->rowCount());
    for (int row = 0; row < sourceModel()->rowCount(); ++row) {
        const QModelIndex sourceIndex = sourceModel()->index(row, 0);
        const SearchData *entry = searchData(sourceModel(), sourceIndex);
        if (entry) {
            d_ptr->results.insert(entry, createMatchResult(entry, d_ptr->filterText));
        }
    }
}

} // namespace dccV25
