#include "mainw.h"

MainW::MainW(QWidget *parent): QMainWindow(parent){
	this->resize(800, 600);
	this->systemMenu = new QMenu("System [&S]", this);
	this->questionMenu = new QMenu("Question [&Q]", this);
	this->helpMenu = new QMenu("Help [&H]", this);

	this->startServerAct = new QAction("Start Server [&S]", this);
	this->stopServerAct = new QAction("Stop Server [&T]", this);
	this->startDockerDaemonAct = new QAction("Start Docker Daemon [&D]", this);
	this->stopDockerDaemonAct = new QAction("Stop Docker Daemon [&P]", this);
	this->exitAct = new QAction("Exit [&X]", this);

	this->mainWidget = new QTabWidget;
	this->dockerDaemon = new DockerDaemon("docker", this);
	this->network = new Network(23333, this);

	this->systemMenu->addAction(this->startServerAct);
	this->systemMenu->addAction(this->stopServerAct);
	this->systemMenu->addSeparator();
	this->systemMenu->addAction(this->startDockerDaemonAct);
	this->systemMenu->addAction(this->stopDockerDaemonAct);
	this->systemMenu->addSeparator();
	this->systemMenu->addAction(this->exitAct);
	this->menuBar()->addMenu(this->systemMenu);

	this->menuBar()->addMenu(this->questionMenu);
	this->menuBar()->addMenu(this->helpMenu);

	this->mainWidget->addTab(dockerDaemon, "Docker Daemon");
	this->mainWidget->addTab(network, "Network");


	this->setCentralWidget(this->mainWidget);

	connect(this->startDockerDaemonAct, SIGNAL(triggered(bool)), this->dockerDaemon, SLOT(openDockerDaemon()));
	connect(this->stopDockerDaemonAct, SIGNAL(triggered(bool)), this->dockerDaemon, SLOT(closeDockerDaemon()));
	connect(this->dockerDaemon, SIGNAL(dockerStarted()), this, SLOT(dockerDaemonOpenStateSlot()));
	connect(this->dockerDaemon, SIGNAL(dockerFinished()), this, SLOT(dockerDaemonCloseStateSlot()));

	connect(this->startServerAct, SIGNAL(triggered(bool)), this->network, SLOT(startServer()));
	connect(this->stopServerAct, SIGNAL(triggered(bool)), this->network, SLOT(stopServer()));
	connect(this->network, SIGNAL(serverStarted()), this, SLOT(serverOpenStateSlot()));
	connect(this->network, SIGNAL(serverStoped()), this, SLOT(serverCloseStateSlot()));

	//Set State
	this->dockerDaemonCloseStateSlot();
	this->serverCloseStateSlot();
}

MainW::~MainW()
{

}
