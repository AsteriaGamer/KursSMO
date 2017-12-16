#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
	void RunAndShow();

private:
    Ui::MainWindow *ui;
	double countPercents(double, double);
};

#endif // MAINWINDOW_H
