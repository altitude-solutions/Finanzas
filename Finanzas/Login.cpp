#include "Login.h"

// Json imports
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
// Network imports
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
// File management imports
#include <QFile>
#include <QDir>
// Other imports
#include <QPixMap>
#include <QMessageBox>

Login::Login(QWidget *parent): QWidget(parent) {
	ui.setupUi(this);
	// Create and hide second window
	mainWindow.hide ();
	this->setFixedSize ( QSize(301, 200) );

	// set login icon
	QPixmap icon (":/Resources/img/Logo-LPL.png");
	ui.iconLabel->setPixmap ( icon.scaled( 100, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

	// listen to enter key for username and password line edits
	connect (ui.userName, &QLineEdit::returnPressed, ui.loginButton, &QPushButton::click);
	connect (ui.password, &QLineEdit::returnPressed, ui.loginButton, &QPushButton::click);

	// login button clicked
	connect (ui.loginButton, SIGNAL (clicked ()), this, SLOT (onLoginButtonClicked()));

	// logout event
	connect (&mainWindow, &TabsWindow::logout, this, &Login::onLogout);

	// emit authentication data
	connect (this, &Login::authDataRetrieved, &mainWindow, &TabsWindow::onLoginSuccess);
	
	// Set target address
	QDir tempDir = QDir::tempPath();
	if (!tempDir.exists ("LPL")) {
		tempDir.mkdir ("LPL");
	}
	if (!tempDir.cd ("LPL")) {
		this->close ();
	}
	QString configFileName = "config.json";
	if (!tempDir.exists (configFileName)) {
		// config file does not exist
		QJsonDocument defaultJson;
		QJsonObject content;
		content.insert ("api_url", "http://200.105.171.52:3000");
		defaultJson.setObject (content);

		QFile configFile (tempDir.filePath (configFileName));
		if (configFile.open (QFile::WriteOnly)) {
			configFile.write (defaultJson.toJson ());
			configFile.flush ();
			configFile.close ();
		}
		else {
			this->close ();
		}
		targetAddress = "http://200.105.171.52:3000";
	}
	else {
		// config file exists
		QFile configFile (tempDir.filePath (configFileName));
		if (configFile.open(QFile::ReadOnly)) {
			QJsonDocument config = QJsonDocument::fromJson (configFile.readAll ());
			targetAddress = config.object ().value ("api_url").toString ();
			configFile.close ();
		}
		else {
			this->close ();
		}
	}
}

void Login::onLoginButtonClicked () {
	QNetworkAccessManager *nam = new QNetworkAccessManager(this);
	// request callback
	connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
		QByteArray resBin = reply->readAll ();
		if (reply->error ()) {
			QJsonDocument errorJson = QJsonDocument::fromJson( resBin );
			if (errorJson.object ().value ("err").toObject ().contains ("message")) {
				// If there is a known error
				QMessageBox::critical (this, "Error", QString::fromLatin1 (errorJson.object ().value ("err").toObject ().value ("message").toString ().toLatin1 ()));
			}
			else {
				if (reply->error () == QNetworkReply::ConnectionRefusedError) {
					QMessageBox::critical (this, QString::fromLatin1 ("Error de conexión"), QString::fromLatin1 ("No se pudo establecer conexión con el servidor"));
				}else{
					// If there is a server error
					QMessageBox::critical (this, "Error en base de datos", "Por favor enviar un reporte de error con una captura de pantalla de esta venta.\n" + QString::fromStdString (errorJson.toJson ().toStdString ()));
				}
			}
			// enable login button
			ui.loginButton->setEnabled (true);
			return;
		}
		QJsonDocument response = QJsonDocument::fromJson ( resBin );
		QStringList permissions;

		// Clear password field
		ui.password->setText ("");

		foreach (QJsonValue perm, response.object ().value ("user").toObject ().value ("permisos").toArray ()) {
			permissions << perm.toString ();
		}
		//qDebug () << "Permisos" << permissions;

		// enable login button
		ui.loginButton->setEnabled (true);

		// Emit authentication data to other windows
		emit authDataRetrieved (targetAddress, response.object().value("user").toObject().value("nombreUsuario").toString(), QString::fromLatin1 ( response.object().value("user").toObject().value("nombreReal").toString().toLatin1() ), response.object().value("token").toString());
		// swap windows
		mainWindow.show ();
		this->hide ();
		reply->deleteLater ();
	});

	QNetworkRequest req;
	req.setUrl (QUrl (targetAddress + "/login"));
	req.setRawHeader ("Content-Type", "application/json");
	QJsonDocument body;
	QJsonObject bodyContent;
	// Append username and password to request body
	bodyContent.insert ("nombreUsuario", ui.userName->text ());
	bodyContent.insert ("contra", ui.password->text ());
	body.setObject (bodyContent);

	// Post request
	nam->post (req, body.toJson ());
	
	// disabale login button to prevent multiple windows from opening
	ui.loginButton->setEnabled (false);
}

void Login::onLogout () {
	// Swap windows on logout
	mainWindow.hide ();
	this->show ();
}