#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_bChooseImageFile_clicked();
    void on_rbUploadProgram_toggled(bool checked);
    void on_rbUploadSpiffs_toggled(bool checked);
    void on_cbSelectDevice_currentIndexChanged(int index);
    void on_bAddNewDevice_clicked();
    void on_bClearAllDevices_clicked();
    void on_cDebug_toggled(bool checked);
    void on_sbUploadPort_valueChanged(int arg1);
    void on_leUploadPasswd_textChanged(const QString &arg1);
    void on_bUpload_clicked();
    void readProcessData();

    void on_bClearOutput_clicked();

    void on_twUploadMethod_currentChanged(int index);

    void on_sbComPort_valueChanged(int arg1);

    void on_cbUploadSpeed_currentIndexChanged(int index);

    void on_cbEraseFlash_toggled(bool checked);

    void on_bRemoveDevice_clicked();

private:
    QProcess *process;
    char last_char;
    int upload_progress;
    Ui::MainWindow *ui;
    void loadConfig();
    void saveConfig();
    void updateDeviceSelector();
    void addDevice(const QString &name, const QString &ip);
};

#endif // MAINWINDOW_H
