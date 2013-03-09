#ifndef BACKUP_H
#define BACKUP_H

#include <QString>

class Backup {
public:
    Backup(QString, QString, QString);
    QString getName();
    QString getMainPath();
    QString getBackupPath();
private:
    QString name;
    QString mainPath;
    QString backupPath;
};

#endif // BACKUP_H
