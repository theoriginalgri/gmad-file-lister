#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gmadreader.h"

#include <QTextStream>
#include <QClipboard>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->toolBar->addAction(ui->action_Open);
    ui->toolBar->addAction(ui->action_Copy_to_clipboard);

    // Disable copy to clipboard by default
    ui->action_Copy_to_clipboard->setEnabled(false);

    setUnifiedTitleAndToolBarOnMac(true);
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        showError(tr("Error opening file: %1").arg(file.errorString()));
        return;
    }

    GMadReader reader;
    if (!reader.parse(&file)) {
        showError(tr("Error parsing file: %1").arg(reader.errorString()));
        return;
    }

    delete ui->treeView->model();

    QStandardItemModel *model = new QStandardItemModel(this);

    foreach(const GMadContentFileInfo &fileInfo, reader.files()) {
        QStandardItem *item = new QStandardItem(fileInfo.name);
        model->appendRow(item);
    }

    ui->treeView->setModel(model);
    ui->action_Copy_to_clipboard->setEnabled(model->rowCount() != 0);
}

void MainWindow::on_action_Open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select a gmad file"), QString(), "*.gma");
    if (fileName.isEmpty())
        return;

    openFile(fileName);
}

void MainWindow::showError(const QString &message)
{
    QMessageBox::critical(this, windowTitle(), message);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QUrl url = mimeData->urls().at(0);
        QFileInfo fileInfo(url.toLocalFile());

        if (fileInfo.suffix() == "gma")
            event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QUrl url = event->mimeData()->urls().at(0);
    openFile(url.toLocalFile());
}

void MainWindow::on_action_Copy_to_clipboard_triggered()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->treeView->model());
    if (!model)
        return;

    QString text;
    QTextStream stream(&text, QIODevice::WriteOnly);

    int count = model->rowCount();
    for(int i=0; i < count; ++i) {
        QStandardItem *item = model->item(i);

        stream << item->text() << '\n';
    }

    // Actually copy the text to the clipboard
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
}
