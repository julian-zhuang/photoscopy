#include "Photos_Copy.h"
#include "ui_Photos_Copy.h"

#include <QDir>
#include "ExivInfo.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QDateTime>
#include <QTimer>

Photos_Copy::Photos_Copy(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Photos_Copy)
{
    ui->setupUi(this);
    m_IndexThreadRunning = false;
    m_CopyThreadRunning = false;

    connect(ui->pushButton_Stop, &QPushButton::clicked, this, &Photos_Copy::Slot_BtnClick);
    connect(ui->pushButton_Start, &QPushButton::clicked, this, &Photos_Copy::Slot_BtnClick);
    connect(ui->pushButton_RefreshIndex, &QPushButton::clicked, this, &Photos_Copy::Slot_BtnClick);
    connect(ui->pushButton_SelectTargetPath, &QPushButton::clicked, this, &Photos_Copy::Slot_BtnClick);
    connect(ui->pushButton_SlelctSourcePath, &QPushButton::clicked, this, &Photos_Copy::Slot_BtnClick);

    connect(this, &Photos_Copy::Sign_IndexInformation, this, &Photos_Copy::Slot_IndexInformation);
    connect(this, &Photos_Copy::Sign_CopyInformation, this, &Photos_Copy::Slot_CopyInformation);

    ui->progressBar_Task->setVisible(false);
    ui->progressBar_File->setVisible(false);
    ui->pushButton_Stop->setEnabled(false);
}

Photos_Copy::~Photos_Copy()
{
    delete ui;
}

void Photos_Copy::Slot_BtnClick()
{
    QPushButton *Btn = static_cast<QPushButton*>(sender());
    if (Btn == nullptr){
        return;
    }
    if (Btn == ui->pushButton_Start){
        m_CopyThreadRunning = true;
        auto ThreadPtr = new std::thread(&Photos_Copy::CopyThread, this);
        ThreadPtr->detach();
        return;
    }
    if (Btn == ui->pushButton_Stop){
        m_CopyThreadRunning = false;
        return;
    }

    if (Btn == ui->pushButton_RefreshIndex){
        SourceRootPath = ui->lineEdit_SourcePath->text();
        auto Thread_Ptr = new std::thread(&Photos_Copy::IndexThread, this);
        Thread_Ptr->detach();
        return;
    }

//    QString HomePath =
    auto DirPath = QFileDialog::getExistingDirectory(this, "choose src Directory", QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first());
    if (DirPath.isEmpty()){
        return;
    }

    if (Btn == ui->pushButton_SlelctSourcePath){
        ui->lineEdit_SourcePath->setText(DirPath);
        SourceRootPath = DirPath;
        ui->treeWidget_Source->clear();
        auto Thread_Ptr = new std::thread(&Photos_Copy::IndexThread, this);
        Thread_Ptr->detach();
    }

    if (Btn == ui->pushButton_SelectTargetPath){
        ui->LineEdit_TargetPath->setText(DirPath);
    }
}

void Photos_Copy::Slot_IndexInformation(int state, QString FilePath)
{
    /*
     * state:
     *       0 : Start
     *       1 : FindFile
     *       2 : Stop
    */
    if (state == 0){
        ui->treeWidget_Source->clear();
        ui->treeWidget_Target->clear();
        m_SrcFileIndex.clear();
        m_TarFileIndex.clear();
        return;
    }
    if (state == 1){
        QFileInfo FileInfo(FilePath);
        QTreeWidgetItem *Item = new QTreeWidgetItem;
        Item->setText(0, FileInfo.fileName());
        Item->setText(1, QString::number(FileInfo.size()));
        Item->setText(2, FileInfo.absolutePath());
        ui->treeWidget_Source->addTopLevelItem(Item);

        m_SrcFileIndex[FileInfo.absoluteFilePath()] = Item;
    }
}

