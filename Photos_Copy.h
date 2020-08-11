#ifndef PHOTOS_COPY_H
#define PHOTOS_COPY_H

#include <iostream>
#include <thread>

#include <QWidget>
#include <QMap>
#include <QFileInfo>
#include <QTreeWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class Photos_Copy; }
QT_END_NAMESPACE

class Photos_Copy : public QWidget
{
    Q_OBJECT

signals:
    void Sign_IndexInformation(int state, QString FilePath);
    void Sign_CopyInformation(int state, unsigned int FullLenth, unsigned int CopyLenth, QString FileName, QString SrcDir, QString TarDir);

public:
    Photos_Copy(QWidget *parent = nullptr);
    ~Photos_Copy();

private slots:
    void Slot_BtnClick();
    void Slot_IndexInformation(int state, QString FilePath);
    void Slot_CopyInformation(int state, unsigned int FullLenth, unsigned int CopyLenth, QString FileName, QString SrcDir, QString TarDir);

private:
    void Index(QString RootPath);
    void IndexThread();

    void CopyThread();

    bool ReadExifInfo(QString Path);

private:
    Ui::Photos_Copy *ui;

    QString SourceRootPath;

    bool m_IndexThreadRunning;

    bool m_CopyThreadRunning;

    QMap<QString, QTreeWidgetItem*> m_SrcFileIndex;
    QMap<QString, QTreeWidgetItem*> m_TarFileIndex;
};
#endif // PHOTOS_COPY_H
