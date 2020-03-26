#include "OperacionImpuestosNacionales.h"

#include "CuotasPlanDePagos.h"

QString targetAddress3, token_ID3;


OperacionImpuestosNacionales::OperacionImpuestosNacionales(QObject *parent): Operacion(parent) {
	this->operationType = OperacionesFinancieras::TiposDeOperacion::CasoImpuestosNacionales;
}

OperacionImpuestosNacionales::~OperacionImpuestosNacionales() {

}

bool OperacionImpuestosNacionales::validate () {
	if (this->contractNumber == "") {
		emit notifyValidationStatus (OperationValidationErros::CONTRACT_ERROR);
		return  false;
	}
	if (this->enterprise == 0) {
		emit notifyValidationStatus (OperationValidationErros::ENTERPRISE_ERROR);
		return  false;
	}
	if (this->entity == 0) {
		emit notifyValidationStatus (OperationValidationErros::ENTITY_ERROR);
		return  false;
	}
	if (this->currency == OperacionesFinancieras::Moneda::NONE) {
		emit notifyValidationStatus (OperationValidationErros::CURRENCY_ERROR);
		return  false;
	}
	if (this->ammount <= 0) {
		emit notifyValidationStatus (OperationValidationErros::AMMOUNT_ERROR);
		return  false;
	}
	if (this->initialDue < 0) {
		emit notifyValidationStatus (OperationValidationErros::INITIAL_DUE_ERROR);
		return  false;
	}
	if (this->warranty < 0) {
		emit notifyValidationStatus (OperationValidationErros::INITIAL_DUE_ERROR);
		return  false;
	}
	if ((this->initialDue + this->warranty) > this->ammount) {
		emit notifyValidationStatus (OperationValidationErros::INITIAL_DUE_ERROR);
		return  false;
	}
	if (this->frequency == OperacionesFinancieras::FrecuenciaDePagos::NONE) {
		emit notifyValidationStatus (OperationValidationErros::FREQ_ERROR);
		return  false;
	}
	if (this->term <= 0) {
		emit notifyValidationStatus (OperationValidationErros::TERM_ERROR);
		return  false;
	}
	return true;
}

void OperacionImpuestosNacionales::save (QString targetURL, QString token) {
	if (validate ()) {
		targetAddress3 = targetURL;
		token_ID3 = token;

		QNetworkAccessManager* nam = new QNetworkAccessManager (this);
		QNetworkRequest request;
		request.setUrl (QUrl (targetURL + "/planDePagos"));
		request.setRawHeader ("Content-Type", "application/json");
		request.setRawHeader ("token", token.toUtf8 ());

		connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
			QJsonDocument response = QJsonDocument::fromJson (reply->readAll ());
			if (reply->error ()) {
				if (response.object ().value ("err").toObject ().contains ("message")) {
					// If there is a known error
					emit notifyValidationStatus (OperationValidationErros::SERVER_SIDE_ERROR, QString::fromLatin1 (response.object ().value ("err").toObject ().value ("message").toString ().toLatin1 ()));
				}
				else {
					if (reply->error () == QNetworkReply::ConnectionRefusedError) {
						emit notifyValidationStatus (OperationValidationErros::SERVER_SIDE_ERROR, QString::fromLatin1 ("No se pudo establecer conexión con el servidor"));
					}
					else {
						// If there is a server error
						emit notifyValidationStatus (OperationValidationErros::SERVER_SIDE_ERROR, QString::fromStdString (response.toJson ().toStdString ()));
					}
				}
			}
			else {
				this->id = response.object ().value ("planDePagos").toObject ().value ("id").toInt ();
				createFirstDue (targetAddress3, token_ID3);

				emit notifyValidationStatus (OperationValidationErros::NO_ERROR);
			}
			reply->deleteLater ();
			});

		QJsonDocument body;
		QJsonObject bodyContent;

		bodyContent.insert ("tipoOperacion", OperacionesFinancieras::MapOperationEnum (this->operationType));
		bodyContent.insert ("numeroDeContratoOperacion", this->contractNumber);
		bodyContent.insert ("fechaFirma", QDateTime (this->signDate).toMSecsSinceEpoch ());
		bodyContent.insert ("concepto", this->concept);
		bodyContent.insert ("detalle", this->detail);
		bodyContent.insert ("moneda", OperacionesFinancieras::MapMonedaEnum (this->currency));
		bodyContent.insert ("monto", this->ammount);
		bodyContent.insert ("cuotaInicial", this->initialDue);
		bodyContent.insert ("garantia", this->warranty);
		bodyContent.insert ("plazo", this->term);
		bodyContent.insert ("frecuenciaDePagos", OperacionesFinancieras::MapFrecuenciaEnum (this->frequency));
		bodyContent.insert ("fechaVencimiento", QDateTime (this->expirationDate).toMSecsSinceEpoch ());
		bodyContent.insert ("empresaGrupo", this->enterprise);
		bodyContent.insert ("entidadFinanciera", this->entity);

		body.setObject (bodyContent);
		nam->post (request, body.toJson ());
	}
}

