#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QKeyEvent>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <QMap>
#include <QPluginLoader>

#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    fileMenu(nullptr),
    viewMenu(nullptr),
    currentImage(nullptr)
{
    initUI();
    loadPlugins();
}

void MainWindow::initUI(){
    this->resize(800,600);
// Изменение размера окна
    fileMenu = menuBar()->addMenu("&File");
    viewMenu = menuBar()->addMenu("&View");
    editMenu = menuBar()->addMenu("&Edit");
// Добавление меню сверху
    editToolBar = addToolBar("Edit");
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

    blureAction = new QAction("Blure image", this);
    editMenu->addAction(blureAction);
    editToolBar->addAction(blureAction);
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
    connect(blureAction, SIGNAL(triggered(bool)), this, SLOT(blureImage()));

    setupShortcuts();
}


void MainWindow::blureImage(){
    qDebug() << "Blure image action!";
    if (currentImage == nullptr){
        QMessageBox::information(this, "Information", "Image is not opened");
        return;
    }
    QPixmap pixmap = currentImage->pixmap();
    QImage image = pixmap.toImage();
    image = image.convertToFormat(QImage::Format_RGB888);
    cv::Mat img = cv::Mat(image.height(), image.width(), CV_8UC3, image.bits(), image.bytesPerLine());
    cv::Mat tmp;
    cv::blur(img, tmp, cv::Size(8,8));
    // tmp - blured image
    QImage blur_img(tmp.data, tmp.cols, tmp.rows, tmp.step,QImage::Format_RGB888);
    QPixmap pixmap_b = QPixmap::fromImage(blur_img);
    imageScene->clear();
    imageView->resetMatrix();
    currentImage = imageScene->addPixmap(pixmap_b);
    imageScene->update();
    imageView->setSceneRect(pixmap_b.rect());
    QString status = QString("%1, %2x%3, %4 Bytes").arg(currentImagePath).arg(blur_img.width()).arg(blur_img.height()).arg(QFile(currentImagePath).size());
    mainStatusLabel->setText(status);
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

void MainWindow::loadPlugins()
{
    QDir pluginsDir("/home/freeexotic/Qt_project/ImageEditor/ImageViewer/plugins/");
    qDebug() << QApplication::instance()->applicationDirPath();
    QStringList nameFilters;
    nameFilters << "*.so" << "*.dylib" << "*.dll";
    QFileInfoList plugins = pluginsDir.entryInfoList(nameFilters, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
    qDebug() << "111";
    foreach(QFileInfo plugin, plugins) {
        qDebug() << "222";
        QPluginLoader pluginLoader(plugin.absoluteFilePath(), this);
        EditorPluginInterface *plugin_ptr = dynamic_cast<EditorPluginInterface*>(pluginLoader.instance());
        if(plugin_ptr) {
            QAction *action = new QAction(plugin_ptr->name());
            editMenu->addAction(action);
            editToolBar->addAction(action);
            editPlugins[plugin_ptr->name()] = plugin_ptr;
            connect(action, SIGNAL(triggered(bool)), this, SLOT(pluginPerform()));
            // pluginLoader.unload();
        } else {
            qDebug() << "bad plugin: " << plugin.absoluteFilePath();
        }
    }
}


void MainWindow::pluginPerform()
{
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "No image to edit.");
        return;
    }

    QAction *active_action = qobject_cast<QAction*>(sender());
    EditorPluginInterface *plugin_ptr = editPlugins[active_action->text()];
    if(!plugin_ptr) {
        QMessageBox::information(this, "Information", "No plugin is found.");
        return;
    }
    qDebug() << "333";
    QPixmap pixmap = currentImage->pixmap();
    QImage image = pixmap.toImage();
    image = image.convertToFormat(QImage::Format_RGB888);
    cv::Mat mat = cv::Mat(
        image.height(),
        image.width(),
        CV_8UC3,
        image.bits(),
        image.bytesPerLine());

    plugin_ptr->edit(mat, mat);

    QImage image_edited(
        mat.data,
        mat.cols,
        mat.rows,
        mat.step,
        QImage::Format_RGB888);
    pixmap = QPixmap::fromImage(image_edited);
    imageScene->clear();
    imageView->resetMatrix();
    currentImage = imageScene->addPixmap(pixmap);
    imageScene->update();
    imageView->setSceneRect(pixmap.rect());
    QString status = QString("(editted image), %1x%2").arg(pixmap.width()).arg(pixmap.height());
    mainStatusLabel->setText(status);
}
