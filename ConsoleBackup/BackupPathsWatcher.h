#ifndef BACKUPPATHSWATCHER_H
#define BACKUPPATHSWATCHER_H
#include <QObject>
#include <iostream>
using namespace std;
class BackupPathsWatcher : public QObject {
    Q_OBJECT
public:
    BackupPathsWatcher() { }
public slots:
    void directory_changed(const QString &str) {
        cout << "Directory changed: " << str.toStdString() << endl;
    }

    void file_changed(const QString &str) {
        cout << "File changed : " << str.toStdString() << endl;
    }
};

#endif // BACKUPPATHSWATCHER_H