void OperacionImpuestosNacionales::update (QString targetURL, QString token) {
	if (validate ()) {
		QNetworkAccessManager* nam = new QNetworkAccessManager (this);
		QNetworkRequest request;
		request.setUrl (QUrl (targetURL + "/planDePagos/" + QString::number (this->getID ())));
		request.setRawHeader ("Content-Type", "application/json");
		request.setRawHeader ("token", token.toUtf8 ());

		connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
			QJsonDocument response = QJsonDocument::fromJson (reply->readAll ());
			if (reply->error ()) {
				if (response.object ().value ("err").toObject ().contains ("message")) {
					// If there is a known error
					emit notifyValidationStatus (OperationValidationErros::SERVER_SIDE_ERROR, QString::fromLatin1 (response.object ().value ("err").toObject ().value ("message").toString ().toLatin1 ()));
				}
				else {
					if (reply->error () == QNetworkReply::ConnectionRefusedError) {
						emit notifyValidationStatus (OperationValidationErros::SERVER_SIDE_ERROR, QString::fromLatin1 ("No se pudo establecer conexión con el servidor"));
					}
					else {
						// If there is a server error
						emit notifyValidationStatus (OperationValidationErros::SERVER_SIDE_ERROR, QString::fromStdString (response.toJson ().toStdString ()));
					}
				}
			}
			else {
				emit notifyValidationStatus (OperationValidationErros::NO_ERROR);
			}
			reply->deleteLater ();
			});

		QJsonDocument body;
		QJsonObject bodyContent;

		bodyContent.insert ("tipoOperacion", OperacionesFinancieras::MapOperationEnum (this->operationType));
		bodyContent.insert ("numeroDeContratoOperacion", this->contractNumber);
		bodyContent.insert ("fechaFirma", QDateTime (this->signDate).toMSecsSinceEpoch ());
		bodyContent.insert ("concepto", this->concept);
		bodyContent.insert ("detalle", this->detail);
		bodyContent.insert ("moneda", OperacionesFinancieras::MapMonedaEnum (this->currency));
		bodyContent.insert ("monto", this->ammount);
		bodyContent.insert ("iva", this->iva);
		bodyContent.insert ("cuotaInicial", this->initialDue);
		bodyContent.insert ("garantia", this->warranty);
		bodyContent.insert ("plazo", this->term);
		bodyContent.insert ("frecuenciaDePagos", OperacionesFinancieras::MapFrecuenciaEnum (this->frequency));
		bodyContent.insert ("fechaVencimiento", QDateTime (this->expirationDate).toMSecsSinceEpoch ());
		bodyContent.insert ("empresaGrupo", this->enterprise);
		bodyContent.insert ("entidadFinanciera", this->entity);

		body.setObject (bodyContent);
		nam->put (request, body.toJson ());
	}
}

void OperacionImpuestosNacionales::createFirstDue (QString targetURL, QString token) {
	CuotasPlanDePagos* currentDue = new CuotasPlanDePagos (this);

	double total = this->getInitialDue () + this->getWarranty ();
	double capital = total;

	currentDue->setDueNumber (0);
	currentDue->setDueDate (this->getSignDate ());
	currentDue->setTotal (total);
	currentDue->setCapital (capital);
	currentDue->setInterest (0);
	currentDue->setIva (0);

	currentDue->setParentID (this->getID ());


	currentDue->save (targetURL, token);
	//===============================================================================================================
	// Network manager and request
	QNetworkAccessManager* nam = new QNetworkAccessManager (this);
	QNetworkRequest request;
	request.setUrl (QUrl (targetURL + "/cuotaEfectiva"));
	request.setRawHeader ("Content-Type", "application/json");
	request.setRawHeader ("token", token.toUtf8 ());

	// On response lambda
	connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
		QJsonDocument jsonReply = QJsonDocument::fromJson (reply->readAll ());
		reply->deleteLater ();
		});

	// Request body
	QJsonDocument body;
	QJsonObject bodyContent;

	bodyContent.insert ("numeroDeCuota", 0);
	bodyContent.insert ("fechaDePago", QDateTime (this->getSignDate ()).toMSecsSinceEpoch ());
	bodyContent.insert ("montoTotalDelPago", total);
	bodyContent.insert ("pagoDeCapital", capital);
	bodyContent.insert ("pagoDeInteres", 0);
	bodyContent.insert ("pagoDeIva", 0);

	bodyContent.insert ("parent", this->getID ());

	body.setObject (bodyContent);
	nam->post (request, body.toJson ());
}
