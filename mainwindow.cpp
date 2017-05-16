#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionUndo->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    fileName = QFileDialog::getOpenFileName(this, "Open Image", nullptr,
                                            "Image files (*.jpg *.jpeg *.png)");
    if (!image.load(fileName)) {
        ui->statusBar->showMessage("Invalid image");
        return;
    }
    if (image.format() != QImage::Format_ARGB32) {
        image = image.convertToFormat(QImage::Format_ARGB32);
    }
    imageHistory.clear();
    ui->actionUndo->setEnabled(false);
    ui->statusBar->showMessage("Image loaded");
    showImage(image);
}

void MainWindow::updateProgress(int progress) {
    ui->statusBar->showMessage(QString("Progress: %1%").arg(progress));
}

void MainWindow::showImage(QImage image) {
    this->image = image;
    delete scene;
    scene = new QGraphicsScene(ui->graphicsView);
    scene->addPixmap(QPixmap::fromImage(image));
    ui->graphicsView->setScene(scene);
}

void MainWindow::handleFinished() {
    showImage(image);
    ui->statusBar->showMessage("Image sorting completed");
    ui->actionUndo->setEnabled(true);
    ui->menuSort->setEnabled(true);
    delete pixelSort;
}

void MainWindow::sortPixels(sortpixels::Mode sortDirection)
{
    if (!image.isNull()) {
        ui->actionUndo->setEnabled(false);
        ui->menuSort->setEnabled(false);
        imageHistory.push(image);
        pixelSort = new sortpixels(sortDirection, &image, ui->thresholdSpinBox->value(), ui->brightnessSpinBox->value());
        connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinished()), Qt::UniqueConnection);
        connect(pixelSort, SIGNAL(progressChanged(int)), this, SLOT(updateProgress(int)), Qt::UniqueConnection);
        future = QtConcurrent::run(pixelSort, &sortpixels::sortPixels);
        watcher.setFuture(future);
    }
    else ui->statusBar->showMessage("No image loaded");
}

void MainWindow::on_actionHorizontal_triggered()
{
    sortPixels(sortpixels::Horizontal);
}

void MainWindow::on_actionVertical_triggered()
{
    sortPixels(sortpixels::Vertical);
}

void MainWindow::on_actionASDF_triggered()
{
    sortPixels(sortpixels::ASDF);
}

void MainWindow::on_actionUndo_triggered()
{
    if (!imageHistory.isEmpty()) {
        showImage(imageHistory.pop());
        if (imageHistory.isEmpty()) {
            ui->actionUndo->setEnabled(false);
        }
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (!image.isNull()) {
        QFileInfo fileInfo(fileName);
        QString fileName = QFileDialog::getSaveFileName(this, "Save Image",
                                                        QString("%1/%2_sorted%3.png").arg(fileInfo.filePath())
                                                        .arg(fileInfo.baseName())
                                                        .arg(imageHistory.length()),
                                                        "PNG Image (*.png);;JPG Image (*.jpg)");
        if (image.save(fileName)) {
            ui->statusBar->showMessage("Image saved");
        }
        else ui->statusBar->showMessage("Unsuccessful save");
    }
    else ui->statusBar->showMessage("No image to save");
}
