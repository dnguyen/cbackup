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

    foreach (QVariant backup, settingsJson["backups"].toList()) {
        qDebug() << backup.toMap()["name"].toString() << endl;
        QMap<QString, QVariant> backupMap = backup.toMap();
        Backup backupObj(backupMap["name"].toString(), backupMap["mainPath"].toString(), backupMap["backupPath"].toString());
        backups.push_back(backupObj);
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
}
