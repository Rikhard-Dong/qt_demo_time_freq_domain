#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QUrl>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    playBtn = ui->playBtn;
    playBtn->setEnabled(false);

    // 时域
    timeDomainPlot = ui->timeDomainPlot;
    timeDomainPlot->addGraph();
    timeDomainPlot->xAxis->setLabel("Sample");
    timeDomainPlot->yAxis->setLabel("Amplitude");

    // 频域
    freqDomainPlot = ui->freqDomainPlot;
    freqDomainPlot->addGraph();
    freqDomainPlot->xAxis->setLabel("Frequency (Hz)");
    freqDomainPlot->yAxis->setLabel("Magnitude");

    player = new QMediaPlayer(this);
    timer = new QTimer(this);

    timeDomainData = QVector<double>();


    // 更新进度条
    connect(timer, &QTimer::timeout, this, &MainWindow::updateprogress);
    audioProbe = new QAudioProbe(this);
    audioProbe->setSource(player);
    connect(audioProbe, &QAudioProbe::audioBufferProbed, this, &MainWindow::processAudioBuffer);

    // 验证连接是否成功
     if (audioProbe->isActive()) {
         qDebug() << "Audio probe is active and connected to the player.";
     } else {
         qDebug() << "Failed to connect audio probe to the player.";
     }

    connect(timer, &QTimer::timeout, this, &MainWindow::updatePlots);


}

MainWindow::~MainWindow()
{
    delete ui;
}


// 选择音频
void MainWindow::on_chooseFileBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择wav文件", QString());
    if(!filePath.isEmpty())
    {
        player->setMedia(QUrl::fromLocalFile(filePath));
        playBtn->setEnabled(true);
    }
    else
    {
        QApplication::exit();
    }
}


// 开始播放
void MainWindow::on_playBtn_clicked()
{

    switch (player->state()) {
        case QMediaPlayer::PlayingState:
            qDebug() << "playing";
            break;
        case QMediaPlayer::PausedState:
            qDebug() << "Paused";
            break;
        case QMediaPlayer::StoppedState:
            qDebug() << "stopped";
            break;
    }

    if (player->state() == QMediaPlayer::PlayingState)
    {
        player->pause();
        timer->stop();
        playBtn->setText("播放");
    }
    else
    {
        player->play();
        timer->start(100);
        playBtn->setText("暂停");
    }
}

// 更新进度条
void MainWindow::updateprogress()
{
    int postion = player->position();
    int duration = player->duration();

    if(duration > 0) {
        int process = (postion * 100) / duration;
        ui->progressBar->setValue(process);
    }
}

// 处理音频缓冲区
void MainWindow::processAudioBuffer(const QAudioBuffer &buffer)
{
    qDebug() << "Process audio buffer";
    qDebug() << "Player state: " << player->state();
    if (player->state() == QMediaPlayer::PlayingState) {
        // 获取音频数据
        const qint16 *data = buffer.constData<qint16>();

        // 处理时域数据
        QVector<double> timeData;
        for (int i = 0; i < buffer.frameCount(); ++i)
        {
            timeData.append(data[i]);
        }
        timeDomainData.append(timeData);
    }
}

void MainWindow::updatePlots()
{
    // qDebug() << "updatePlots";
    // qDebug() << "timeDomainData size ==> " << timeDomainData.size();

    // 创建 x 值序列
    QVector<double> xValues(timeDomainData.size());
    for (int i = 0; i < timeDomainData.size(); ++i) {
        xValues[i] = i; // 使用数据点的索引作为 x 值
    }

    // 更新时域图表
    timeDomainPlot->graph(0)->setData(xValues ,timeDomainData);
    timeDomainPlot->rescaleAxes();
    timeDomainPlot->replot();
}

