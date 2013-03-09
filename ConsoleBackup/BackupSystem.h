#ifndef BACKUPSYSTEM_H
#define BACKUPSYSTEM_H

#include <QCoreApplication>
#include <QDataStream>
#include <QDirIterator>
#include <QDebug>
#include <QFile>
#include <QFileSystemWatcher>
#include <QString>
#include <QTextStream>
#include <QVariant>
#include <QVariantMap>
#include <vector>

#include "Backup.h"
#include "json.h"

class BackupSystem {
private:
    std::vector<Backup> backups;
    QFileSystemWatcher watcher;
public:
    BackupSystem();
    void addBackup(Backup);
    void load();
    void addBackupsToWatcher();
};

#endif // BACKUPSYSTEM_H
