#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <iostream>
#include <vector>
#include <QSettings>
#include <QStringList>
using namespace std;

vector<QString> fileBackups;
vector <QString> directoryBackups;

void loadSettings() {
    QString filePath("/settings.ini");

    QSettings settings(QCoreApplication::applicationDirPath() + filePath, QSettings::IniFormat);


    cout << "Load file paths: " << endl;
    settings.beginGroup("FileBackups");
    const QStringList fileBackupPathKeys = settings.childKeys();

    cout << "File Paths Length: " << fileBackupPathKeys.length() << endl;

    foreach (const QString &filePathKey, fileBackupPathKeys) {
        cout << "\t" << QString(settings.value(filePathKey).toString()).toStdString() << endl;
        fileBackups.push_back(QString(settings.value(filePathKey).toString()));
    }
    settings.endGroup();


    cout << "Load directory paths " << endl;
    settings.beginGroup("DirectoryBackups");
    const QStringList directoryBackupPathKeys = settings.childKeys();

    cout << "Directory Paths length: " << directoryBackupPathKeys.length() << endl;

    foreach (const QString &directoryPathKey, directoryBackupPathKeys) {
        cout << "\t" << QString(settings.value(directoryPathKey).toString()).toStdString() << endl;
        directoryBackups.push_back(QString(settings.value(directoryPathKey).toString()));
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    cout << "Command list" << endl;
    cout << "\tcreate [-f, -d] [file path]" << endl;
    cout << "\tquit" << endl;
    cout << endl;

    loadSettings();

    bool status = true;
    while (status) {
        string enteredText;
        string command = "";

        cin >> command; // use first word as command.

        if (command == "create") {
            cout << "Create a backup" << endl;
            // Get option to backup a file or a directory.
            string option = "";
            cin >> option;
            if (option != "") {
                // Get file/directory path
                string path = "";
                cin >> path;

                cout << "Create backup of " << option << " at " << path << endl;
            } else {
                cout << "No option";
            }
        } else if (command == "quit") {
            exit(EXIT_SUCCESS);
        }
        cin.clear();

        //getline(cin, enteredText);
    }

    return a.exec();
}
