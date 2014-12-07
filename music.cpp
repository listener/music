
#include "music.h"
#include "ui_music.h"
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include <QMouseEvent>

UpdateUI::UpdateUI(QObject *parent)
{
    music = dynamic_cast<Music*>(parent);
    connect(this, SIGNAL(UpdateSignal(QStringList)), music, SLOT(UpdateSignal(QStringList)));
}

UpdateUI::~UpdateUI()
{
}

void UpdateUI::run()
{
    emit UpdateSignal(pathList_); //update ui in main thread use signal/slots
    for (int i = 0; i < pathList_.size(); ++i)
    {
        QString path = QDir::toNativeSeparators(pathList_.at(i));
        QString fileName = path.split("/").last();
        (*(music->csqlite3))->execDML(QString ("insert into playlist values(\"" + fileName + "\",\"" + path + "\");").toStdString().c_str());
    }
}

void UpdateUI::setStrings(QStringList& pathList)
{
    pathList_ = pathList;
}


QStringList ToStringList(QList<QFileInfo> *fileInfolist)
{
    QStringList pathlist;
    int count = fileInfolist->count();
    if (count >= 1)
    {
        for (int i = 0; i != count; ++i)
        {
            pathlist.append(fileInfolist->at(i).filePath());
        }
    }
    return pathlist;
}

Music::Music(QWidget * parent):
    QMainWindow(parent), ui(new Ui::Music)
{
    ui->setupUi(this);
    thread = new UpdateUI(this);
    QString path = QCoreApplication::applicationDirPath();
    csqlite3 = new CSqlite3((path + "/musiclist.db").toStdString().c_str());

    MFileList fileInfo = csqlite3->getFileList();
    playlist = new QMediaPlaylist;
    for (int i = 0; i < fileInfo.count(); ++i)
    {
        ui->listMusic->addItem(QString(fileInfo.at(i).name));
        playlist->addMedia(QUrl::fromLocalFile(fileInfo.at(i).path));
    }
    playlist->setPlaybackMode(QMediaPlaylist::Random);
    player = new QMediaPlayer;
    player->setPlaylist(playlist);

    this->getSettings(path);
    this->setConnect();

}

void Music::setConnect()
{
    connect(ui->actionAddfile, SIGNAL(triggered()), this, SLOT(add_file_triggered()));
    connect(ui->actionAddFolder, SIGNAL(triggered()), this, SLOT(add_folder_triggered()));
    connect(ui->actionClear, SIGNAL(triggered()), this, SLOT(clear_clicked()));
    connect(ui->action_once, SIGNAL(triggered()), this, SLOT(once_clicked()));
    connect(ui->action_repeat, SIGNAL(triggered()), this, SLOT(repeat_clicked()));
    connect(ui->action_shuffle, SIGNAL(triggered()), this, SLOT(shuffle_clicked()));
    connect(ui->locate, SIGNAL(sliderMoved(int)), this, SLOT(setPosition(int)));

    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));

    connect(ui->listMusic,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(listMusic_customContextMenuRequested(QPoint)));
}

void Music::getSettings(QString& path)
{
    inifile = new QSettings(path+"/setting.ini",QSettings::IniFormat);
    if(inifile->value("settings/vol").toString() == "")
    {
        inifile->setValue("settings/vol",50);
        ui->vol->setValue(50);
    }
    else
    {
        Music::vol = inifile->value("settings/vol").toInt();
        ui->vol->setValue(Music::vol);
    }
}

Music::~Music()
{
    delete player;
    inifile->setValue("settings/vol",Music::vol);
    delete ui;
    delete csqlite3;
    delete inifile;
    delete thread;
}

void Music::changeEvent(QEvent * e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Music::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        if ( QApplication::keyboardModifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
        {
            QMessageBox::about(this,"  ","I love you !");
        }
    }
}
void Music::UpdateSignal(QStringList pathList)
{
    for (int i = 0; i < pathList.size(); ++i)
    {
        QString path = QDir::toNativeSeparators(pathList.at(i));
        QString fileName = path.split("/").last();
        ui->listMusic->addItem(fileName);
        playlist->addMedia(QUrl::fromLocalFile(path));
    }
}

void Music::add_file_triggered()
{
    QStringList pathList = QFileDialog::getOpenFileNames(this, tr("选择文件"), QDir::homePath(), tr("Mp3 file(*.mp3)"));

    thread->setStrings(pathList);
    thread->start();
}

