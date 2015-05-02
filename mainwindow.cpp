#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <qdesktopwidget.h>
#include <QTextStream>
#include <QMessageBox>
#include <QStringList>
#include <QTableWidget>
#include <QDir>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    settings_file = qApp->applicationDirPath() + "/codegenerator.ini";
    QSettings settings(settings_file,QSettings::IniFormat);
    ui->setupUi(this);
    //Центрирование экрана
    QRect scr = QApplication::desktop()->screenGeometry();
    move( scr.center() - rect().center() );
    ui->prgAnalysis->setMaximum(100);
    ui->prgAnalysis->setValue(0);
    //Current table row
    currentRow = 0;
    argumentCount = 0;
    //Output string
    strOutput = "";
    //Path to qmake.exe
    qmake_path = settings.value("qmake_path", "").toString();//c:/qt/2010.05-rc1/qt/bin/qmake.exe
    ui->txtQmake->setText(qmake_path);
    //Path to mingw32-make.exe
    minigw_path = settings.value("minigw_path","").toString();//C:/Qt/2010.05-rc1/mingw/bin/mingw32-make.exe
    ui->txtMinGW->setText(minigw_path);
    saveFunData = "";
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnOpenFile_clicked()
{
    filename = QFileDialog::getOpenFileName();
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) return;
    if(filename.contains(".cpp")==false)
    {
        QMessageBox msgBox;
        msgBox.setText("Not .cpp file!");
        msgBox.exec();
        return;
    }
    QString file_h = filename.replace("cpp","h");
    QFile file2(file_h);
    if (!file2.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox msgBox2;
        msgBox2.setText("Can't find .h file!");
        msgBox2.exec();
        return;
    }
    //
    saveStr = this->filename;
    fullsource_h = saveStr;
    fullsource_cpp = saveStr.replace("h","cpp");
    source_cpp = fullsource_cpp.mid(fullsource_cpp.lastIndexOf("/")+1);
    source_h =  fullsource_h.mid(fullsource_h.lastIndexOf("/")+1);
    className = source_h.mid(0,source_h.lastIndexOf("."));
    //
    ui->listFunctions->clear();
    ui->prgAnalysis->setValue(0);
    QTextStream in(&file2);
    QString myString = in.readAll();
    //*.h file parsing
    //myList = myString.split(QRegExp("(?![a-z])[^\:,>,\.]([a-z,A-Z]+[_]*[a-z,A-Z]*)+[(]"));
    QStringList myList = myString.split("\n");
    for(int i=0; i<myList.size(); i++ )
    {
        ui->prgAnalysis->setValue((i+1)*100/myList.size()+1);
        QString line = myList.at(i);
        line = line.trimmed();
        //empty line
        if(line.length()==0)continue;
        //skip the comment
        if (line.startsWith("//") || line.startsWith("/*") || !line.contains("(")) continue;
        QStringList returnType = line.split(QRegExp("\\s"));//the tokens of the line
        if (returnType.size() == 0 || line.length() == returnType.at(0).size() || returnType.at(0).length() < 2) continue;
        for (int j = 0; j < returnType.size(); j++)
        {
            if (returnType.at(j).at(0) == '='){//binding a value
                        continue;
            }
        }
        //the first string should start with a letter
        QString firstCh = line.mid(0, 1);
        if (!firstCh.contains(QRegExp("[a-zA-Z]"))) {
            continue;
        }
        //todo stub!!! need test!!!
        QStringList test = line.split("(");//after operation 2 strings: 0 - type + name; 1 - args + );
        QString typeName = test.at(0);
        //0
        QStringList tn = typeName.split(QRegExp("\\s"));
        QString type = "";
        QString name = "";
        for(int k = 0; k < tn.size(); k++)
        {
            if(k!=tn.size()-1)
            {
                type += tn.at(k);
            }
            else
            {
                name = tn.at(k);
            }
        }
        TypeFunction *fun = new  TypeFunction();
        fun->SetFunctionName(name);
        fun->SetFunctionReturnType(type);
        //1
        QString args = test.at(1);
        QStringList targs = args.split(",");
        for(int m = 0; m < targs.size(); m++)
        {
            if(m!=targs.size()-1)
            {
                QStringList tn2 = targs.at(m).split(QRegExp("\\s"));
                QString type2 = "";
                QString name2 = "";
                for(int n = 0; n < tn2.size(); n++)
                {
                    if(n!=tn2.size()-1)
                    {
                        type2 += tn2.at(n);
                    }
                    else
                    {
                        name2 = tn2.at(n);
                    }
                }
                fun->AddFunctionArgument(name2);
                fun->AddFunctionArgumentTypes(type2);
            }
            else//delete );
            {
                QString arg3 = targs.at(m).mid(0,targs.at(m).indexOf(")"));
                QStringList tn3 = arg3.split(QRegExp("\\s"));
                QString type3 = "";
                QString name3 = "";
                for(int l = 0; l < tn3.size(); l++)
                {
                    if(l!=tn3.size()-1)
                    {
                        type3 += tn3.at(l);
                    }
                    else
                    {
                        name3 = tn3.at(l);
                    }
                }
                fun->AddFunctionArgument(name3);
                fun->AddFunctionArgumentTypes(type3);
            }
        }
        listfun->append(*fun);
        ui->listFunctions->addItem(fun->GetFunctionName());
    }
    ui->prgAnalysis->setValue(100);

}
//Run code generator
void MainWindow::on_btnRun_clicked()
{
    list_args.clear();
    tmpSave();
    if(ui->tableTestData->rowCount()==0)return;
    for(int i = 0; i < ui->tableTestData->rowCount();i++)
    {
        QStringList vars;
        for(int j = 0; j < ui->tableTestData->columnCount();j++)
        {
            QString text = ui->tableTestData->item(i, j)->text();
            vars<<text;
        }
        list_args<<vars;
    }
    TestGenerator();
    return;
}