void Photos_Copy::Slot_CopyInformation(int state, unsigned int FullLenth, unsigned int CopyLenth, QString FileName, QString SrcDir, QString TarDir)
{
    if (state == 0){
        ui->pushButton_Stop->setEnabled(true);
        ui->pushButton_Start->setEnabled(false);
        ui->pushButton_RefreshIndex->setEnabled(false);
        ui->pushButton_SelectTargetPath->setEnabled(false);
        ui->pushButton_SlelctSourcePath->setEnabled(false);

        ui->progressBar_Task->setVisible(true);
        ui->progressBar_File->setVisible(true);

        ui->lineEdit_SourcePath->setEnabled(false);
        ui->LineEdit_TargetPath->setEnabled(false);
        ui->lineEdit_TargetDirectroFormat->setEnabled(false);
        ui->progressBar_Task->setMaximum(m_SrcFileIndex.count());
        ui->progressBar_Task->setValue(0);
        return;
    }

    if (state == 1){
        ui->label_FileName->setText("Copying " + FileName);
        ui->progressBar_File->setMaximum(FullLenth);
        return;
    }

    if (state == 2){
        ui->progressBar_File->setValue(CopyLenth);
        return;
    }

    if (state == 3){
        QTreeWidgetItem *Item = new QTreeWidgetItem;
        QFileInfo FileInfo(TarDir + "/" + FileName);
        Item->setText(0, FileInfo.fileName());
        Item->setText(1, QString::number(FileInfo.size()));
        Item->setText(2, FileInfo.absolutePath());
        ui->treeWidget_Target->addTopLevelItem(Item);

        auto absoluteFilePath = SrcDir + "/" + FileName;
        delete m_SrcFileIndex[absoluteFilePath];
        m_SrcFileIndex.remove(absoluteFilePath);
        m_TarFileIndex[absoluteFilePath] = Item;

        ui->progressBar_Task->setValue(m_TarFileIndex.count());
        return;
    }

    if (state == 4){
        ui->pushButton_Stop->setEnabled(false);
        ui->pushButton_Start->setEnabled(true);
        ui->pushButton_RefreshIndex->setEnabled(true);
        ui->pushButton_SelectTargetPath->setEnabled(true);
        ui->pushButton_SlelctSourcePath->setEnabled(true);

        ui->lineEdit_SourcePath->setEnabled(true);
        ui->LineEdit_TargetPath->setEnabled(true);
        ui->lineEdit_TargetDirectroFormat->setEnabled(true);
        ui->label_FileName->setText("Copy end.");

        QTimer::singleShot(5000, [&]{
           ui->label_FileName->setText("");

           ui->progressBar_Task->setVisible(false);
           ui->progressBar_Task->setValue(0);
           ui->progressBar_Task->setMaximum(1);

           ui->progressBar_File->setVisible(false);
           ui->progressBar_File->setValue(0);
           ui->progressBar_File->setMaximum(1);
        });

        return;
    }
}

void Photos_Copy::Index(QString RootPath)
{
    QDir Directory(RootPath);
    if(!Directory.exists())
    {
        return;
    }
    Directory.setFilter(QDir::AllEntries | QDir::NoSymLinks);
    QFileInfoList FIleList = Directory.entryInfoList();
    foreach(auto var, FIleList){
        if (var.isDir()){
            if (var.fileName() == "." || var.fileName() == ".."){
                continue;
            }
            Index(var.absoluteFilePath());
        }

        if (var.isSymLink()){
            continue;
        }
        if (var.isFile()){
            if (var.suffix().toUpper() == "DB"){
                continue;
            }
            if (var.suffix().toUpper() == "XML"){
                continue;
            }
            emit Sign_IndexInformation(1, var.absoluteFilePath());
        }
    }
}

void Photos_Copy::IndexThread()
{
    emit Sign_IndexInformation(0, "");
    Index(SourceRootPath);
    emit Sign_IndexInformation(2, "");
}

void Photos_Copy::CopyThread()
{
    emit Sign_CopyInformation(0, 0, 0, "", "", "");
    QList<QString> Files;
    QTreeWidgetItemIterator it(ui->treeWidget_Source);
    while (*it) {
         Files.append((*it)->text(2) + "/" + (*it)->text(0));
        ++it;
    }
    QByteArray FileData;
    std::string ExifValue;
    QString TargetDirectroFormat = ui->lineEdit_TargetDirectroFormat->text();
    foreach(auto var, Files){
        if (m_CopyThreadRunning == false){
            break;
        }
        ExivInfo SrcExifInfo(var.toStdString());
        SrcExifInfo.GetExifKey("Exif.Image.DateTime", ExifValue);
        QString DirectroPath = ui->LineEdit_TargetPath->text() + "/";
                DirectroPath += QDateTime::fromString(QString::fromStdString(ExifValue), "yyyy:MM:dd hh:mm:ss").toString(TargetDirectroFormat);

        QDir TargetDirectro;
        if (TargetDirectro.exists(DirectroPath) == false){
            TargetDirectro.mkpath(DirectroPath);
        }

        QFileInfo SrcFileInfo(var);
        QFile SrcFile(var);
        if (SrcFile.open(QIODevice::ReadOnly) == false){
            continue;
        }

        QFile TargetFile(DirectroPath + "/" + SrcFileInfo.fileName());
        if (TargetFile.open(QIODevice::WriteOnly) == false){
            SrcFile.close();
            continue;
        }

        FileData.clear();
        unsigned int CopyedLenth = 0;
        emit Sign_CopyInformation(1, SrcFileInfo.size(), CopyedLenth, SrcFileInfo.fileName(), SrcFileInfo.absolutePath(), DirectroPath);
        do{
            FileData = SrcFile.read(4096);
            CopyedLenth += FileData.length();
            TargetFile.write(FileData);
            emit Sign_CopyInformation(2, SrcFileInfo.size(), CopyedLenth, SrcFileInfo.fileName(), SrcFileInfo.absolutePath(), DirectroPath);
        }while(FileData.size() == 4096);
        SrcFile.close();
        if (ui->checkBox_DeleteDourceFile->checkState() == Qt::Checked){
            SrcFile.remove();
        }
        TargetFile.close();
        emit Sign_CopyInformation(3, SrcFileInfo.size(), CopyedLenth, SrcFileInfo.fileName(), SrcFileInfo.absolutePath(), DirectroPath);
    }
    emit Sign_CopyInformation(4, 0, 0, "", "", "");
    m_CopyThreadRunning = false;
}

bool Photos_Copy::ReadExifInfo(QString Path)
{
    ExivInfo Info(Path.toStdString());
    if (Info.IsEmpty() == true){
        return false;
    }
    return true;
}
