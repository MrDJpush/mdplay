#ifndef MDPLAYER_H
#define MDPLAYER_H

#include <QWidget>
#include<QString>
#include<QDir>
#include<QStringList>
#include<QDebug>
#include <QFileDialog>
#include<QProcess>
#include<QTimer>
#include<QDateTime>
#include <QFile>
#include <QTextStream>
#include<QList>
namespace Ui {
class mdplayer;
}

class mdplayer : public QWidget
{
    Q_OBJECT

public:
    explicit mdplayer(QWidget *parent = 0);

    void show_music_name();

    ~mdplayer();

    void play();

public slots:
    void onButtonClicked(int id);
    void on_timeout();
    void play_now();
protected:
    void song_word();
private slots:

    void on_pushButton_4_clicked();

    void on_pushButton_play_clicked();

    void on_started();
    void on_finished(int);
    void on_readyread();


    void on_horizontalSlider_voice_valueChanged(int value);

    void on_listWidget_name_doubleClicked(const QModelIndex &index);

    void on_horizontalSlider_sliderReleased();

private:
    QStringList list ;
    Ui::mdplayer *ui;
    QString music_path;
    QProcess * process;
    int index;
    QTimer * time;
    int play_n;
};

#endif // MDPLAYER_H
