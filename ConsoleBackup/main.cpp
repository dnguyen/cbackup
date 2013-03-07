#include <QCoreApplication>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    cout << "Command list" << endl;
    cout << "create [file path]" << endl;
    cout << "quit" << endl;
    cout << endl;

    bool status = true;
    while (status) {
        string enteredCommand;
        getline(cin, enteredCommand);
        cout << enteredCommand << endl;
    }

    return a.exec();
}
