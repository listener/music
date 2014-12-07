#include "csqlite3.h"

CSqlite3::CSqlite3(const char *path)
{
    try{
        db.open(path);
        if( !db.tableExists("playlist") )
        {
            db.execDML("create table playlist(name text, empname text);");
        }
    }catch(CppSQLite3Exception)
    {
        db.execDML("create table playlist(name text, empname text);");
    }
}

MFileList CSqlite3::getFileList()
{
    CppSQLite3Query q = db.execQuery("select * from playlist;");
    MFileList *fileinfos = new MFileList();
    while (!q.eof())
    {
        file.name = QString(q.fieldValue(0));
        file.path = QString(q.fieldValue(1));

        fileinfos->push_back(file);
        q.nextRow();
    }

    return *fileinfos;
}

CSqlite3::~CSqlite3()
{
    db.close();
}
