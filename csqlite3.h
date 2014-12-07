#ifndef CMYSQL_H
#define CMYSQL_H
#include <cppsqlite3.h>
#include <QString>
#include <QList>

struct Mfileinfo
{
    QString name;
    QString path;
};

typedef QList<Mfileinfo> MFileList;

class CSqlite3
{
public:
    CSqlite3(const char* path);
    ~CSqlite3();
    MFileList getFileList();
    CppSQLite3DB* operator->()
    {
        return &db;
    }
private:
    CSqlite3();
    CSqlite3& operator=(const CSqlite3&);
private:
    Mfileinfo file;
    CppSQLite3DB db;
};

#endif // CMYSQL_H
