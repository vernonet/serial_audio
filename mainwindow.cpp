#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtMultimedia/QAudioOutput>
//#include <QtMultimedia/QAudioFormat>
#include <QDebug>
#include <QIODevice>
#include <QBuffer>
#include <QFile>
#include <QCoreApplication>
#include <QApplication>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QTimer>
#include <QTime>



#define BAUDRATE     (921600)    //500000  for  samplerate 22050
#define PLAY_TIME    (600)       //sec   0 - unlimited

void delay( int millisecondsToWait );

QFile sourceFile;
QByteArray arrey_0, arrey_1;
QBuffer file;
QAudioOutput* audio;
QSerialPort serial;
QTimer *tmr;
volatile bool first, shift = false;
int posic = 0;
uint32_t smp_rate=0;
uint8_t bits_per_sample = 16;
uint32_t baud_rate=500000;
uint32_t play_size=0;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Serial Audio"));
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ui->comboBox_port->addItem(info.portName());
    ui->comboBox_port->setFocus();
    ui->comboBox_s_rate->addItem("22050");
    ui->comboBox_s_rate->addItem("16029");
    ui->comboBox_s_rate->addItem("37400");

    ui->pushButton_2->setToolTip("if noise or silence, click once or more");
    ui->pushButton_2->setToolTipDuration(3000);

    ui->comboBox_bits->addItem("16");
    ui->comboBox_bits->addItem("24");
    ui->comboBox_bits->setToolTip("bits per sample");
    ui->comboBox_bits->setToolTipDuration(3000);

    ui->comboBox_b_rate->addItem("500000");
    ui->comboBox_b_rate->addItem("921600");
    ui->comboBox_b_rate->setToolTip("921600 optimal for sample rate > 22050 or bits > 16, otherwise you can set 500000");
    ui->comboBox_b_rate->setToolTipDuration(4000);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

    tmr = new QTimer();        //timer for reading serial data
    tmr->setInterval(400);     //400ms
    connect(tmr, SIGNAL(timeout()), this, SLOT(ReadInPort()));
    //tmr->start();

    QAudioFormat format;
    // Set up the format, eg.
    if (smp_rate) format.setSampleRate(smp_rate);
        else format.setSampleRate(22050);
    format.setChannelCount(1);
    format.setSampleSize(bits_per_sample);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
        if (!info.isFormatSupported(format)) {
            qWarning() << "Raw audio format not supported by backend, cannot play audio.";
            return;
        }


        audio = new QAudioOutput(format);
        audio->setBufferSize(16384);
        audio->setVolume(0.1);
        connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
        serial.setBaudRate(baud_rate);
        if (!serial.open(QIODevice::ReadWrite)) {
            emit error(tr("Can't open %1, error code %2")
                       .arg(serial.portName()).arg(serial.error()));
            return;
        }
        delay(1000);
        arrey_0 = serial.readAll();
        int p_ff = arrey_0.indexOf(0xff);
        posic = arrey_0.size();
        qDebug() << "posic = " << posic <<"  0xFF pos -> " << p_ff ;
        play_size = smp_rate * bits_per_sample/8 * PLAY_TIME;
        qDebug() << "play_size = " <<play_size;
 #if (PLAY_TIME)
        arrey_0.resize(play_size);     //50000000
 #else
         arrey_0.resize(50000000);
 #endif
        file.setBuffer(&arrey_0);
        file.open(QIODevice::ReadOnly);

        if(p_ff>=0) file.seek(p_ff+2);
        tmr->start();
        audio->start(&file);

}

void MainWindow::handleStateChanged(QAudio::State state) {
            qWarning() << "state = " << state;
            switch (state) {
                    case QAudio::IdleState:
                        // Finished playing (no more data)
                        audio->stop();
                        file.close();
                        delete audio;
                        break;

                    case QAudio::StoppedState:
                        // Stopped for other reasons
                        if (audio->error() != QAudio::NoError) {
                            // Error handling
                        }
                        qApp->exit();
                        break;

                    default:
                        // ... other cases as appropriate
                        break;
                }
        }

void MainWindow::on_comboBox_port_currentIndexChanged(const QString &arg1)
{
  QString currentPortName;
  currentPortName = arg1;
  serial.close();
  serial.setPortName(currentPortName);
  serial.setBaudRate(baud_rate);//230400
}

void MainWindow::ReadInPort(){//
   QByteArray data;
#if PLAY_TIME != 0
   if ((uint32_t)posic>=play_size) return;         //play end
#endif
   data = serial.readAll();
   if (shift) {
       data = data.mid(1);
       shift = false;
   }
   arrey_0.insert(posic,data);
   posic+= data.size();
   qDebug() << "ReadIn size = " << data.size();
}


void MainWindow::on_comboBox_s_rate_currentIndexChanged(const QString &arg1)
{
    smp_rate = arg1.toUInt(nullptr);
    qDebug() << "smp_rate = " << smp_rate;
}

void delay( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    shift = true;
}

void MainWindow::on_comboBox_bits_currentIndexChanged(const QString &arg1)
{
   bits_per_sample = arg1.toUInt(nullptr);
}

void MainWindow::on_comboBox_b_rate_currentIndexChanged(const QString &arg1)
{
   baud_rate = arg1.toUInt(nullptr);
}
