#include "mdplayer.h"
#include "ui_mdplayer.h"

mdplayer::mdplayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mdplayer)
{
    ui->setupUi(this);
    music_path="/mnt/hgfs/cs2307/QT/MP3";
    show_music_name();
    index=0;
    play_n=0;
    process = new QProcess(this);
    QStringList arg;
    process->setProgram("/usr/bin/mplayer");
    arg << "-quiet" << "-slave" << "/mnt/hgfs/cs2307/QT/MP3/2.mp3";
    process->setArguments(arg);
    connect(process,SIGNAL(started()),this,SLOT(on_started()));
    connect(ui->buttonGroup,SIGNAL(buttonClicked(int)),
            this,SLOT(onButtonClicked(int))
            );
    ui->horizontalSlider_voice->setValue(99);
    ui->label_nowplay->setText("正在播放"+list.at(index));
    QTimer * date_time = new QTimer(this);
    time = new QTimer(this);
    time->setInterval(1000);//设置间隔时间，以毫秒为单位
    connect(date_time,SIGNAL(timeout()),this,SLOT(on_timeout()));
    connect(time,SIGNAL(timeout()),this,SLOT(play_now()));
    date_time->start();
}

//显示歌曲列表
void mdplayer::show_music_name()
{
    list.clear();
    ui->listWidget_name->clear();
    QDir dir(music_path);
    QStringList filters;
    filters<<"*.mp3";
    list =dir.entryList(filters);
    ui->listWidget_name->addItems(list);
}

mdplayer::~mdplayer()
{
    delete ui;
}


void mdplayer::play()
{
    process->setProgram("/usr/bin/mplayer");
    QStringList arg;
    QString pathname = music_path +"/" +list.at(index);
    arg << "-quiet" << "-slave" << pathname;
    process->setArguments(arg);
    process->start();
    ui->label_nowplay->setText("正在播放:"+list.at(index));
}

void mdplayer::on_pushButton_4_clicked()
{
    QString temppath=QFileDialog::getExistingDirectory();
    if(!temppath.isEmpty())
        music_path=temppath;
    show_music_name();
}

void mdplayer::on_pushButton_play_clicked()
{
    //分两种清情况：1,进程还没启动; 2,已经启动了
    if(process->state() != QProcess::Running)//还没有启动
    {
        play();
    }
    else//已经启动了
    {
        if(ui->pushButton_play->text() == "播放")
        {
            time->stop();
            ui->pushButton_play->setText("暂停");
        }
        else
        {
            time->start();
            ui->pushButton_play->setText("播放");
        }
        process->write("pause\n");
    }
}

void mdplayer::on_started()
{
    song_word();
    process->write("get_time_length\n");
    //如果进程成功启动，就会调用该槽函数
    qDebug() << "启动成功";
    connect(process,SIGNAL(finished(int)),this,SLOT(on_finished(int)));
    connect(process,SIGNAL(readyRead()),this,SLOT(on_readyread()));
    time->start();//启动定时器
}

void mdplayer::on_finished(int x)
{
    //自动播放下一首
    index ++;
    if(index == list.size())
        index = 0;
    song_word();
    play();
}

void mdplayer::on_readyread()
{
    //说明 mplayer输出了数据(process有数据可读了)
    while(1)//读完为止
    {
        QString s = process->readLine();
        if(s.isEmpty())
            break;
        //qDebug() << s;
        if(s.contains("ANS_LENGTH="))
        {
            s.remove("ANS_LENGTH=");
            s.remove(".00\n");
            ui->label_total->setText(s);
        }
        if(s.contains("ANS_TIME_POSITION="))
        {
            s.remove("ANS_TIME_POSITION=");
            s.remove("\n");
            int i=s.toDouble();
            ui->label_now->setText(QString::number(i));
            int m=i/60;
            int n=i%60;
            QString s1;
            if(n<10)
                s1 = QString("[0") + QString::number(m) + ":" + QString("0")+QString::number(n);
            else
                s1 = QString("[0") + QString::number(m) + ":" + QString::number(n);
            QString searchItem = s1; // 要查找的元素
            //qDebug() << s1;
            QList<QListWidgetItem*> items = ui->listWidget_2->findItems(searchItem, Qt::MatchContains); // 查找匹配的项

            if (!items.isEmpty()) {
                QListWidgetItem* item = items.first(); // 获取第一个匹配的项
                int row = ui->listWidget_2->row(item); // 获取该项的行数
                //ui->listWidget_2->item(row)->setTextColor(QColor(0xff,0,0));//把第5行显示为红色
                ui->listWidget_2->setCurrentRow(row);

            }

        }
        if(s.contains("ANS_PERCENT_POSITION="))
        {
            s.remove("ANS_PERCENT_POSITION=");
            s.remove("\n");
            int j=s.toDouble();
            ui->horizontalSlider->setValue(j);
        }
    }
}

//上下曲按钮组
void mdplayer::onButtonClicked(int id)
{

    if(process->state() != QProcess::Running)//还没有启动
    {
        play();
    }
    QString text = ui->buttonGroup->button(id)->text();
    if(text=="上一曲")
    {
        if(index != 0)
            index --;
        else
            index=list.size()-1 ;
    }else
    {
        index ++;
        if(index == list.size())
            index = 0;
    }
    QString cmd = QString("loadfile ") + music_path+ "/"+ list[index] +"\n";
    process->write(cmd.toUtf8());
    process->write("get_time_length\n");//发送该命令，获取当前音乐总时长
    ui->label_nowplay->setText("正在播放:"+list.at(index));
    ui->pushButton_play->setText("暂停");
    song_word();
}

//改变音量
void mdplayer::on_horizontalSlider_voice_valueChanged(int value)
{
    QString cmd=QString("volum ")+QString::number(value)+QString(" 1\n");
    process->write(cmd.toUtf8());
    if(value==0)
        ui->label_voice->setText(QString::number(value));
    else
        ui->label_voice->setText(QString::number(value+1));
}

//双击歌曲名切换
void mdplayer::on_listWidget_name_doubleClicked(const QModelIndex &index)
{

    if(process->state() != QProcess::Running)//还没有启动
    {
        play();
    }
    this->index=index.row();
    QString name = list[this->index];
    QString cmd = QString("loadfile ") + music_path+ "/"+ name +"\n";
    process->write(cmd.toUtf8());
    process->write("get_time_length\n");//发送该命令，获取当前音乐总时长
    ui->label_nowplay->setText(QString("正在播放:")+list.at(this->index));
    song_word();
    ui->pushButton_play->setText("暂停");
}

void mdplayer::on_timeout()
{
    QString s= QDateTime::currentDateTime().toString();
    ui->label_date->setText(s);
}

void mdplayer::play_now()
{
    process->write("get_time_pos\n");
    process->write("get_percent_pos\n");
}



void mdplayer::on_horizontalSlider_sliderReleased()
{

    if(process->state() != QProcess::Running)//还没有启动
    {
        play();
    }
    int i=ui->horizontalSlider->value();
    QString cmd = QString("seek ") + QString::number(i) + " 1" + "\n";
    process->write(cmd.toUtf8());
}

void mdplayer::song_word()
{
    ui->listWidget_2->clear();
    QString song= "/mnt/hgfs/cs2307/QT/MP3/"+list.at(index);
    song.chop(4);
    song+=".lrc";
    QFile file(song);

    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    stream.setCodec("GBK");
    QString s1;
    while(1)
    {
    s1 = stream.readLine();
    //if()
    qDebug() << s1;

    ui->listWidget_2->addItem(s1);
    if(stream.atEnd())
        break;
    }
}













