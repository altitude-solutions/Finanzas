#include "PagosEfectivos.h"

// Network imports
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

// Json imports
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

// Other imports
#include <QMessageBox>


PagosEfectivos::PagosEfectivos(QWidget *parent): QWidget(parent) {
	ui.setupUi(this);

	// ==================================================================
	// Load planes de pago info
	// ==================================================================
}

PagosEfectivos::~PagosEfectivos(){

}

void PagosEfectivos::setAuthData (QString address, QString token, QString userName) {
	this->targetAddress = address;
	this->token = token;
	this->userName = userName;

	loadEmpresasGrupo ();
	loadEntidadesFinancieras ();
}

// On Tab selected
// Use it to setup current tab
void PagosEfectivos::onTabSelected () {
	// Tab setup
}

// Finish cycle
void PagosEfectivos::saveButtonClicked () {

}

// Start cycle
void PagosEfectivos::findButtonClicked () {

}

void PagosEfectivos::setTableHeaders () {
	// ==================================================================
	// Set table headers
	// ==================================================================
	ui.tableWidget->setColumnCount (12);
	QStringList headers = {
		QString::fromLatin1 ("Cuotas"), QString::fromLatin1 ("Estado"),
		QString::fromLatin1 ("Empres Grupo"), QString::fromLatin1 ("Tipo de Operación"),
		QString::fromLatin1 ("Fecha de Pago"), QString::fromLatin1 ("Pago total efectivo"),
		QString::fromLatin1 ("Pago de capital efectivo"), QString::fromLatin1 ("Pago de interés efectivo"),
		QString::fromLatin1 ("Pago de IVA efectivo"), QString::fromLatin1 ("saldo de capital"),
		QString::fromLatin1 ("Crédito fiscal"), QString::fromLatin1 ("Saldo Capital Real")
	};
	ui.tableWidget->setHorizontalHeaderLabels (headers);
}

void PagosEfectivos::loadEmpresasGrupo () {
	listaEmpresas.clear ();
	ui.grupo->clear ();
	QNetworkAccessManager* nam = new QNetworkAccessManager (this);
	connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
		QByteArray resBin = reply->readAll ();
		if (reply->error ()) {
			QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
			QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
			return;
		}
		QJsonDocument okJson = QJsonDocument::fromJson (resBin);
		foreach (QJsonValue entidad, okJson.object ().value ("empresas").toArray ()) {
			ui.grupo->addItem (entidad.toObject ().value ("empresa").toString ());
			listaEmpresas.insert (entidad.toObject ().value ("empresa").toString (), QString::number (entidad.toObject ().value ("id").toInt ()));
		}
		reply->deleteLater ();
	});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/empresas?status=1"));

	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}

void PagosEfectivos::loadEntidadesFinancieras () {
	listaEntidades.clear ();
	ui.entidad->clear ();
	QNetworkAccessManager* nam = new QNetworkAccessManager (this);
	connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
		QByteArray resBin = reply->readAll ();
		if (reply->error ()) {
			QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
			QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
			return;
		}
		QJsonDocument okJson = QJsonDocument::fromJson (resBin);
		foreach (QJsonValue entidad, okJson.object ().value ("entidades").toArray ()) {
			ui.entidad->addItem (entidad.toObject ().value ("nombreEntidad").toString ());
			QHash<QString, QString> current;
			current.insert ("id", QString::number (entidad.toObject ().value ("id").toInt ()));
			current.insert ("nombreEntidad", entidad.toObject ().value ("nombreEntidad").toString ());
			current.insert ("tipoDeEntidad", QString::number (entidad.toObject ().value ("tipoDeEntidad").toInt ()));
			listaEntidades.insert (entidad.toObject ().value ("nombreEntidad").toString (), current);
		}
		reply->deleteLater ();
	});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/entidadFinanciera?status=1"));

	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}

void PagosEfectivos::loadPlanesData () {

}
