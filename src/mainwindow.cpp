#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    QIcon::setThemeName("oxygen");
#endif
    ui->setupUi(this);
    scene = nullptr;
    ui->actionUndo->setEnabled(false);
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", nullptr,
                                            "Image files (*.jpg *.jpeg *.png)");
    loadImage(fileName);
}


void MainWindow::loadImage(QString fileName)
{
    this->fileName = fileName;
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

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    loadImage(event->mimeData()->urls()[0].toLocalFile());
    event-> acceptProposedAction();
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

void MainWindow::on_action_About_Pixsrt_triggered()
{
    QString compiler = "Unknown compiler";
#ifdef Q_PROCESSOR_X86_64
#ifdef _MSC_VER
    compiler = QString("MSVC %1, 64 bit").arg(_MSC_VER);
#elif __MINGW32__
    compiler = QString("MinGW %1.%2, 64 bit").arg(__MINGW32_MAJOR_VERSION).arg(__MINGW32_MINOR_VERSION)
#elif __clang__
    compiler = QString("Clang %1.%2.%3, 64 bit").arg(__clang_major__).arg(__clang_minor__).arg(__clang_patchlevel__);
#elif __GNUC__
    compiler = QString("GCC %1.%2.%3, 64 bit").arg(__GNUC__).arg(__GNUC_MINOR__).arg(__GNUC_PATCHLEVEL__);
#endif
#elif Q_PROCESSOR_X86
#ifdef _MSC_VER
    compiler = QString("MSVC %1, 32 bit").arg(_MSC_VER);
#elif __MINGW32__
    compiler = QString("MinGW %1.%2, 32 bit").arg(__MINGW32_MAJOR_VERSION).arg(__MINGW32_MINOR_VERSION);
#elif __clang__
    compiler = QString("Clang %1.%2.%3, 32 bit").arg(__clang_major__).arg(__clang_minor__).arg(__clang_patchlevel__);
#elif __GNUC__
    compiler = QString("GCC %1.%2.%3, 32 bit").arg(__GNUC__).arg(__GNUC_MINOR__).arg(__GNUC_PATCHLEVEL__);
#endif
#endif
    QMessageBox::about(this, "About Pixsrt", QString("Pixsrt (%1)\
                                                            \nLawrence Thorpe\
                                                            \n\nBuilt with Qt %2 (%3)").arg(TAG).arg(QT_VERSION_STR).arg(compiler));
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_Licenses_triggered()
{
    QMessageBox licenseMessage(QMessageBox::NoIcon, "About Licenses", "Oxygen Icons:"
                             "\n    http://www.kde.org/ "
                             "\n    LGPLv3");
    licenseMessage.exec();
}
