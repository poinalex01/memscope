#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "anticheat_guard.h"
#include "memory_scanner.h"
#include "process_finder.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("MemScope");
    window.resize(800, 600);

    QWidget *centralWidget = new QWidget(&window);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QPushButton *attachButton = new QPushButton("Attach to Notepad", centralWidget);
    QPushButton *scanButton = new QPushButton("Scan Own Process for 123456789", centralWidget);
    QPushButton *rescanButton = new QPushButton("Change Value and Rescan", centralWidget);
    QPushButton *freezeButton = new QPushButton("Freeze to 100", centralWidget);
    QTextEdit *outputBox = new QTextEdit(centralWidget);
    outputBox->setReadOnly(true);

    layout->addWidget(attachButton);
    layout->addWidget(scanButton);
    layout->addWidget(rescanButton);
    layout->addWidget(freezeButton);
    layout->addWidget(outputBox);

    window.setCentralWidget(centralWidget);

    HANDLE processHandle = NULL;
    std::vector<uintptr_t> matches;
    int32_t currentValue = 123456789;
    memscope::FreezeWorker freezeWorker;
    bool isFreezing = false;

    QObject::connect(attachButton, &QPushButton::clicked, [outputBox]()
                     {
        if (memscope::IsAntiCheatProcessRunning())
        {
            outputBox->append("Anti-cheat process detected. Refusing to run.");
            return;
        }

        auto pid = memscope::FindProcessIdByName(L"Notepad.exe");

        if (!pid.has_value())
        {
            outputBox->append("Notepad not found.");
            return;
        }

        outputBox->append(QString("Found Notepad, PID: %1").arg(pid.value())); });

    QObject::connect(scanButton, &QPushButton::clicked, [&, outputBox]()
                     {
        if (processHandle == NULL)
        {
            processHandle = memscope::OpenProcessByPid(GetCurrentProcessId());
        }

        auto regions = memscope::GetReadableWritableRegions(processHandle);

        matches.clear();
        for (const auto& region : regions)
        {
            auto found = memscope::ScanRegionForValue(processHandle, region, currentValue);
            matches.insert(matches.end(), found.begin(), found.end());
        }

        outputBox->append(QString("Scan found %1 matches.").arg(matches.size())); });

    QObject::connect(rescanButton, &QPushButton::clicked, [&, outputBox]()
                     {
        currentValue = 999999999;
        matches = memscope::RescanAddresses(processHandle, matches, currentValue);

        outputBox->append(QString("Rescan narrowed to %1 matches.").arg(matches.size())); });

    QObject::connect(freezeButton, &QPushButton::clicked, [&, outputBox, freezeButton]()
                     {
        if (matches.empty())
        {
            outputBox->append("No address to freeze. Scan and rescan first.");
            return;
        }

        if (!isFreezing)
        {
            freezeWorker.Start(processHandle, matches[0], 100);
            isFreezing = true;
            freezeButton->setText("Stop Freezing");
            outputBox->append("Freezing started.");
        }
        else
        {
            freezeWorker.Stop();
            isFreezing = false;
            freezeButton->setText("Freeze to 100");
            outputBox->append("Freezing stopped.");
        } });

    window.show();

    return app.exec();
}