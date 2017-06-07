#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QFileDialog>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>
#include "sortpixels.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void handleFinished();

    void updateProgress(int);

private slots:
    void on_actionOpen_triggered();

    void on_actionHorizontal_triggered();

    void on_actionVertical_triggered();

    void on_actionASDF_triggered();

    void on_actionUndo_triggered();

    void on_actionSave_triggered();

    void on_action_About_Pixsrt_triggered();

    void on_actionAbout_Qt_triggered();

    void on_actionExit_triggered();

    void on_actionAbout_Licenses_triggered();

private:
    Ui::MainWindow *ui;

    void showImage(QImage);

    void sortPixels(sortpixels::Mode);

    QImage image;
    QString fileName;
    QGraphicsScene *scene;
    QStack<QImage> imageHistory;
    sortpixels *pixelSort;
    QFuture<void> future;
    QFutureWatcher<void> watcher;
};

#endif // MAINWINDOW_H
