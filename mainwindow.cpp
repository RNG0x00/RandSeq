#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QToolButton>
#include <QPixmap>
#include <QStyle>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QMessageBox>
#include <QSettings>
bool m_bPressed=false,hsis=false,contg=true,ised=false;
int history=0;
QPoint m_point;
QString filedir;
QToolButton *minButton;
QToolButton *closeButton;
QList<QString> hisdata;

void MainWindow::loadfile(){
    QFile fl(filedir);
    fl.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream in(&fl);
    in.readLine();
    while(1){
        QString tmp=in.readLine();
        if(tmp!="[Hidden]")
            ui->listWidget->addItem(tmp);
        else break;
    }
    while(1){
        QString tmp=in.readLine();
        if(tmp!="[History]")
            ui->listWidget_2->addItem(tmp);
        else break;
    }
    ui->checkBox->setChecked(in.readLine().toInt());
    in.readLine();
    ui->checkBox->setChecked(in.readLine().toInt());
    //read all lines
    fl.close();
    updaterem();
    hidsequpd();
}

void MainWindow::savefile(){
    QFile fl(filedir);
    fl.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream out(&fl);
    out<<"[Main]\n";
    for(int cnt=0;cnt<ui->listWidget->count();cnt++)
        out<<ui->listWidget->item(cnt)->text()<<"\n";
    out<<"[Hidden]\n";
    for(int cnt=0;cnt<ui->listWidget_2->count();cnt++)
        out<<ui->listWidget_2->item(cnt)->text()<<"\n";
    out<<"[History]\n"+QString::number(history);
    out<<"\n[UseHidden]\n"<<ui->checkBox->isChecked();
    out<<"\n[UseDNR]\n"<<ui->checkBox_2->isChecked();
    //output
    fl.close();
}

int MainWindow::warpnum(int num){
    if(num==-1)return ui->listWidget->count()-1;
    if(num==ui->listWidget->count()) return 0;
    return num;
}

void MainWindow::updaterem(int cnt){
    ui->label_2->setText(ui->listWidget->item(warpnum(cnt-1))->text());
    ui->label_3->setText(ui->listWidget->item(warpnum(cnt))->text());
    ui->label_4->setText(ui->listWidget->item(warpnum(cnt+1))->text());
}

void MainWindow::hidsequpd(){
    ui->comboBox->clear();
    for(int cnt=0;cnt<ui->listWidget->count();cnt++)
        ui->comboBox->addItem(ui->listWidget->item(cnt)->text());
}

void MainWindow::procedit(){
    if(!ised){
        ised=true;
        ui->label->setText("*"+ui->label->text());
    }
}

void MainWindow::procsave(){
    ised=false;
    QFileInfo fi(filedir);
    ui->label->setText(fi.fileName()+"-RandSeq 6.1.0 beta");//set titlebar text
}

void MainWindow::exitapp(){
    if(ised){
            QMessageBox msgBox;
            msgBox.setText("The document has been modified.");
            msgBox.setInformativeText("Do you want to save your changes?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();
            switch (ret) {
                case QMessageBox::Save:
                    if(filedir=="")
                        filedir=QFileDialog::getSaveFileName(this,"Save File","","*.rng");
                    savefile();
                    break;
                case QMessageBox::Discard:
                    break;
                case QMessageBox::Cancel:
                    return;
                    // Cancel was clicked
                default:
                    // should never be reached
                    break;
            }
        }
    QApplication::exit();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton&&event->pos().y()<31){
        m_bPressed = true;
        m_point = event->pos();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_bPressed = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bPressed)
    move(event->pos()-m_point+pos());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowTitle("Random 6.1.0beta");
    setMinimumSize(size());
    minButton = new QToolButton(this);
    closeButton = new QToolButton(this);
    QPixmap minPix  = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
    QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
    minButton->setIcon(minPix);
    closeButton->setIcon(closePix);
    minButton->setGeometry(width()-45,5,20,20);
    closeButton->setGeometry(width()-25,5,20,20);
    minButton->setStyleSheet("background-color:transparent;");
    closeButton->setStyleSheet("background-color:transparent;");
    connect(closeButton, SIGNAL(clicked()), this, SLOT(exitapp()));
    connect(minButton, SIGNAL(clicked()), this, SLOT(showMinimized()));
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label_2->setAlignment(Qt::AlignCenter);
    ui->label_3->setAlignment(Qt::AlignCenter);
    ui->label_4->setAlignment(Qt::AlignCenter);
    ui->frame->setVisible(false);
    QString className("RandSeq6");
    QString ext(".ranlist");
    QString extDes("Random List");
    QString appPath(QApplication::applicationDirPath());
    QString baseUrl("HKEY_CURRENT_USER\\Software\\Classes");
    QSettings settingClasses(baseUrl, QSettings::NativeFormat);
    settingClasses.setValue("/" + className + "/Shell/Open/Command/.", "\"" + appPath + "\" \"%1\"");
    settingClasses.setValue("/" + className + "/.", extDes);
    settingClasses.setValue("/" + className + "/DefaultIcon/.", appPath + ",0");
    settingClasses.setValue("/" + ext + "/OpenWithProgIds/" + className, "");
    settingClasses.sync();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    filedir=QFileDialog::getOpenFileName(this,"Open File","","Random Lists(*ranlist)");//select file
    if(filedir==""){
        QMessageBox::warning(this,"Warning","No File Selected");
        return;
    }
    QFileInfo fi(filedir);
    ui->label->setText(fi.fileName()+"-RandSeq 6.1.0 beta");//set titlebar text
    loadfile();
}


