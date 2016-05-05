#include "mainw.h"

void MainW::closeEvent(QCloseEvent *e){
	if(this->dockerDaemon->isRunning() || this->network->isRunning() || this->dockerRest->isRunning()){
		int result = QMessageBox::question(this, "Ruilx Judge Server", "this program has some daemon run in background, force stop these will lose its all progress, do you really want to exit?", QMessageBox::Yes | QMessageBox::No);
		if(result == QMessageBox::Yes){
			this->dockerDaemon->closeDockerDaemon();
			this->network->stopServer();
			this->dockerRest->closeSocket();
			e->accept();
			return;
		}else{
			e->ignore();
			return;
		}
	}else{
		e->accept();
	}
}

MainW::MainW(QWidget *parent): QMainWindow(parent){
	this->resize(800, 600);
	this->systemMenu = new QMenu("System[&S]", this);
	this->questionMenu = new QMenu("Question[&Q]", this);
	this->helpMenu = new QMenu("Help[&H]", this);
	this->dockerCmdMenu = new QMenu("Docker Command", this);

	this->startServerAct = new QAction("Start Server [&S]", this);
	this->stopServerAct = new QAction("Stop Server [&T]", this);
	this->startDockerDaemonAct = new QAction("Start Docker Daemon [&D]", this);
	this->stopDockerDaemonAct = new QAction("Stop Docker Daemon [&P]", this);
	this->startDockerRestAct = new QAction("Open Docker REST [&O]", this);
	this->stopDockerRestAct = new QAction("Close Docker REST [&C]", this);
	this->exitAct = new QAction("Exit [&X]", this);

	this->problemHelloWorldAct = new QAction("Hello, world!", this);
	this->problemAPlusBAct = new QAction("A+B problem", this);

	this->mainWidget = new QTabWidget;
	this->dockerDaemon = new DockerDaemon("/usr/bin/docker", this);
	this->network = new Network(23333, this);
	this->dockerRest = new DockerRest("/var/run/docker.sock", this);
	this->complieOutput = new CompileOutput(this);

	this->systemMenu->addAction(this->startServerAct);
	this->systemMenu->addAction(this->stopServerAct);
	this->systemMenu->addSeparator();
	this->systemMenu->addAction(this->startDockerDaemonAct);
	this->systemMenu->addAction(this->stopDockerDaemonAct);
	this->systemMenu->addSeparator();
	this->systemMenu->addAction(this->startDockerRestAct);
	this->systemMenu->addAction(this->stopDockerRestAct);
	this->systemMenu->addSeparator();
	this->systemMenu->addAction(this->exitAct);
	this->menuBar()->addMenu(this->systemMenu);

	this->questionMenu->addAction(this->problemHelloWorldAct);
	this->questionMenu->addAction(this->problemAPlusBAct);
	this->menuBar()->addMenu(this->questionMenu);

	this->menuBar()->addMenu(this->dockerCmdMenu);

	this->menuBar()->addMenu(this->helpMenu);

	this->mainWidget->addTab(dockerDaemon, "Docker Daemon");
	this->mainWidget->addTab(network, "Network");
	this->mainWidget->addTab(dockerRest, "docker REST log");
	this->mainWidget->addTab(complieOutput, "Compile");


	this->setCentralWidget(this->mainWidget);

	connect(this->startDockerDaemonAct, SIGNAL(triggered(bool)), this->dockerDaemon, SLOT(openDockerDaemon()));
	connect(this->stopDockerDaemonAct, SIGNAL(triggered(bool)), this->dockerDaemon, SLOT(closeDockerDaemon()));
	connect(this->dockerDaemon, SIGNAL(dockerStarted()), this, SLOT(dockerDaemonOpenStateSlot()));
	connect(this->dockerDaemon, SIGNAL(dockerFinished()), this, SLOT(dockerDaemonCloseStateSlot()));

	connect(this->startServerAct, SIGNAL(triggered(bool)), this->network, SLOT(startServer()));
	connect(this->stopServerAct, SIGNAL(triggered(bool)), this->network, SLOT(stopServer()));
	connect(this->network, SIGNAL(serverStarted()), this, SLOT(serverOpenStateSlot()));
	connect(this->network, SIGNAL(serverStoped()), this, SLOT(serverCloseStateSlot()));

	connect(this->startDockerRestAct, SIGNAL(triggered(bool)), this->dockerRest, SLOT(openSocket()));
	connect(this->stopDockerRestAct, SIGNAL(triggered(bool)), this->dockerRest, SLOT(closeSocket()));
	connect(this->dockerRest, SIGNAL(socketOpened()), this, SLOT(dockerRestStartStateSlot()));
	connect(this->dockerRest, SIGNAL(socketClosed()), this, SLOT(dockerRestStopStateSlot()));

	connect(this->problemHelloWorldAct, SIGNAL(triggered(bool)), this, SLOT(problemHelloworldSlot()));
	//Set State
	this->dockerDaemonCloseStateSlot();
	this->serverCloseStateSlot();
	this->dockerRestStopStateSlot();
}

MainW::~MainW()
{

}
