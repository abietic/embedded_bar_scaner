#include "dbsqlite.h"
#ifdef EMB
DBSqlite::DBSqlite():db(NULL)
{
}

DBSqlite::DBSqlite(const QString& filename)
{
    this->open(filename);
}

int DBSqlite::open(const QString& filename)
{
    int rc = sqlite3_open(filename.toLocal8Bit().data(),&this->db);
    if( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
#ifdef DEBUG
        exit(1);
#endif
#ifndef DEBUG
        return -1;
#endif
    }
#ifdef DEBUG
    printf("open sucess!\n");
#endif
    return 0;
}

DBSqliteStmt DBSqlite::prepare(const QString& zSql)
{
    char* zErrMsg=NULL;
    sqlite3_stmt* stmt=NULL;
    int nret=sqlite3_prepare(db,zSql.toLocal8Bit().data(),zSql.toLocal8Bit().length(),&stmt,(const char**)(&zErrMsg));
    if(nret!=SQLITE_OK)
    {
#ifdef DEBUG
        fprintf(stderr,"Failed to compile statement %s \nError is : %s \n",zSql.toLocal8Bit().data(),zErrMsg);
#endif
        sqlite3_free(zErrMsg);
        return DBSqliteStmt(NULL);
    }
    else
    {
        return DBSqliteStmt(stmt);
    }
}

int DBSqlite::close()
{
    sqlite3_close(this->db);
}

DBSqlite::~DBSqlite()
{
    this->close();
}

int DBSqlite::exec(const QString& sql)
{
    int rc;
    char *zErrMsg = 0;
    rc = sqlite3_exec(db, sql.toLocal8Bit().data(), NULL, 0, &zErrMsg);
    if( rc!=SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return rc;
}


DBSqliteStmt::DBSqliteStmt(sqlite3_stmt *stm):stmt(stm){}
DBSqliteStmt::DBSqliteStmt(const DBSqliteStmt& origin):stmt(origin.stmt){}
bool DBSqliteStmt::next()
{
    int rc = sqlite3_step(this->stmt);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW)
    {
        return false;
    }
    else if (rc == SQLITE_DONE)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool DBSqliteStmt::exec()
{
    int rc = sqlite3_step(this->stmt);
    if (rc != SQLITE_DONE)
    {
        return false;
    }
    return true;
}

int DBSqliteStmt::getInt(int iCol)
{
    return sqlite3_column_int(this->stmt,iCol);
}
double DBSqliteStmt::getDouble(int iCol)
{
    return sqlite3_column_double(this->stmt, iCol);
}
QString DBSqliteStmt::getString(int iCol)
{
    const char *res = (const char*)sqlite3_column_text(this->stmt, iCol);
    return QString(res);
}
bool DBSqliteStmt::bindInt(int iCol, int val)
{
    int rc = sqlite3_bind_int(this->stmt,iCol,val);
    if (rc != SQLITE_OK)
    {
        return false;
    }
    return true;
}
bool DBSqliteStmt::bindDouble(int iCol, double val)
{
    int rc = sqlite3_bind_double(this->stmt,iCol,val);
    if (rc != SQLITE_OK)
    {
        return false;
    }
    return true;
}
bool DBSqliteStmt::bindString(int iCol,const QString& val)
{
    int rc = sqlite3_bind_text(this->stmt,iCol,val.toLocal8Bit().data(),val.toLocal8Bit().length(),SQLITE_TRANSIENT);
    if (rc != SQLITE_OK)
    {
        return false;
    }
    return true;
}
bool DBSqliteStmt::reset()
{
    int rc = sqlite3_reset(this->stmt);
    if (rc != SQLITE_OK)
    {
        return false;
    }
    return true;
}
bool DBSqliteStmt::clearBindings()
{
    int rc = sqlite3_clear_bindings(this->stmt);
    if (rc != SQLITE_OK)
    {
        return false;
    }
    return true;
}
bool DBSqliteStmt::close()
{
    int rc = sqlite3_finalize(this->stmt);
    if (rc != SQLITE_OK)
    {
        return false;
    }
    return true;
}
#endif

#ifdef _X11
DBSqlite::DBSqlite():db(NULL){}
DBSqlite::DBSqlite(const QString& filename):db(NULL)
{
    this->open(filename);
}
int DBSqlite::open(const QString& filename)
{
    if (this->db != NULL)
    {
        this->db->close();
    }
    this->db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE",filename));
    this->db->setDatabaseName(filename);
    if (!this->db->open())
    {
#ifdef DEBUG
        fprintf(stderr,"db opnen failed : %s",filename.toLocal8Bit().data());
#endif
        return this->db->lastError().number();
    }
}
int DBSqlite::close()
{
    if (this->db != NULL)
    {
        this->db->close();
        this->db = NULL;
    }
    return 0;

}
DBSqlite::~DBSqlite()
{
    this->close();
}
int DBSqlite::exec(const QString& sql)
{
    QSqlQuery query(*this->db);
    if(query.exec(sql))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


DBSqliteStmt DBSqlite::prepare(const QString& zSql)
{
    QSqlQuery *query = new QSqlQuery(*this->db);
    query->prepare(zSql);
    return DBSqliteStmt(query);
}

DBSqliteStmt::DBSqliteStmt(QSqlQuery *other):query(other),isExeced(false){}
DBSqliteStmt::DBSqliteStmt(const DBSqliteStmt& origin):query(origin.query),isExeced(origin.isExeced){}
bool DBSqliteStmt::close()
{
    delete this->query;
}
bool DBSqliteStmt::exec()
{
    if (!this->isExeced)
    {
        return this->isExeced=this->query->exec();
    }
    return this->isExeced;
}
bool DBSqliteStmt::next()
{
    if (!this->isExeced)
    {
        this->exec();
    }
    return this->query->next();
}
bool DBSqliteStmt::bindInt(int iCol, int val)
{
    this->query->bindValue(iCol,val);
    return true;
}
bool DBSqliteStmt::bindDouble(int iCol, double val)
{
    this->query->bindValue(iCol,val);
    return true;
}
bool DBSqliteStmt::bindString(int iCol, const QString& val)
{
    this->query->bindValue(iCol,val);
    return true;
}
int DBSqliteStmt::getInt(int iCol)
{
    return this->query->value(iCol).toInt();
}
double DBSqliteStmt::getDouble(int iCol)
{
    return this->query->value(iCol).toDouble();
}
QString DBSqliteStmt::getString(int iCol)
{
    return this->query->value(iCol).toString();
}
bool DBSqliteStmt::reset()
{
    return false;
}
bool DBSqliteStmt::clearBindings()
{
    return false;
}
#endif
