#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QProcess>
#include <QDebug>

#include "variables.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loadConfig();
}

MainWindow::~MainWindow()
{
    saveConfig();
    delete ui;
}

void MainWindow::loadConfig(){
    QSettings setup("OTA-Config.ini", QSettings::IniFormat);
    qDebug() << "loadConfig";

    esp.upload_port = setup.value ("esp/port", 8266).toInt ();
    ui->sbUploadPort->setValue (esp.upload_port);

    esp.spiffs = setup.value ("esp/spiffs", false).toBool ();
    if(esp.spiffs){
        ui->rbUploadSpiffs->setChecked (true);
    } else{
        ui->rbUploadProgram->setChecked (true);
    }

    esp.upload_passwd = setup.value ("esp/passwd", "").toString ();
    ui->leUploadPasswd->setText (esp.upload_passwd);

    esp.image_file = setup.value ("esp/image", "").toString ();
    ui->leImagePath->setText (esp.image_file);

    esp.debug = setup.value ("esp/debug", false).toBool ();
    ui->cDebug->setChecked (esp.debug);

    int dev_length = setup.beginReadArray ("esp/devices");
    for (int i = 0; i < dev_length; ++i) {
        setup.setArrayIndex(i);
        QString name = setup.value ("name").toString ();
        QString ip = setup.value ("ip").toString ();
        addDevice (name, ip);
    }
    setup.endArray ();

    esp.selected_device = setup.value ("esp/selected", -1).toInt ();
    ui->cbSelectDevice->setCurrentIndex (esp.selected_device);

    esp.com_port = setup.value ("esp/com_port",0).toInt ();
    ui->sbComPort->setValue (esp.com_port);

    esp.up_method = setup.value ("esp/up_method", 1).toInt ();
    ui->twUploadMethod->setCurrentIndex (esp.up_method);

    esp.upload_speed = setup.value ("esp/up_speed",115200).toInt ();
    ui->cbUploadSpeed->setCurrentText (QString::number (esp.upload_speed));

    esp.erase_flash = setup.value ("esp/erase_flash",false).toBool ();
    ui->cbEraseFlash->setChecked (esp.erase_flash);

    ui->twTabs->setCurrentIndex (0);
}

void MainWindow::saveConfig(){
    QSettings setup("OTA-Config.ini", QSettings::IniFormat);
    qDebug() << "saveConfig";

    setup.setValue ("esp/port", esp.upload_port);
    setup.setValue ("esp/passwd", esp.upload_passwd);
    setup.setValue ("esp/spiffs", esp.spiffs);
    setup.setValue ("esp/debug", esp.debug);
    setup.setValue ("esp/image", esp.image_file);
    setup.setValue ("esp/selected", esp.selected_device);
    setup.setValue ("esp/up_method", esp.up_method);
    setup.setValue ("esp/com_port", esp.com_port);
    setup.setValue ("esp/up_speed", esp.upload_speed);
    setup.setValue ("esp/erase_flash", esp.erase_flash);

    setup.beginWriteArray ("esp/devices", esp.device_names.length ());
    for (int i = 0; i < esp.device_names.length (); ++i) {
        setup.setArrayIndex (i);
        setup.setValue ("name", esp.device_names.at (i));
        setup.setValue ("ip", esp.device_ips.at (i));
    }
    setup.endArray ();

    setup.sync();
}

void MainWindow::updateDeviceSelector (){
    ui->cbSelectDevice->clear ();
    for (int i = 0; i < ui->twDevices->rowCount (); i++) {
        QString dev = ui->twDevices->item(i, 0)->text () + ": " + ui->twDevices->item(i, 1)->text ();
        ui->cbSelectDevice->addItem (dev);
    }
}

void MainWindow::addDevice (const QString &name, const QString &ip){
    int curr_row = ui->twDevices->rowCount ();
    ui->twDevices->insertRow (curr_row);
    ui->twDevices->setItem (curr_row, 0, new QTableWidgetItem(name));
    ui->twDevices->setItem (curr_row, 1, new QTableWidgetItem(ip));
    esp.device_names.append (name);
    esp.device_ips.append (ip);
    updateDeviceSelector ();
}

void MainWindow::on_bChooseImageFile_clicked()
{
    QString image_path = QFileDialog::getOpenFileName (this, "Select ESP Image File","","Binary Image (*.bin)");
    ui->leImagePath->setText (image_path);
    esp.image_file = image_path;
}

void MainWindow::on_rbUploadProgram_toggled(bool checked)
{
    if(checked){
        esp.spiffs = false;
    }
}

void MainWindow::on_rbUploadSpiffs_toggled(bool checked)
{
    if(checked){
        esp.spiffs = true;
    }
}

void MainWindow::on_cbSelectDevice_currentIndexChanged(int index)
{
    if(index >= 0 && index < ui->twDevices->rowCount() ){
        esp.selected_device = index;
    }
    else{
        esp.selected_device = -1;
    }
}

void MainWindow::on_bAddNewDevice_clicked()
{
    QString new_dev_name = ui->leNewDeviceName->text ();
    int ip0 = ui->sbNewDeviceIP0->value ();
    int ip1 = ui->sbNewDeviceIP1->value ();
    int ip2 = ui->sbNewDeviceIP2->value ();
    int ip3 = ui->sbNewDeviceIP3->value ();
    QString new_dev_ip = "" + QString::number(ip0) + "." + QString::number(ip1) + "." + QString::number(ip2) + "." + QString::number(ip3);

    if(new_dev_name.length () == 0){
        QMessageBox::critical (this, "Invalid Input", "The Device Name is Empty!");
    }
    else{
        ui->leNewDeviceName->clear ();

        addDevice (new_dev_name, new_dev_ip);
    }

}