void MainWindow::on_btnSave_clicked()
{
    QString flename = QFileDialog::getSaveFileName();
    QFile file(flename);
    if (!file.open(QFile::WriteOnly | QFile::Text)) return;
    QTextStream out(&file);
    out << ui->textConsoleOut->toPlainText();
    file.close();
    qApp->exit();
}

void MainWindow::on_listFunctions_itemSelectionChanged()
{
    argumentCount = 0;
    tmpSave();
    while (ui->tableTestData->rowCount() > 0)
    {
        ui->tableTestData->removeRow(0);
    }
    while (ui->tableTestData->columnCount() > 0)
    {
        ui->tableTestData->removeColumn(0);
    }
    //checking ini file and then load this
    QSettings settings(settings_file,QSettings::IniFormat);
    QListWidgetItem *item = ui->listFunctions->currentItem();
    if(item!=NULL)
    {
        TypeFunction myFunc = FindFunction(item->text().toUtf8());
        saveFunData = item->text().toUtf8();
        QString findItem = settings.value("f_" + className + "_" + saveFunData,"").toString();
        if(findItem.length()==0)//No settings
        {
            argumentCount = myFunc.getArgumentCount();
            ui->tableTestData->setColumnCount(argumentCount + 1);
            ui->tableTestData->setRowCount(1);

            QStringList headerList;
            for(int i = 0; i < myFunc.getArgumentCount(); i++)
            {
                QString arg = myFunc.getFunctionArgumentType(i) + " " + myFunc.getFunctionArgument(i);
                headerList<<arg;
            }
            headerList.append(myFunc.GetFunctionReturnType());
            ui->tableTestData->setHorizontalHeaderLabels(headerList);
            currentRow = 1;
        }
        else
        {
            int columns = settings.beginReadArray(className + "_" + saveFunData + "_Columns");
            ui->tableTestData->setColumnCount(columns);
            QStringList headerList2;
            for(int i = 0; i < columns; i++)
            {
              settings.setArrayIndex(i);
              headerList2<<settings.value(className + "_" + saveFunData + "_col","").toString();
            }
            ui->tableTestData->setHorizontalHeaderLabels(headerList2);
            int rows = settings.value(className + "_" + saveFunData + "_rowCount").toInt();
            currentRow = rows;
            ui->tableTestData->setRowCount(rows);
            settings.beginReadArray(className + "_" + saveFunData + "_Data");
            for(int j = 0; j< rows; j++)
            {
                for(int k = 0; k< columns; k++)
                {
                    settings.setArrayIndex(j*columns + k);
                    QString strVal = settings.value(className + "_" + saveFunData + "_val","").toString();
                    QTableWidgetItem *qtablewidgetitem = new QTableWidgetItem(strVal);
                    ui->tableTestData->setItem(j,k,qtablewidgetitem);
                }
            }
        }
    }
}
//find function from list
TypeFunction MainWindow::FindFunction(QString functionName)
{
    TypeFunction res;
    for(int i = 0; i < listfun->size(); i++)
    {
        TypeFunction function = listfun->at(i);
        if(QString::compare(functionName, function.GetFunctionName())==0)
        {
            return function;
        }
    }
    return res;
}
//add row
void MainWindow::on_btnAdd_clicked()
{
    ui->tableTestData->setRowCount(ui->tableTestData->rowCount()+1);
    currentRow++;
}
//delete last row
void MainWindow::on_btnDel_clicked()
{
    if(ui->tableTestData->rowCount()==0)return;
    ui->tableTestData->removeRow(ui->tableTestData->currentRow());
    currentRow--;
}
//clear all cells
void MainWindow::on_btnClear_clicked()
{
    if(ui->tableTestData->rowCount()==0)return;
    for(int i = 0; i < ui->tableTestData->rowCount();i++)
    {
        for(int j = 0; j < ui->tableTestData->columnCount();j++)
        {
            QTableWidgetItem *qtablewidgetitem = ui->tableTestData->item(i, j);
            if(qtablewidgetitem!=NULL)
            {
                qtablewidgetitem->setText("");
            }
        }
    }

}
//Test generator
void MainWindow::TestGenerator()
{
    //list_args arguments
    QListWidgetItem *item = ui->listFunctions->currentItem();
    if(item==NULL)return;
    TypeFunction myFunc = FindFunction(item->text().toUtf8());
    QString funName = myFunc.GetFunctionName();
    QString funResult = myFunc.GetFunctionReturnType();
    QStringList funArgs = myFunc.getFunctionArguments();
    QStringList funArgsType = myFunc.getFunctionArgumentTypes();
    qmake_path = ui->txtQmake->text();
    minigw_path = ui->txtMinGW->text();

    if(qmake_path.size()==0)
    {
        QMessageBox msgBoxa;
        msgBoxa.setText("Not selected qmake.exe file!");
        msgBoxa.exec();
        return;
    }
    if(minigw_path.size()==0)
    {
        QMessageBox msgBoxb;
        msgBoxb.setText("Not selected mingw32-make file!");
        msgBoxb.exec();
        return;
    }
    QDir().mkdir("gen");
    if(QDir("gen").exists())
    {
        QString getGenFillPath = QDir("gen").absolutePath();
        QString dest;
        dest = "gen/" + source_cpp;
        QFile::copy(fullsource_cpp, dest);
        dest = "gen/" + source_h;
        QFile::copy(fullsource_h, dest);

        //Test_MyClass.h
        dest = "gen/Test_" + className + ".h";
        QFile file1(dest);
        if (!file1.open(QFile::WriteOnly | QFile::Text)) return;
        QTextStream out(&file1);
        out<<"#ifndef TEST_"<<className.toUpper()<<"_H\r\n";
        out<<"#define TEST_"<<className.toUpper()<<"_H\r\n\r\n";
        out<<"#include <QtTest>\r\n";
        out<<"#include \""<<source_h<<"\"\r\n";
        out<<"class Test_"<<className<<" : public QObject {\r\n";
        out<<"Q_OBJECT\r\n";
        out<<"private slots:\r\n";
        for(int i = 0; i<1; i++)
        {
            out<<"\tvoid "<<funName<<"();\r\n";
        }
        out<<"};\r\n\r\n#endif\r\n";
        file1.close();
        //test.cpp
        dest = "gen/test.cpp";
        QFile file2(dest);
        if (!file2.open(QFile::WriteOnly | QFile::Text)) return;
        QTextStream out2(&file2);
        out2<<"#include <QtTest>\r\n";
        out2<<"#include <QDebug>\r\n";
        out2<<"#include \"Test_"<<className<<".h\"\r\n\r\n";
        for(int i = 0; i<1; i++)
        {
            out2<<"void Test_"<<className<<"::"<<funName<<"()\r\n{\r\n";
            out2<<"\t"<<className<<" myClass;\r\n";
            for(int j=0;j<list_args.size();j++)
            {
                out2<<"\tQCOMPARE(myClass."<<funName<<"(";
                for(int k=0;k<list_args.at(j).size();k++)
                {
                    if(k!=list_args.at(j).size()-1)
                    {
                        out2<<list_args.at(j).at(k);
                        if((list_args.at(j).size()>2)&&(k!=list_args.at(j).size()-2))
                        {
                            out2<<", ";
                        }
                    }
                    else
                    {
                        out2<<"), "<<list_args.at(j).at(k)<<");\r\n";
                    }
                }
            }
            out2<<"}\r\n\r\n";
        }
        out2<<"QTEST_MAIN(Test_"<<className<<")\r\n";
        file2.close();
        //test.pro
        dest = "gen/test.pro";
        QFile file3(dest);
        if (!file3.open(QFile::WriteOnly | QFile::Text)) return;
        QTextStream out3(&file3);
        out3<<"SOURCES       = test.cpp "<<source_cpp<<"\r\n";
        out3<<"HEADERS       = "<<source_h<<" Test_"<<className<<".h\r\n";
        out3<<"CONFIG       += qtestlib\r\n";
        file3.close();
        //Run test
        process = new QProcess;
        process->setWorkingDirectory("gen");
        connect(process, SIGNAL(readyReadStandardOutput ()), this, SLOT(logReady()));
        process->start(qmake_path);
        process->waitForFinished();
        QStringList arguments;
        arguments<<"-w";
        process->start(minigw_path,arguments);
        process->waitForFinished();
        dest = "\"" + getGenFillPath + "/debug\"";
        dest +="/test.exe";
        process->start(dest);
        QMessageBox msgBox2;
        msgBox2.setText("DONE!");
        msgBox2.exec();

        while (ui->tableTestData->rowCount() > 0)
        {
            ui->tableTestData->removeRow(0);
        }
        while (ui->tableTestData->columnCount() > 0)
        {
            ui->tableTestData->removeColumn(0);
        }

        return;
    }

}