void Music::add_folder_triggered()
{
    auto path = QFileDialog::getExistingDirectory(NULL, tr("选择mp3所在文件夹"), QDir::homePath(), QFileDialog::ShowDirsOnly);
    QDir *dir = new QDir(path);
    QStringList filter;

    filter << "*.mp3";
    dir->setNameFilters(filter);
    QList<QFileInfo> *fileInfolist = new QList<QFileInfo>(dir->entryInfoList(filter));

    auto a = ToStringList(fileInfolist);
    thread->setStrings(a);
    thread->start();
}

void Music::playFile(int index)
{
    ui->lblMusic->setText(ui->listMusic->item(index)->data(0).toString());
    playlist->setCurrentIndex(index);
    player->setVolume(Music::vol);
    player->play();
    ui->btnPause->setText("Pause");
}

void Music::on_btnPause_clicked()
{
    if (ui->btnPause->text() == "Pause")
    {
        ui->btnPause->setText("Resume");
        player->pause();
    }
    else if (ui->btnPause->text() == "Resume")
    {
        player->play();
        ui->btnPause->setText("Pause");
    }
    else if(ui->listMusic->count() > 0)
    {
        qsrand(time(NULL));
        auto i = qrand() % ui->listMusic->count();
        playFile(i);
        ui->btnPause->setText("Pause");
    }
    return;
}

void Music::on_vol_valueChanged(int value)
{
    Music::vol = value;
    player->setVolume(value);
}

void Music::setPosition(int position)
{
    player->setPosition(position);
}

void Music::clear_clicked()
{
    player->stop();
    ui->lblMusic->clear();
    ui->locate->setValue(0);
    ui->btnPause->setText("Play");
    ui->listMusic->clear();
    playlist->clear();
    (*csqlite3)->execDML("delete from playlist;");
}

void Music::positionChanged(qint64 position)
{
    ui->locate->setValue(position);
}

void Music::durationChanged(qint64 duration)
{
    ui->locate->setRange(0, duration);
}

void Music::on_listMusic_doubleClicked(const QModelIndex &)
{
    playFile(ui->listMusic->currentRow());
}

void Music::on_btnStop_clicked()
{
    int i = ui->listMusic->currentRow();

    if (mode == Mode::Once);
    else if (mode == Mode::Repeat)
    {
        i == ui->listMusic->count() - 1 ? i = 0 : ++i;
    }
    else if (mode == Mode::Shuffle)
    {
        qsrand(time(NULL));
        i = qrand() % ui->listMusic->count();
    }
    ui->listMusic->setCurrentRow(i);
    playFile(ui->listMusic->currentRow());
}

void Music::on_locate_valueChanged(int value)
{
    if (value == player->duration())
    {
        QtConcurrent::run([&]()
        {
            this->on_btnStop_clicked();
        }
        );
    }
}

void Music::once_clicked()
{
    ui->action_once->setChecked(true);
    ui->action_repeat->setChecked(false);
    ui->action_shuffle->setChecked(false);
    mode = Mode::Once;
}

void Music::repeat_clicked()
{
    ui->action_repeat->setChecked(true);
    ui->action_once->setChecked(false);
    ui->action_shuffle->setChecked(false);
    mode = Mode::Repeat;
}

void Music::shuffle_clicked()
{
    ui->action_shuffle->setChecked(true);
    ui->action_once->setChecked(false);
    ui->action_repeat->setChecked(false);
    mode = Mode::Shuffle;
}
void Music::del_file()
{
    QListWidgetItem *item = ui->listMusic->takeItem(ui->listMusic->currentRow());
    QString name = item->data(0).toString();
    qDebug(name.toStdString().c_str());
    if(name == ui->lblMusic->text())
    {
        player->stop();
        ui->lblMusic->setText("");
        ui->btnPause->setText("Play");
    }
    playlist->removeMedia(ui->listMusic->currentRow());
    (*csqlite3)->execDML(QString("delete from playlist where name='"+name +"'").toStdString().c_str());
    delete item;
}

void Music::listMusic_customContextMenuRequested(const QPoint &pos)
{
    if(ui->listMusic->itemAt(pos) == NULL) return;

    QMenu *popMenu =new QMenu(this);
    popMenu->addAction("Delete");
    popMenu->addMenu(ui->menuFile);
    popMenu->addMenu(ui->menuPre);

    connect(popMenu->actions().at(0),SIGNAL(triggered()),this,SLOT(del_file()));

    popMenu->exec(QCursor::pos());//弹出右键菜单，菜单位置为光标位置
    popMenu->close();
    delete popMenu;
}
