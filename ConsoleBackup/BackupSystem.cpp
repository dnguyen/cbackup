#include "BackupSystem.h"
#include "BackupPathsWatcher.h"

using namespace std;
using namespace QtJson;

BackupSystem::BackupSystem() {
    BackupPathsWatcher* backupWatcher = new BackupPathsWatcher;

    QObject::connect(&this->watcher, SIGNAL(fileChanged(QString)), backupWatcher, SLOT(file_changed(QString)));
    QObject::connect(&this->watcher, SIGNAL(directoryChanged(QString)), backupWatcher, SLOT(directory_changed(QString)));

    this->load();
    this->addBackupsToWatcher();
}


void BackupSystem::load() {
    cout << "Loading settings" << endl;

    QString filePath(QCoreApplication::applicationDirPath() + "/settings.js");

    QFile settingsFile(filePath);
    if (!settingsFile.open(QIODevice::ReadOnly))
        cout << "Error with opening file" << endl;

    QTextStream in(&settingsFile);
    QString settingsFileContents;
    while (!in.atEnd()) {
        QString line = in.readLine();
        settingsFileContents += line;
    }
    settingsFile.close();

    bool jsonParseStatus;
    QVariantMap settingsJson = QtJson::parse(settingsFileContents, jsonParseStatus).toMap();
    if (!jsonParseStatus)
        cout << "Failed to read json" << endl;

    int count = 0;
    foreach (QVariant backup, settingsJson["backups"].toMap()) {
        QString name = settingsJson["backups"].toMap().keys().at(count);
        qDebug() << "\t" << name << endl;
        QString mainPath = backup.toMap()["mainPath"].toString();
        qDebug() << "\t\t" << mainPath << endl;
        QString backupPath = backup.toMap()["backupPath"].toString();
        qDebug() << "\t\t" << backupPath << endl;

        Backup backupObj(name, mainPath, backupPath);
        backups.push_back(backupObj);
        count++;
    }
}

void BackupSystem::addBackupsToWatcher() {
    foreach (Backup backup, this->backups) {
        // Add root directory to watcher
        this->watcher.addPath(backup.getMainPath());

        // Add its subdirectories
        QDirIterator dirIterator(backup.getMainPath(), QDirIterator::Subdirectories);
        while (dirIterator.hasNext()) {
            QString dirPath = dirIterator.next();
            QDir dir(dirPath);

            if (dir.dirName() != "." && dir.dirName() != "..") {
                cout << "Adding to watcher: " << dirPath.toStdString() << endl;
                this->watcher.addPath(dirPath);
            }
        }
    }
}

void BackupSystem::addBackup(Backup backup) {
    this->watcher.addPath(backup.getMainPath());
    this->backups.push_back(backup);

    // Save settings json file
    QVariantMap backupsMap;

    foreach (Backup backup, this->backups) {
        QVariantMap backupMap;
        backupMap["mainPath"] = backup.getMainPath();
        backupMap["backupPath"] = backup.getBackupPath();
        backupsMap[backup.getName()] = backupMap;
    }

    QVariantMap mainMap;
    mainMap["backups"] = backupsMap;

    QByteArray data = QtJson::serialize(mainMap);
    QFile file(QCoreApplication::applicationDirPath() + "/settings.js");
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();
}
