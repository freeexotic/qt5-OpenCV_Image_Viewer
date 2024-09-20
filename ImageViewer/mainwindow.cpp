#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QKeyEvent>
#include <QDebug>

#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    fileMenu(nullptr),
    viewMenu(nullptr),
    currentImage(nullptr)
{
    initUI();
}
void MainWindow::initUI(){
    this->resize(800,600);
// Изменение размера окна
    fileMenu = menuBar()->addMenu("&File");
    viewMenu = menuBar()->addMenu("&View");
// Добавление меню сверху
    fileToolBar = addToolBar("File");
    viewToolBar = addToolBar("View");

    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    setCentralWidget(imageView);

    mainStatusBar = statusBar();
    mainStatusLabel = new QLabel(mainStatusBar);
    mainStatusBar->addPermanentWidget(mainStatusLabel);
    mainStatusLabel->setText("information in Status Label!");

    createActions();
}

void MainWindow::createActions(){
    // Добавление функционала в menuBar -> Open
    openAction = new QAction("&Open", this);
    fileMenu->addAction(openAction);

    saveasAction = new QAction("&Save as", this);
    fileMenu->addAction(saveasAction);

    exitAction = new QAction("E&xit", this);
    fileMenu->addAction(exitAction);
    // Добавление функционала в menuBar -> View
    zoominAction = new QAction("&Zoom In", this);
    viewMenu->addAction(zoominAction);

    zoomoutAction = new QAction("&Zoom out", this);
    viewMenu->addAction(zoomoutAction);

    priviousimageAction= new QAction("&Privious Image", this);
    viewMenu->addAction(priviousimageAction);

    nextimageAction = new QAction("&Next image", this);
    viewMenu->addAction(nextimageAction);
    // Добавление Функций в ToolBar

    fileToolBar->addAction(openAction);
    fileToolBar->addAction(zoominAction);
    fileToolBar->addAction(zoomoutAction);
    fileToolBar->addAction(priviousimageAction);
    fileToolBar->addAction(nextimageAction);

    connect(exitAction, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));
    connect(zoominAction, SIGNAL(triggered(bool)), this, SLOT(zoomIn()));
    connect(zoomoutAction, SIGNAL(triggered(bool)), this, SLOT(zoomOut()));
    connect(saveasAction, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
    connect(priviousimageAction, SIGNAL(triggered(bool)), this, SLOT(prevImage()));
    connect(nextimageAction, SIGNAL(triggered(bool)), this, SLOT(nextImage()));

    setupShortcuts();
}


void MainWindow::nextImage(){
    QFileInfo current(currentImagePath);
    QDir dir = current.absoluteDir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg";
    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    int idx = fileNames.indexOf(QRegExp(QRegExp::escape(current.fileName())));
     /*  .fileName - формирует список имен всех файлов в папке используя информацию из файла
       QRegExp::scape - разбивает список на отдельные имена
       QRegExp - ищет совпадение с названием имени файла
       .indexOf - возвращает индекс имени файла  */
    if (idx < fileNames.size()-1) {
        showImage(dir.absoluteFilePath(fileNames.at(idx+1)));
    }
    else{
        QMessageBox::information(this, "Information", "Current image is the last one.");
    }
}

void MainWindow::prevImage(){
    QFileInfo current(currentImagePath);
    QDir dir = current.absoluteDir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg";
    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    int idx = fileNames.indexOf(QRegExp(QRegExp::escape(current.fileName())));
    if (idx > 0) {
        showImage(dir.absoluteFilePath(fileNames.at(idx-1)));
    }
    else{
        QMessageBox::information(this, "Information", "Current image is the first one.");
    }
}




void MainWindow::openImage(){
    QFileDialog dialog(this);
    dialog.setWindowTitle("Open Image");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    QStringList filePaths;
    if (dialog.exec()){
        filePaths = dialog.selectedFiles();
        showImage(filePaths.at(0));
    }

    qDebug() << "Slot openImage work!";
}

void MainWindow::showImage(QString path){
    currentImagePath = path;
    imageScene->clear();
    imageView->resetMatrix();
    QPixmap image(path);
    currentImage = imageScene->addPixmap(image);
    imageScene->update();
    imageView->setSceneRect(image.rect());
    QString status = QString("%1, %2x%3, %4 Bytes").arg(path).arg(image.width()).arg(image.height()).arg(QFile(path).size());
    mainStatusLabel->setText(status);
}

void MainWindow::saveAs(){
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "Nothing to save.");
        return;
    }
    QFileDialog dialog(this);
    dialog.setWindowTitle("Save Image As ...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        if(QRegExp(".+\\.(png|bmp|jpg)").exactMatch(fileNames.at(0))) {
            currentImage->pixmap().save(fileNames.at(0));
        } else {
            QMessageBox::information(this, "Information", "Save error: bad format or filename.");
        }
    }
}

void MainWindow::setupShortcuts(){
    QList<QKeySequence> shortcuts;
    shortcuts << Qt::Key_Plus << Qt::Key_Equal;
    zoominAction->setShortcuts(shortcuts);
    shortcuts.clear();

    shortcuts << Qt::Key_Minus << Qt::Key_Underscore;
    zoomoutAction->setShortcuts(shortcuts);
    shortcuts.clear();

    shortcuts << Qt::Key_Up << Qt::Key_Left;
    priviousimageAction->setShortcuts(shortcuts);
    shortcuts.clear();

    shortcuts << Qt::Key_Down << Qt::Key_Right;
    nextimageAction->setShortcuts(shortcuts);
    shortcuts.clear();
}

void MainWindow::zoomIn(){
    imageView->scale(1.2,1.2);
}

void MainWindow::zoomOut(){
    imageView->scale(0.8,0.8);
}


MainWindow::~MainWindow(){
}
