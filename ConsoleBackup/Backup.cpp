#include "Backup.h"

Backup::Backup(QString name, QString path, QString backupPath) {
    this->name = name;
    this->mainPath = path;
    this->backupPath = backupPath;
}

QString Backup::getMainPath() { return this->mainPath; }
QString Backup::getBackupPath() { return this->backupPath; }
