#include "TableViewModel.h"

TableViewModel::TableViewModel(QObject *parent)
    : QAbstractTableModel{parent} {
    _header << "模型"
            << "大小";
}

TableViewModel::~TableViewModel() {
}

int TableViewModel::rowCount(const QModelIndex &parent) const {
    return _dataList.count();
}

int TableViewModel::columnCount(const QModelIndex &parent) const {
    return _header.count();
}

QVariant TableViewModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        return _dataList[index.row()][index.column()];
    } else if (role == Qt::DecorationPropertyRole) {
        return Qt::AlignCenter;
    } else if (role == Qt::TextAlignmentRole && index.column() == 4) {
        return Qt::AlignCenter;
    }
    return QVariant();
}

QVariant TableViewModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return _header[section];
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

void TableViewModel::addModel(QStringList model) {
    beginInsertRows(QModelIndex(), _dataList.size(), _dataList.size());
    _dataList.append(model);
    endInsertRows();
}

void TableViewModel::removeModel(const QString &model) {
    // 遍历查找匹配模型名称的项
    for (int i = 0; i < _dataList.size(); i++) {
        if (_dataList[i][0] == model) {
            beginRemoveRows(QModelIndex(), i, i);
            _dataList.removeAt(i);
            endRemoveRows();
            return;
        }
    }
}
