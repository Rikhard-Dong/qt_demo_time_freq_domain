#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QTimer>
#include <QPushButton>
#include <QAudioBuffer>
#include <QAudioProbe>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_chooseFileBtn_clicked();

    void on_playBtn_clicked();

    // 更新进度条
    void updateprogress();

    // 处理音频缓冲区
    void processAudioBuffer(const QAudioBuffer &buffer);

    // 更新图表
    void updatePlots();

private:
    Ui::MainWindow *ui;
    QPushButton *playBtn;

    QMediaPlayer *player;
    QTimer *timer;
    QAudioProbe *audioProbe;
    QCustomPlot *timeDomainPlot;
    QCustomPlot *freqDomainPlot;

    QVector<double> timeDomainData;


};
#endif // MAINWINDOW_H
