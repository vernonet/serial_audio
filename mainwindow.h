#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia/QAudioOutput>
#include <QDebug>
#include <QCoreApplication>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void error(const QString &s);

private slots:
    void on_pushButton_clicked();
    void handleStateChanged(QAudio::State state);
    void on_comboBox_port_currentIndexChanged(const QString &arg1);
    void ReadInPort();

    void on_comboBox_s_rate_currentIndexChanged(const QString &arg1);

    void on_pushButton_2_clicked();

    void on_comboBox_bits_currentIndexChanged(const QString &arg1);

    void on_comboBox_b_rate_currentIndexChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
   // QAudioOutput* audio;
};



#endif // MAINWINDOW_H

 
