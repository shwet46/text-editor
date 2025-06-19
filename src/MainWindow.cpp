#include "MainWindow.h"
#include <QTextEdit>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), textEdit(new QTextEdit(this)) {
    setCentralWidget(textEdit);
    createActions();
    createMenus();
    setWindowTitle("Text Editor");
}

MainWindow::~MainWindow() {}

void MainWindow::createActions() {
    // ...existing code...
}

void MainWindow::createMenus() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *openAct = new QAction(tr("&Open..."), this);
    QAction *saveAct = new QAction(tr("&Save"), this);
    QAction *exitAct = new QAction(tr("E&xit"), this);
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);
    connect(exitAct, &QAction::triggered, this, &MainWindow::exitApp);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
}

void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            textEdit->setPlainText(in.readAll());
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Cannot open file"));
        }
    }
}

void MainWindow::saveFile() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << textEdit->toPlainText();
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Cannot save file"));
        }
    }
}

void MainWindow::exitApp() {
    close();
}
