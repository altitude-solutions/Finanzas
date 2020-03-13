#include "CuotasPlanDePagos.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>


CuotasPlanDePagos::CuotasPlanDePagos(QObject *parent): QObject(parent) {
	id = 0;
	dueNumber = 0;
	dueDate = QDate::currentDate ();
	totalAmmount = 0;
	capitalDue = 0;
	interestDue = 0;
	ivaDue = 0;

	parent_ID = 0;
}

CuotasPlanDePagos::~CuotasPlanDePagos() {

}

//==================================================================================================================================
//====================================================== getters and setters =======================================================
void CuotasPlanDePagos::setDueNumber (int dueNumber) {
	this->dueNumber = dueNumber;
}

void CuotasPlanDePagos::setDueDate (QDate date) {
	this->dueDate = date;
}

void CuotasPlanDePagos::setTotal (double ammount) {
	this->totalAmmount = ammount;
}

void CuotasPlanDePagos::setCapital (double ammount) {
	this->capitalDue = ammount;
}

void CuotasPlanDePagos::setInterest (double ammount) {
	this->interestDue = ammount;
}

void CuotasPlanDePagos::setIva (double ammount) {
	this->ivaDue = ammount;
}

void CuotasPlanDePagos::setParentID (int parentID) {
	this->parent_ID = parentID;
}

int CuotasPlanDePagos::getID () {
	return this->id;
}

int CuotasPlanDePagos::getDueNumber () {
	return this->dueNumber;
}

QDate CuotasPlanDePagos::getDueDate () {
	return this->dueDate;
}

double CuotasPlanDePagos::getTotal () {
	return this->totalAmmount;
}

double CuotasPlanDePagos::getCapital () {
	return this->capitalDue;
}

double CuotasPlanDePagos::getInterest () {
	return this->interestDue;
}

double CuotasPlanDePagos::getIva () {
	return this->ivaDue;
}

int CuotasPlanDePagos::getParentID () {
	return this->parent_ID;
}
//==================================================================================================================================

void CuotasPlanDePagos::save (QString targetUrl, QString token) {
	QNetworkAccessManager* nam = new QNetworkAccessManager (this);
	QNetworkRequest request;
	request.setUrl (QUrl (targetUrl + "/cuotaPlanDePagos"));
	request.setRawHeader ("Content-Type", "application/json");
	request.setRawHeader ("token", token.toUtf8 ());

	connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {

		QJsonDocument response = QJsonDocument::fromJson (reply->readAll ());
		if (reply->error ()) {
			if (response.object ().value ("err").toObject ().contains ("message")) {
				// If there is a known error
				emit notifyValidationStatus (DueValidationError::SERVER_SIDE_ERROR, QString::fromLatin1 (response.object ().value ("err").toObject ().value ("message").toString ().toLatin1 ()));
			}
			else {
				if (reply->error () == QNetworkReply::ConnectionRefusedError) {
					emit notifyValidationStatus (DueValidationError::SERVER_SIDE_ERROR, QString::fromLatin1 ("No se pudo establecer conexión con el servidor"));
				}
				else {
					// If there is a server error
					emit notifyValidationStatus (DueValidationError::SERVER_SIDE_ERROR, QString::fromStdString (response.toJson ().toStdString ()));
				}
			}
		}
		else {
			this->id = response.object ().value ("cuotaPlanDePagos").toObject ().value ("id").toInt ();
			emit notifyValidationStatus (DueValidationError::NO_ERROR);
		}
		reply->deleteLater ();
		});


	QJsonDocument body;
	QJsonObject bodyContent;

	bodyContent.insert ("numeroDeCuota", this->dueNumber);
	bodyContent.insert ("fechaDePago", QDateTime (this->dueDate).toMSecsSinceEpoch ());
	bodyContent.insert ("montoTotalDelPago", this->totalAmmount);
	bodyContent.insert ("pagoDeCapital", this->capitalDue);
	bodyContent.insert ("pagoDeInteres", this->interestDue);
	if (this->ivaDue != 0) {
		bodyContent.insert ("pagoDeIva", this->ivaDue);
	}
	bodyContent.insert ("parent", this->parent_ID);

	body.setObject (bodyContent);
	nam->post (request, body.toJson ());
}


void CuotasPlanDePagos::update (QString targetUrl, QString token) {

}
