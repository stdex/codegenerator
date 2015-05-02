#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "typefunction.h"
#include <QProcess>
#include <QSettings>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QString filename;
    QList<TypeFunction> listfun[100];
    int currentRow;
    int argumentCount;
    QList<QStringList> list_args;

    QProcess *process;
    QString strOutput;
    QString qmake_path;
    QString minigw_path;
    //args
    QString saveStr;
    QString fullsource_h;
    QString fullsource_cpp;
    QString source_cpp;
    QString source_h;
    QString className;

public:
    QString settings_file;
    QString saveFunData;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    TypeFunction FindFunction(QString functionName);
    void TestGenerator();
    //temporary save seans table data
    void tmpSave();

private:
    Ui::MainWindow *ui;

private slots:
    void on_btnGetMinGW_clicked();
    void on_btnGetQmake_clicked();
    void on_btnClear_clicked();
    void on_btnDel_clicked();
    void on_btnAdd_clicked();
    void on_listFunctions_itemSelectionChanged();
    void on_btnSave_clicked();
    void on_btnRun_clicked();
    void on_btnOpenFile_clicked();

    void logReady();

};

#endif // MAINWINDOW_H