void MainWindow::on_pushButton_2_clicked()
{
    if(ui->lineEdit->text()==""){
        QMessageBox::warning(this,"Warning","Object Empty");
    }
    ui->listWidget->addItem(ui->lineEdit->text());
    ui->lineEdit->clear();
    procedit();
}


void MainWindow::on_pushButton_3_clicked()
{
    ui->listWidget->takeItem(ui->listWidget->currentRow());
    hidsequpd();
    procedit();
}


void MainWindow::on_pushButton_5_clicked()
{
    savefile();
    procsave();
}


void MainWindow::on_pushButton_6_clicked()
{
    filedir=QFileDialog::getSaveFileName(this,"Save File",".ranlist","Random Lists(*ranlist)");//select file
    QFileInfo fi(filedir);
    ui->label->setText(fi.fileName()+"-RandSeq 6.1.0 beta");//set titlebar text
    savefile();
    procsave();
}


void MainWindow::on_pushButton_8_clicked()
{
    int index=ui->listWidget->currentRow();
    QListWidgetItem *item=ui->listWidget->currentItem();
    ui->listWidget->takeItem(ui->listWidget->currentRow());
    ui->listWidget->insertItem(index-1,item);
    ui->listWidget->setCurrentRow(index-1);
    hidsequpd();
    procedit();
}


void MainWindow::on_pushButton_9_clicked()
{
    int index=ui->listWidget->currentRow();
    QListWidgetItem *item=ui->listWidget->currentItem();
    ui->listWidget->takeItem(ui->listWidget->currentRow());
    ui->listWidget->insertItem(index+1,item);
    ui->listWidget->setCurrentRow(index+1);
    hidsequpd();
    procedit();
}


void MainWindow::on_pushButton_4_clicked()
{
    if(!ui->listWidget->count()){//if no element created
        QMessageBox::warning(this,"Warning","Cannot Generate With Empty Sequence");
        return;
    }
    contg=true;
    int cnt=0;
    updaterem(0);
    while(contg){
        if(ui->checkBox_2->isChecked())
            if(hisdata.contains(ui->label_3->text()))
                continue;
        if(cnt==ui->listWidget->count()){
            cnt=0;
            continue;
        }
        updaterem(cnt);
        cnt++;
        QApplication::processEvents();
    }
    if(ui->checkBox->isChecked()&&history!=ui->listWidget_2->count()){
        int tmp=-1;
        for(int rcnt=0;rcnt<ui->listWidget->count();rcnt++)
            if(ui->listWidget->item(rcnt)->text()==ui->listWidget_2->item(history)->text()){
                tmp=rcnt;
                break;
            }
        if(tmp!=-1)updaterem(tmp);
        history++;
    }
    if(ui->checkBox_2->isChecked())
        hisdata.append(ui->label_3->text());
}


void MainWindow::on_pushButton_7_clicked()
{
    contg=false;
}


void MainWindow::on_pushButton_11_clicked()
{
    ui->listWidget->clear();
    for(int i=ui->lineEdit_2->text().toInt();i<=ui->lineEdit_3->text().toInt();i+=ui->lineEdit_4->text().toInt())
        ui->listWidget->addItem(QString::number(i));
}


void MainWindow::on_pushButton_12_clicked()
{
    if(hsis){
        ui->frame->setVisible(false);
        ui->pushButton_12->setText("Show Hidden Sequence");
        hsis=false;
    }
    else{
        ui->frame->setVisible(true);
        ui->pushButton_12->setText("Hide Hidden Sequence");
        hsis=true;
    }
}


void MainWindow::on_pushButton_10_clicked()
{
    ui->listWidget_2->addItem(ui->comboBox->currentText());
    procedit();
}


void MainWindow::on_pushButton_14_clicked()
{
    ui->listWidget_2->takeItem(ui->listWidget_2->currentRow());
    procedit();
}


void MainWindow::on_pushButton_15_clicked()
{
    int index=ui->listWidget_2->currentRow();
    QListWidgetItem *item=ui->listWidget_2->currentItem();
    ui->listWidget_2->takeItem(ui->listWidget_2->currentRow());
    ui->listWidget_2->insertItem(index-1,item);
    ui->listWidget_2->setCurrentRow(index-1);
    procedit();
}


void MainWindow::on_pushButton_16_clicked()
{
    int index=ui->listWidget_2->currentRow();
    QListWidgetItem *item=ui->listWidget_2->currentItem();
    ui->listWidget_2->takeItem(ui->listWidget_2->currentRow());
    ui->listWidget_2->insertItem(index+1,item);
    ui->listWidget_2->setCurrentRow(index+1);
    procedit();
}


void MainWindow::on_pushButton_13_clicked()
{
    history=0;
    hisdata.clear();
}


void MainWindow::on_pushButton_17_clicked()
{
    ui->listWidget->clear();
    ui->listWidget_2->clear();
    ui->comboBox->clear();
    filedir="";
    history=0;
}


void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
}


void MainWindow::on_listWidget_2_itemDoubleClicked(QListWidgetItem *item)
{
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
}

