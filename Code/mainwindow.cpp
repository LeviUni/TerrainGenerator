#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "math.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// --- Functions that listen for widget events
// forewards to the mainview

void MainWindow::on_ResetRotationButton_clicked(bool checked)
{
    Q_UNUSED(checked);
    ui->RotationDialX->setValue(354);
    ui->RotationDialY->setValue(0);
    ui->mainView->setRotation(354, 0);
}

void MainWindow::on_RotationDialX_sliderMoved(int value)
{
    ui->mainView->setRotation(value, ui->RotationDialY->value());
}

void MainWindow::on_RotationDialY_sliderMoved(int value)
{
    ui->mainView->setRotation(ui->RotationDialX->value(), value);
}

void MainWindow::on_ResetScaleButton_clicked(bool checked)
{
    Q_UNUSED(checked);
    ui->ScaleSlider->setValue(30);
    ui->mainView->setScale(30);
}

void MainWindow::on_ScaleSlider_sliderMoved(int value)
{
    ui->mainView->setScale(value);
}

void MainWindow::on_RegenerationButton_clicked(bool checked) {
    Q_UNUSED(checked);
    ui->mainView->regenerate();
}
