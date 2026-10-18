#ifndef DBHELPER_H
#define DBHELPER_H

#include <QSqlQuery>
#include <QVariant>
#include <QList>
#include <functional>

// 键值对
struct KeyValuePair{
    QString column;
    QVariant value;
};

// 查询选项
struct QueryOptions {
    QList<KeyValuePair> conds{}; // 查询条件
    QStringList orderBy{}; // 排序
};

class DbHelper
{
public:
    DbHelper();

    // sql语句查询表数据
    QList<QMap<QString, QVariant>> query(QSqlDatabase db, const QString &sql);

    // 查询表数据
    QList<QMap<QString, QVariant>> query(QSqlDatabase db, const QString &tableName, const QStringList &fields, const QueryOptions &opts = {});

    // 插入数据到表中
    bool insert(QSqlDatabase db, const QString &tableName, const QStringList &fields, const QList<QVariant> &values = {});

    // 更新表中的数据
    bool update(QSqlDatabase db, const QString &tableName, const QList<KeyValuePair> &pairs, const QList<KeyValuePair> &conds = {});

    // 删除表中的数据
    bool remove(QSqlDatabase db, const QString &tableName, const QList<KeyValuePair> &conds = {});

    // 执行事务
    bool executeTransaction(QSqlDatabase db, std::function<bool(QSqlDatabase)> func);
};

#endif // DBHELPER_H
