#include "LICENSE.h"
#include "ui_LICENSE.h"

LICENSE::LICENSE(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LICENSE)
{
    ui->setupUi(this);
}

LICENSE::~LICENSE()
{
    delete ui;
}
