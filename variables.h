#ifndef VARIABLES_H
#define VARIABLES_H

#include <QString>
#include <QList>
#include <QProcess>
#include <stdio.h>

struct {
    QString upload_passwd;
    int upload_port;
    bool spiffs;
    QString image_file;
    bool debug;
    int selected_device;
    QList<QString> device_names;
    QList<QString> device_ips;
    int up_method;
    int com_port;
    int upload_speed;
    bool erase_flash;
} esp;

QString console_output;
QProcess *process;



#endif // VARIABLES_H
