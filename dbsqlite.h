#ifndef DBSQLITE_H
#define DBSQLITE_H
#include <QtCore>
#include "defdbg.h"
#ifdef EMB
#include "sqlite3.h"
#endif
#ifdef _X11
#include <QtSql>
#endif
class DBSqliteStmt
{
public:
    bool next();
    bool exec();
    int getInt(int iCol);
    double getDouble(int iCol);
    QString getString(int iCol);
    bool close();
    bool reset();
    bool clearBindings();
    //index start from 1
    bool bindInt(int iCol, int val);
    bool bindDouble(int iCol, double val);
    bool bindString(int iCol, const QString& val);
    DBSqliteStmt(const DBSqliteStmt& origin);
private:
#ifdef EMB
    DBSqliteStmt(sqlite3_stmt *stm);
    sqlite3_stmt *stmt;
#endif
#ifdef _X11
    QSqlQuery *query;
    bool isExeced;
    DBSqliteStmt(QSqlQuery *other);
#endif
   friend class DBSqlite;
};
class DBSqlite
{
public:
    DBSqlite();
    DBSqlite(const QString& filename);
    ~DBSqlite();
    int open(const QString& filename);
    DBSqliteStmt prepare(const QString& zSql);
    int close();
    int exec(const QString& sql);
private:
    DBSqlite(const DBSqlite& other){}
    DBSqlite(DBSqlite& other){}
#ifdef EMB
    sqlite3 *db;
#endif
#ifdef _X11
    QSqlDatabase *db;
#endif
};

#endif // DBSQLITE_H
