#include "AppHeader.h"
#include <QDateTime>
#include <QPixMap>

AppHeader::AppHeader(QWidget *parent): QWidget(parent) {
	ui.setupUi(this);
	// set header icon
	QPixmap icon (":/Resources/img/Logo-LPL.png");
	ui.icon->setPixmap (icon.scaled (90, 72, Qt::KeepAspectRatio, Qt::SmoothTransformation));

	// timer timeout
	connect (&timer, &QTimer::timeout, this, &AppHeader::updateClock);
	timer.start (1000);

	// logout button clicked
	connect (ui.close_button, &QPushButton::clicked, this, [&]() {
		emit logoutButton ();
	});
}

AppHeader::~AppHeader() {

}

// Set app name and user name
void AppHeader::setAppName (QString appName) {
	ui.appTitle->setText (appName);
}

void AppHeader::setUserName (QString userName) {
	ui.label_user->setText (userName);
}

// update clock
void AppHeader::updateClock () {
	ui.label_date->setText (QDateTime::currentDateTime ().toString ("dd/MM/yyyy HH:mm:ss") );
}