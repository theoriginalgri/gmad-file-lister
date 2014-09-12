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

    void openFile(const QString &fileName);

protected:
    void showError(const QString& message);

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void on_action_Open_triggered();
    void on_action_Copy_to_clipboard_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
