#ifndef TABLEVIEWMODEL_H
#define TABLEVIEWMODEL_H
#include <qabstractitemmodel.h>


class TableViewModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit TableViewModel(QObject *parent = nullptr);

    ~TableViewModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void addModel(QStringList model);

    void removeModel(const QString &model);

private:
    QStringList _header;
    QList<QStringList> _dataList;
};


#endif //TABLEVIEWMODEL_H