void MainWindow::logReady(){
    strOutput += process->readAllStandardOutput();
    ui->textConsoleOut->setText(strOutput);
}

void MainWindow::on_btnGetQmake_clicked()
{
    QSettings settings(settings_file,QSettings::IniFormat);
    QString str = QFileDialog::getOpenFileName();
    ui->txtQmake->setText(str);
    settings.setValue("qmake_path", str);
}

void MainWindow::on_btnGetMinGW_clicked()
{
    QSettings settings(settings_file,QSettings::IniFormat);
    QString str = QFileDialog::getOpenFileName();
    ui->txtMinGW->setText(str);
    settings.setValue("minigw_path", str);
}

void MainWindow::tmpSave()
{
    if(ui->tableTestData->item(0,0)==NULL)return;
    QSettings settings(settings_file,QSettings::IniFormat);
    settings.setValue("f_" + className + "_" + saveFunData, saveFunData);
    int columns = ui->tableTestData->columnCount();
    settings.beginWriteArray(className + "_" + saveFunData + "_Columns",columns);
    for(int i = 0; i < columns; i++)
    {
      settings.setArrayIndex(i);
      settings.setValue(className + "_" + saveFunData + "_col",ui->tableTestData->takeHorizontalHeaderItem(i)->text());
    }
    int rows = ui->tableTestData->rowCount();
    settings.setValue(className + "_" + saveFunData + "_rowCount", rows);
    settings.beginWriteArray(className + "_" + saveFunData + "_Data",columns*rows);
    for(int j = 0; j< rows; j++)
    {
        for(int k = 0; k< columns; k++)
        {
            settings.setArrayIndex(j*columns + k);
            settings.setValue(className + "_" + saveFunData + "_val",ui->tableTestData->item(j,k)->text());
        }
    }
}
