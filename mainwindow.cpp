#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    settings=new QSettings("iLag","QtEdge");
    ui->setupUi(this);
    settings->beginGroup("Main");
    load_save(settings->value("lastUsedFile","").toString());
    settings->endGroup();

#ifndef Q_OS_UNIX
    ui->privlegeBox->hide();
    ui->elevatorBox->hide();
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_output()
{
    ui->output->appendPlainText(edge->readAllStandardOutput());
}

void MainWindow::edge_closed()
{
    ui->scrollAreaWidgetContents->setEnabled(true);
    ui->connect->setEnabled(true);
    ui->disconnect->setEnabled(false);
}

void MainWindow::on_connect_clicked()
{
    ui->scrollAreaWidgetContents->setEnabled(false);
    ui->connect->setEnabled(false);
    ui->disconnect->setEnabled(true);

    QString node = ui->nodeIp->text();
    QString community = ui->communityName->text();
    QString key = ui->key->text();
    QString clientIp = ui->clientIP->text();
    QString edgeExe = ui->exePath->text();
#ifdef Q_OS_UNIX
    QString elevator = ui->elevator->text();
    QString user = ui->user_id->text();
    QString group = ui->group_id->text();
#endif
    QString tun = ui->tun_name->text();
    QString mac = ui->mac_addr->text();
    QString mtu = ui->mtu_value->text();
    QString netmask = ui->netmask_addr->text();

    ui->output->clear();

    edge = new QProcess();
    QString command;
    QStringList parameters;
#ifdef Q_OS_UNIX
    if (elevator.length()){
        command=elevator;
        parameters.push_back(edgeExe);
    } else {
        command=edgeExe;
    }
#else
    command=edgeExe;
#endif

    if (tun.length()) {
        parameters.push_back("-d");
        parameters.push_back(tun);
    }

    parameters.push_back("-a");
    parameters.push_back(clientIp);
    parameters.push_back("-c");
    parameters.push_back(community);
    parameters.push_back("-k");
    parameters.push_back(key);
    parameters.push_back("-l");
    parameters.push_back(node);

#ifdef Q_OS_UNIX
    parameters.push_back("-u");
    if (user.length()){
        if ((user.toInt() == 0) && !(user=="0")){
            edge->start("id -u " + user);
            edge->waitForFinished();
            user=edge->readAllStandardOutput();
        }
    } else {
        edge->start("id -u");
        edge->waitForFinished();
        user=edge->readAllStandardOutput();
    }
    parameters.push_back(user);

    parameters.push_back("-g");
    if (group.length()){
        if ((group.toInt() == 0) && !(group=="0")){
            edge->start("id -g " + group);
            edge->waitForFinished();
            group=edge->readAllStandardOutput();
        }
    } else {
        edge->start("id -g");
        edge->waitForFinished();
        group=edge->readAllStandardOutput();
    }
    parameters.push_back(group);
#endif

    if (mac.length()){
        parameters.push_back("-m");
        parameters.push_back(mac);
    }
    if (mtu.length()){
        parameters.push_back("-M");
        parameters.push_back(mtu);
    }
    if (netmask.length()){
        parameters.push_back("-s");
        parameters.push_back(netmask);
    }

    edge->start(command,parameters);
    connect(edge,SIGNAL(readyReadStandardOutput()),this,SLOT(update_output()));
    connect(edge,SIGNAL(finished(int , QProcess::ExitStatus )),this,SLOT(edge_closed()));
}

void MainWindow::on_disconnect_clicked()
{
    edge->terminate();
}

void MainWindow::on_browse_for_edge_clicked()
{
    ui->exePath->setText(QFileDialog::getOpenFileName(this,"Locate n2n Edge executable","",""));
}

void MainWindow::load_save(QString fileName){
    if ((fileName.isNull()) || (fileName=="")){
        return;
    }
    if (QFileInfo(fileName).exists()){
        setActiveFile(fileName);
    }
    QSettings saveFile(fileName,QSettings::IniFormat);
    ui->nodeIp->setText(saveFile.value("node","0.0.0.0:00000").toString());
    ui->communityName->setText(saveFile.value("community","MyCommunity").toString());
    ui->key->setText(saveFile.value("key","MyKey").toString());
    ui->clientIP->setText(saveFile.value("clientIp","0.0.0.0").toString());
    ui->exePath->setText(saveFile.value("exePath","edge").toString());
    ui->elevator->setText(saveFile.value("elevator","").toString());
    ui->user_id->setText(saveFile.value("user","").toString());
    ui->group_id->setText(saveFile.value("group","").toString());
    ui->tun_name->setText(saveFile.value("tunName","").toString());
    ui->mac_addr->setText(saveFile.value("mac","").toString());
    ui->mtu_value->setText(saveFile.value("mtu","").toString());
    ui->netmask_addr->setText(saveFile.value("netmask","").toString());
    ui->hostnameResolution->setChecked(saveFile.value("resolveHostname","False").toBool());
    ui->httpTunnel->setChecked(saveFile.value("httpTunnel","False").toBool());
    ui->packetForwarding->setChecked(saveFile.value("packetForwarding","False").toBool());
    ui->verboseLogging->setChecked(saveFile.value("verbose","False").toBool());


}

void MainWindow::save_file(QString fileName){
    if ((fileName.isNull()) || (fileName==""))
        return;
    setActiveFile(fileName);
    QSettings saveFile(fileName,QSettings::IniFormat);
    saveFile.setValue("node",ui->nodeIp->text());
    saveFile.setValue("community",ui->communityName->text());
    saveFile.setValue("key",ui->key->text());
    saveFile.setValue("clientIp",ui->clientIP->text());
    saveFile.setValue("exePath",ui->exePath->text());
    saveFile.setValue("elevator",ui->elevator->text());
    saveFile.setValue("user",ui->user_id->text());
    saveFile.setValue("group",ui->group_id->text());
    saveFile.setValue("tunName",ui->tun_name->text());
    saveFile.setValue("mac",ui->mac_addr->text());
    saveFile.setValue("mtu",ui->mtu_value->text());
    saveFile.setValue("netmask",ui->netmask_addr->text());
    saveFile.setValue("resolveHostname",ui->hostnameResolution->checkState());
    saveFile.setValue("httpTunnel",ui->httpTunnel->checkState());
    saveFile.setValue("packetForwarding",ui->packetForwarding->checkState());
    saveFile.setValue("verbose",ui->verboseLogging->checkState());
}

void MainWindow::setActiveFile(QString fileName){
    activeFile=fileName;
    settings->beginGroup("Main");
    settings->setValue("lastUsedFile",fileName);
}

void MainWindow::on_actionNew_triggered()
{
    load_save("");
}

void MainWindow::on_actionOpen_triggered()
{
    QString inputFile=QFileDialog::getOpenFileName(this,"Load QtEdge settings","","QtEdge settings file (*.qte);;All Files(*)");
    if (!inputFile.isNull()){
        load_save(inputFile);
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (!activeFile.isNull()){
        save_file(activeFile);
    } else {
        on_actionSave_As_triggered();
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    QString outputFile=QFileDialog::getSaveFileName(this,"Save QtEdge settings","","QtEdge settings file (*.qte)");
    if (outputFile.isNull()){
           return;
    }
    if (outputFile.split(".").length()==1){
        outputFile.append(".qte");
    }
    save_file(outputFile);
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}



