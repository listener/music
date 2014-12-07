#ifndef MUSIC_H
#define MUSIC_H

#include <QtWidgets/QMainWindow>
#include <QMediaPlayer>
#include <QDir>
#include <QFileDialog>
#include <QMediaPlaylist>
#include <QSettings>
#include <QThread>
#include "csqlite3.h"

class Music;

class UpdateUI : public QThread
{
    Q_OBJECT
signals:
    void UpdateSignal(QStringList);
public:
    UpdateUI(QObject *parent=0);
    ~UpdateUI();
    void setStrings(QStringList&);
protected:
    virtual void run();
private:
    QStringList pathList_;
    Music   *music;
};

QStringList ToStringList(QList<QFileInfo> *fileInfolist);

namespace Ui {
class Music;
}

enum class Mode{ Repeat,Once,Shuffle };

class Music : public QMainWindow
{
    Q_OBJECT

public:
    explicit Music(QWidget *parent = 0);
    ~Music();
    unsigned int vol;

public slots:
    void on_btnStop_clicked();

protected:
    void changeEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
private slots:
    void add_file_triggered();
    void add_folder_triggered();
    void clear_clicked();

    void on_btnPause_clicked();
    void on_vol_valueChanged(int value);
    void on_locate_valueChanged(int value);

    void on_listMusic_doubleClicked(const QModelIndex &);
    void positionChanged(qint64);
    void durationChanged(qint64);
    void setPosition(int);

    void once_clicked();
    void repeat_clicked();
    void shuffle_clicked();

    void listMusic_customContextMenuRequested(const QPoint &pos);
    void del_file();

    void UpdateSignal(QStringList);
private:
    void playFile(int index);
    void setConnect();
    void getSettings(QString& path);

public:
    CSqlite3    *csqlite3;
private:
    Ui::Music       *ui;
    QMediaPlayer    *player;
    QMediaPlaylist  *playlist;
    QSettings       *inifile;
    UpdateUI        *thread;
    Mode mode=Mode::Shuffle;
};


#endif // MUSIC_H










