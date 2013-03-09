#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <iostream>
#include <vector>
#include <QStringList>
#include <QFileSystemWatcher>
#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include <QMap>
#include <QVariant>
#include <QVariantMap>

#include "BackupPathsWatcher.h"
#include "Backup.h"
#include "json.h"

using namespace QtJson;
using namespace std;

vector<Backup> backups;

// Load settings from settings.js file.
// Includes file and directory paths.
void loadSettings() {
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

void addBackupPathsToWatcher(QFileSystemWatcher &watcher) {

    foreach (Backup backup, backups) {
        // Add root directory to watcher
        watcher.addPath(backup.getMainPath());

        // Add its subdirectories
        QDirIterator dirIterator(backup.getMainPath(), QDirIterator::Subdirectories);
        while (dirIterator.hasNext()) {
            QString dirPath = dirIterator.next();
            QDir dir(dirPath);

            if (dir.dirName() != "." && dir.dirName() != "..") {
                cout << "Adding to watcher: " << dirPath.toStdString() << endl;
                watcher.addPath(dirPath);
            }
        }
    }
}

void addPathsToWatcher(QFileSystemWatcher &watcher) {
    addBackupPathsToWatcher(watcher);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    cout << "Command list" << endl;
    cout << "\tCreates a backup for a file or directory (and subdirectories)." << "\n\t\tcreate [-f, -d] [file path]" << endl;
    cout << "\tExit out of command mode" << "\n\t\tsleep" << endl;
    cout << "\tquit" << endl;
    cout << endl;

    loadSettings();

    QFileSystemWatcher watcher;
    addPathsToWatcher(watcher);

    BackupPathsWatcher* backupWatcher = new BackupPathsWatcher;

    QObject::connect(&watcher, SIGNAL(fileChanged(QString)), backupWatcher, SLOT(file_changed(QString)));
    QObject::connect(&watcher, SIGNAL(directoryChanged(QString)), backupWatcher, SLOT(directory_changed(QString)));

    // start waiting for commands
    bool status = false;
    while (status) {
        cout << "Enter a command: ";
        QTextStream qtin(stdin);
        QString line = qtin.readLine();

        QStringList lineSplit = line.split(' ');
        if (lineSplit.at(0) == "create") {
            cout << "Enter path of file or directory: ";

            QString path;
            qtin >> path;
            QDir enteredPath(path);

            if (enteredPath.exists()) {
                cout << "Enter backup path: ";

                QString backupPath;
                qtin >> backupPath;
                QDir qDirBackupPath(backupPath);

                if (qDirBackupPath.exists()) {
                    // Create backup
                    //Backup backup(path, backupPath);

                } else {
                    cout << "Path does not exist" << endl;
                }
            } else {
                cout << "Path does not exist" << endl;
            }

        } else if (lineSplit.at(0) == "sleep") {
            status = false;
        } else {
            cout << "Invalid command" << endl;
        }
    }

    return a.exec();
}
