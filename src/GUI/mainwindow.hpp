#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include "parsing.hpp"
#include "../shared/misc/structs.hpp"
#include "../shared/filtering/lambda-creator.hpp"
#include "../shared/misc/utils.hpp"
#include "../shared/misc/map-parser.hpp"
#include "../shared/analyse-program.hpp"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_GoButton_clicked();

private:
    Ui::MainWindow *ui;
    std::vector<RemoteHeapPointer> ExecuteQuery(const Query q);
    void InsertPtrToTextbox(const RemoteHeapPointer& ptr);
};

#endif // MAINWINDOW_H
