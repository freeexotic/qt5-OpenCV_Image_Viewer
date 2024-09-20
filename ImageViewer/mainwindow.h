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

private slots:
    void openImage();
    void zoomIn();
    void zoomOut();
    void prevImage();
    void nextImage();
    void saveAs();

private:
    QMenu *fileMenu;
    QMenu *viewMenu;

    QToolBar *fileToolBar;
    QToolBar *viewToolBar;

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

// Функционал menuBar -> View
    QAction *zoominAction;
    QAction *zoomoutAction;
    QAction *priviousimageAction;
    QAction *nextimageAction;


};

#endif // MAINWINDOW_H
