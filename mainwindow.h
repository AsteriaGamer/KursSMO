#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
	void RunAndShow();
    void UpdateS(QString, QString);
    void saveFile();
    void clearEdit();
    void barMove(int);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
