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

void BackupSystem::save() {
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

/*
 * Copy files/directories to backup path.
 *      Replaces the file/directory's main path with the backup path.
 */
void BackupSystem::createBackup(Backup backup) {
    QDirIterator dirIterator(backup.getMainPath(), QDirIterator::Subdirectories);
    while (dirIterator.hasNext()) {
        QString dirPath = dirIterator.next();
        QDir dir(dirPath);

        if (dir.dirName() != "." && dir.dirName() != "..") {
            QString oldBackupPath = dirPath;
            QString newBackupPath = dirPath.replace(backup.getMainPath(), backup.getBackupPath());
            cout << "Creating backups for: " << endl;
            cout << "\t" << oldBackupPath.toStdString() << " backup at : " << newBackupPath.toStdString() << endl;

            // Check if the backup is a file or directory.
            struct stat s;
            if (stat(oldBackupPath.toStdString().c_str(), &s) == 0) {

                // If is directory
                if (s.st_mode & S_IFDIR) {
                    cout << oldBackupPath.toStdString() << " Is directory" << endl;
                    QDir newBackupDir;
                    bool ok = newBackupDir.mkpath(newBackupPath);
                    if (!ok) {
                        cout << "Failed to create directory" << endl;
                        return;
                    }
                // If is file
                } else if (s.st_mode & S_IFREG) {
                    cout << oldBackupPath.toStdString() << " is file" << endl;
                    if (!QFile::copy(oldBackupPath, newBackupPath)) {
                        cout << "Failed to copy file" << endl;
                        return;
                    }

                }
            } else {
                cout << "error stat" << endl;
                return;
            }
        }
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

    this->save();
}
