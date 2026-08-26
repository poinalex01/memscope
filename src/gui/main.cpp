#include "anticheat_guard.h"
#include "process_finder.h"

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("MemScope");
    window.resize(800, 600);

    QWidget *centralWidget = new QWidget(&window);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QPushButton *attachButton = new QPushButton("Attach to Notepad", centralWidget);
    QTextEdit *outputBox = new QTextEdit(centralWidget);
    outputBox->setReadOnly(true);

    layout->addWidget(attachButton);
    layout->addWidget(outputBox);

    window.setCentralWidget(centralWidget);

    QObject::connect(attachButton, &QPushButton::clicked, [outputBox]()
                     {
    if (memscope::IsAntiCheatProcessRunning()) {
        outputBox->append("Anti-cheat process detected. Refusing to run.");
        return;
    }

    auto pid = memscope::FindProcessIdByName(L"Notepad.exe");

    if (!pid.has_value()) {
        outputBox->append("Notepad not found.");
        return;
    }

    outputBox->append(QString("Found Notepad, PID: %1").arg(pid.value())); });

    window.show();

    return app.exec();
}