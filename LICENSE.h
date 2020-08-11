#ifndef LICENSE_H
#define LICENSE_H

#include <QDialog>

namespace Ui {
class LICENSE;
}

class LICENSE : public QDialog
{
    Q_OBJECT

public:
    explicit LICENSE(QWidget *parent = nullptr);
    ~LICENSE();

private:
    Ui::LICENSE *ui;
};

#endif // LICENSE_H
