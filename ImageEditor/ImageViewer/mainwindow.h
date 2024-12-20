#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QStatusBar>
#include <QLabel>
#include <QGraphicsPixmapItem>
#include <QMap>

#include "editor_plugin_interface.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT;
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void initUI();
    void createActions();
    void showImage(QString);
    void setupShortcuts();

    void loadPlugins();

private slots:
    void openImage();
    void zoomIn();
    void zoomOut();
    void prevImage();
    void nextImage();
    void saveAs();
    void blureImage();
    void pluginPerform();

private:
    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *editMenu;


    QToolBar *fileToolBar;
    QToolBar *viewToolBar;
    QToolBar *editToolBar;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;
    QString currentImagePath;
    QGraphicsPixmapItem *currentImage;

    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;

// Функционал menuBar -> File
    QAction *openAction;
    QAction *saveasAction;
    QAction *exitAction;
    QAction *blureAction;

// Функционал menuBar -> View
    QAction *zoominAction;
    QAction *zoomoutAction;
    QAction *priviousimageAction;
    QAction *nextimageAction;

    QMap<QString, EditorPluginInterface*> editPlugins;

};

#endif // MAINWINDOW_H
