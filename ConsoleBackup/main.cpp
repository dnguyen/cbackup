#include <QCoreApplication>
#include <QTextStream>
#include <QString>
#include <iostream>
#include <QStringList>
#include <QDir>

#include "BackupSystem.h"

using namespace QtJson;
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    cout << "Command list" << endl;
    cout << "\tCreates a backup for a file or directory (and subdirectories)." << "\n\t\tcreate [-f, -d] [file path]" << endl;
    cout << "\tExit out of command mode" << "\n\t\tsleep" << endl;
    cout << "\tquit" << endl;
    cout << endl;

    BackupSystem backupSystem;

    // start waiting for commands
    bool status = true;
    while (status) {
        cout << "Enter a command: ";
        QTextStream qtin(stdin);
        QString line = qtin.readLine();

        QStringList lineSplit = line.split(' ');
        if (lineSplit.at(0) == "create") {
            cout << "Enter name for backup:" ;
            QString name;
            qtin >> name;

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
                    // Create backup and add it to the list of backups.
                    Backup backup(name, path, backupPath);
                    backupSystem.addBackup(backup);
                    cout << "Successfully created a backup" << endl;
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
