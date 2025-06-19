#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTextEdit;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void saveFile();
    void exitApp();

private:
    void createActions();
    void createMenus();
    QTextEdit *textEdit;
};

#endif 