void MainWindow::on_bRemoveDevice_clicked()
{
    int row = ui->twDevices->currentRow ();
    if(row >= 0){
        QTableWidgetItem *item_name = ui->twDevices->item (row,0);
        QTableWidgetItem *item_ip = ui->twDevices->item (row,1);

        QMessageBox msgBox;
        msgBox.setWindowTitle("Delete device?");
        msgBox.setText(QString("Do you want to delete device \"") + item_name->text() + "\" with ip \"" + item_ip->text() + "\" ?");
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Cancel);

        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Ok:

            // remove single device
            esp.device_names.removeOne (item_name->text ());
            esp.device_ips.removeOne (item_ip->text ());

            ui->twDevices->removeRow (row);

            QMessageBox::information (this, "Removed", "Device was removed");
            break;
        case QMessageBox::Cancel:
            break;
        }
    }
}

void MainWindow::on_bClearAllDevices_clicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Delete ALL devices?");
    msgBox.setText("Do you want to delete all devices?");
    msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Ok:

        ui->twDevices->setRowCount (0);
        esp.device_names.clear ();
        esp.device_ips.clear ();
        updateDeviceSelector();

        QMessageBox::information (this, "Cleared", "All devices were removed");
        break;
    case QMessageBox::Cancel:
        break;
    }
}

void MainWindow::on_cDebug_toggled(bool checked)
{
    esp.debug = checked;
}

void MainWindow::on_sbUploadPort_valueChanged(int arg1)
{
    esp.upload_port = arg1;
}

void MainWindow::on_leUploadPasswd_textChanged(const QString &arg1)
{
    esp.upload_passwd = arg1;
}

void MainWindow::on_bClearOutput_clicked()
{
    ui->teConsoleOutput->clear ();
    ui->pbProgress->reset ();
}

void MainWindow::readProcessData (){
    QByteArray output = process->readAllStandardOutput ();
    output.replace (' ',"  ");

    if(last_char == ' '){
        ui->teConsoleOutput->undo ();
    }
    last_char = output.at (output.length () - 1);

    int index_bracket = output.indexOf (']');
    int index_percent = output.indexOf ('%');
    if(index_bracket > 70 && index_percent > 70) // progress
    {
        QString percent = output.mid (index_bracket + 3, index_percent - index_bracket - 3);
        ui->pbProgress->setValue (percent.toInt ());
    }

    if(output.contains ("[ERROR]:  No  Answer")){
        QMessageBox::critical (this, "ESP Unavailabe", "Failed to reach ESP Module!");
    }

    if(output.contains ("[INFO]:  Result:  OK")){
        QMessageBox::information (this, "Upload sucessfull", "Image uploaded sucessfully");
    }

    ui->teConsoleOutput->moveCursor (QTextCursor::End);
    ui->teConsoleOutput->insertPlainText (output);
}

void MainWindow::on_twUploadMethod_currentChanged(int index)
{
    esp.up_method = index;
}

void MainWindow::on_sbComPort_valueChanged(int arg1)
{
    esp.com_port = arg1;
}

void MainWindow::on_bUpload_clicked()
{
    ui->teConsoleOutput->clear ();
    ui->pbProgress->reset ();

    bool valid = true;
    if(!(esp.device_names.length () > 0 && esp.selected_device >= 0 && esp.selected_device < esp.device_names.length ())){
        valid = false;
        QMessageBox::critical (this, "Invalid Config", "No Devices Added");
    }

    if(!(esp.image_file.length () > 0)){
        valid = false;
        QMessageBox::critical (this, "Invalid Config", "Missing Binary Image File");
    }

    if (valid){
        QString program;
        QStringList args;

        if(esp.up_method == 1){
            program = "python.exe";
            args << QDir::currentPath() + "/espota.py";
            args << "--ip" << ui->twDevices->item (esp.selected_device, 1)->text();
            args << "--port" << QString::number(ui->sbUploadPort->value());
            args << "--auth" << ui->leUploadPasswd->text();
            args << "--file" << ui->leImagePath->text();
            args << "--progress";
            if(esp.debug) args << "--debug";
            if(esp.spiffs) args << "--spiffs";
        }
        else{
            program = "esptool.exe";
            args << "-vv"; // verbose output
            args << "-cd" << "nodemcu";
            args << "-cb" << QString::number (esp.upload_speed);
            args << "-cp" << QString("COM") + QString::number (esp.com_port);
            args << "-ca" << (esp.spiffs ? "0x300000" : "0x00000");
            if(esp.erase_flash) args << "-ca" << "0x0" <<  "-cz" << "0x400000";
            args << "-cf" << ui->leImagePath->text ();
        }


        process = new QProcess(this);
        process->setProcessChannelMode (QProcess::MergedChannels);
        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessData()));
        process->start (program, args);
        ui->teConsoleOutput->insertPlainText ("starting upload\n\n");
    }
}

void MainWindow::on_cbUploadSpeed_currentIndexChanged(int index)
{
    int speed = ui->cbUploadSpeed->itemText (index).toInt ();
    esp.upload_speed = speed;
}

void MainWindow::on_cbEraseFlash_toggled(bool checked)
{
    esp.erase_flash = checked;
}
