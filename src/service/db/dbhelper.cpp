#include "dbhelper.h"
#include "logging.h"
#include "dbmanager.h"
#include <QDebug>
#include <QSqlRecord>

DbHelper::DbHelper() {}

QList<QMap<QString, QVariant>>DbHelper::query(QSqlDatabase db, const QString &sql)
{
    QString trimmed = sql.trimmed().toUpper();

    if(!trimmed.startsWith("SELECT")){
        qCWarning(dbLog) << "Only SELECT statements are allowed:" << sql;
        return {};
    }

    QSqlQuery query(db);

    if(!query.exec(sql)){
        qCCritical(dbLog) << "Query failed:" << sql << query.lastError().text();
        return {};
    }

    QSqlRecord record = query.record();
    int columnRows = record.count();

    QList<QMap<QString, QVariant>> results;
    while(query.next()){
        QMap<QString, QVariant> row;

        for(int i = 0; i < columnRows; ++i){
            row[record.fieldName(i)] = query.value(i);
        }

        results.append(row);
    }

    return results;
}

QList<QMap<QString, QVariant>> DbHelper::query(QSqlDatabase db, const QString &tableName, const QStringList &fields, const QueryOptions &opts)
{
    QSqlQuery query(db);

    QList<KeyValuePair> placeholders;

    QStringList whereParts;
    for(int i = 0; i < opts.conds.size(); ++i){
        QString ph = ":" + opts.conds[i].column;
        whereParts << opts.conds[i].column + " = " + ph;
        placeholders.append({ph, opts.conds[i].value});
    }

    QString sql = "SELECT "
                  + fields.join(", ")
                  + " FROM " + tableName;

    if(!whereParts.isEmpty()){
        sql += " WHERE " + whereParts.join(" AND ");
    }

    if(!opts.orderBy.isEmpty()){
        sql += " ORDER BY " + opts.orderBy.join(", ");
    }

    if(!query.prepare(sql)){
        qCCritical(dbLog) << "Prepare  failed:" << sql << query.lastError().text();
        return {};
    }

    for(const auto &i : placeholders ){
        query.bindValue(i.column, i.value);
    }

    if(!query.exec()){
        qCCritical(dbLog) << "Query failed:" << sql << query.lastError().text();
        return {};
    }

    QList<QMap<QString, QVariant>> results;

    QSqlRecord record = query.record();
    int columnRows = record.count();

    while(query.next()){
        QMap<QString, QVariant> row;

        QSqlRecord record = query.record();

        for(int i = 0; i < columnRows; ++i){
            row[record.fieldName(i)] = query.value(i);
        }

        results.append(row);
    }

    return results;
}

bool DbHelper::insert(QSqlDatabase db, const QString &tableName, const QStringList &fields, const QList<QVariant> &values)
{
    if(fields.isEmpty() || values.isEmpty() || fields.size() != values.size()){
        return false;
    }

    QSqlQuery query(db);

    QStringList placeholders;
    for(int i = 0; i < values.size(); ++i){
        placeholders << ":" + fields[i];
    }

    QString sql = "INSERT INTO " + tableName.trimmed()
                  + " (" + fields.join(", ") + ")"
                  + " VALUES(" + placeholders.join(", ") + ")";

    if(!query.prepare(sql)){
        qCCritical(dbLog) << "Prepare  failed:" << sql << query.lastError().text();
        return false;
    }

    for (int i = 0; i < placeholders.size(); ++i) {
        query.bindValue(placeholders[i], values[i]);
    }

    if(!query.exec()){
        qCCritical(dbLog) << "Insert failed:" << sql << query.lastError().text();
        return false;
    }

    return true;
}

bool DbHelper::update(QSqlDatabase db, const QString &tableName, const QList<KeyValuePair> &pairs, const QList<KeyValuePair> &conds)
{
    if(pairs.isEmpty()){
        return false;
    }

    QSqlQuery query(db);

    QList<KeyValuePair> placeholders;

    QStringList setParts;
    for(int i = 0; i < pairs.size(); ++i){
        QString ph = ":" + pairs[i].column;
        setParts << pairs[i].column + " = " + ph;
        placeholders.append({ph, pairs[i].value});
    }

    QStringList whereParts;
    for(int i = 0; i < conds.size(); ++i){
        QString ph = ":" + conds[i].column;
        whereParts << conds[i].column + " = " + ph;
        placeholders.append({ph, conds[i].value});
    }

    QString sql = "UPDATE " + tableName.trimmed()
                  + " SET " + setParts.join(", ");

    if(!whereParts.isEmpty()){
        sql += " WHERE " + whereParts.join(" AND ");
    }

    if(!query.prepare(sql)){
        qCCritical(dbLog) << "Prepare  failed:" << sql << query.lastError().text();
        return false;
    }

    for(const auto &i : placeholders){
        query.bindValue(i.column, i.value);
    }

    if(!query.exec()){
        qCCritical(dbLog) << "Update failed:" << sql << query.lastError().text();
        return false;
    }

    return true;
}

bool DbHelper::remove(QSqlDatabase db, const QString &tableName, const QList<KeyValuePair> &conds)
{
    if(conds.isEmpty()){
        qCCritical(dbLog) << "Refuse to DELETE without any condition, table:" << tableName;
        return false;
    }

    QSqlQuery query(db);

    QList<KeyValuePair> placeholders;

    QStringList whereParts;
    for(int i = 0; i < conds.size(); ++i){
        QString ph = ":" + conds[i].column;
        whereParts << conds[i].column + " = " + ph;
        placeholders.append({ph, conds[i].value});
    }

    QString sql = "DELETE FROM " + tableName.trimmed();

    if(!conds.isEmpty()){
        sql += " WHERE " + whereParts.join(" AND ");
    }

    if(!query.prepare(sql)){
        qCCritical(dbLog) << "Prepare  failed:" << sql << query.lastError().text();
        return false;
    }

    for(const auto &i : placeholders){
        query.bindValue(i.column, i.value);
    }


    if(!query.exec()){
        qCCritical(dbLog) << "Update failed:" << sql << query.lastError().text();
        return false;
    }

    return true;
}

bool DbHelper::executeTransaction(QSqlDatabase db, std::function<bool(QSqlDatabase)> func)
{
    if(!db.isOpen()) return false;

    if(!db.transaction()) return false;

    bool ok = func(db);

    if(ok){
        if(!db.commit()){
            qCCritical(dbLog) << "事务提交失败!";
            db.rollback();
        }
    }
    else{
        qCCritical(dbLog) << "事务回调函数执行失败!";
        db.rollback();
    }

    return ok;
}
