#include "OperacionLeasing.h"

#include "CuotasPlanDePagos.h"

QString targetAddress;
QString token_ID;

OperacionLeasing::OperacionLeasing(QObject *parent): Operacion(parent) {
	this->operationType = OperacionesFinancieras::TiposDeOperacion::CasoLeasing;
}

OperacionLeasing::~OperacionLeasing() {

}

bool OperacionLeasing::validate () {
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
	if (this->iva <= 0) {
		emit notifyValidationStatus (OperationValidationErros::IVA_ERROR);
		return  false;
	}
	if (this->initialDue <= 0 || this->initialDue > this->ammount) {
		emit notifyValidationStatus (OperationValidationErros::INITIAL_DUE_ERROR);
		return  false;
	}
	if (this->rateType == OperacionesFinancieras::TipoTasa::NONE) {
		emit notifyValidationStatus (OperationValidationErros::RATE_TYPE_ERROR);
		return  false;
	}
	if (this->staticRate <= 0) {
		emit notifyValidationStatus (OperationValidationErros::S_RATE_ERROR);
		return  false;
	}
	if (this->rateType == OperacionesFinancieras::TipoTasa::Variable && this->dynamicRate <= 0) {
		emit notifyValidationStatus (OperationValidationErros::D_RATE_ERROR);
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

void OperacionLeasing::save (QString targetURL, QString token) {
	if (validate ()) {
		targetAddress = targetURL;
		token_ID = token;

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
				createFirstDue (targetAddress, token_ID);

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
		bodyContent.insert ("tipoDeTasa", OperacionesFinancieras::MapTipoTasaEnum (this->rateType));
		bodyContent.insert ("interesFijo", this->staticRate);
		if (this->rateType == OperacionesFinancieras::TipoTasa::Variable) {
			bodyContent.insert ("interesVariable", this->dynamicRate);
		}
		bodyContent.insert ("plazo", this->term);
		bodyContent.insert ("frecuenciaDePagos", OperacionesFinancieras::MapFrecuenciaEnum (this->frequency));
		bodyContent.insert ("fechaVencimiento", QDateTime (this->expirationDate).toMSecsSinceEpoch ());
		bodyContent.insert ("empresaGrupo", this->enterprise);
		bodyContent.insert ("entidadFinanciera", this->entity);

		body.setObject (bodyContent);
		nam->post (request, body.toJson ());
	}
}

void OperacionLeasing::update (QString targetURL, QString token) {
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
		bodyContent.insert ("tipoDeTasa", OperacionesFinancieras::MapTipoTasaEnum (this->rateType));
		bodyContent.insert ("interesFijo", this->staticRate);
		if (this->rateType == OperacionesFinancieras::TipoTasa::Variable) {
			bodyContent.insert ("interesVariable", this->dynamicRate);
		}
		bodyContent.insert ("plazo", this->term);
		bodyContent.insert ("frecuenciaDePagos", OperacionesFinancieras::MapFrecuenciaEnum (this->frequency));
		bodyContent.insert ("fechaVencimiento", QDateTime (this->expirationDate).toMSecsSinceEpoch ());
		bodyContent.insert ("empresaGrupo", this->enterprise);
		bodyContent.insert ("entidadFinanciera", this->entity);

		body.setObject (bodyContent);
		nam->put (request, body.toJson ());
	}
}

void OperacionLeasing::createFirstDue (QString targetURL,  QString token) {
	CuotasPlanDePagos* currentDue = new CuotasPlanDePagos (this);

	double capital = 0.87 * this->getInitialDue ();
	QString aux = QString::number (this->getInitialDue () - capital, 'f', 2);
	double iva = aux.toDouble ();

	currentDue->setDueNumber (1);
	currentDue->setDueDate (this->getSignDate ());
	currentDue->setTotal (this->getInitialDue ());
	currentDue->setCapital (capital);
	currentDue->setInterest (0);
	currentDue->setIva (iva);

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

	bodyContent.insert ("numeroDeCuota", 1);
	bodyContent.insert ("fechaDePago", QDateTime (this->getSignDate ()).toMSecsSinceEpoch ());
	bodyContent.insert ("montoTotalDelPago", this->getInitialDue ());
	bodyContent.insert ("pagoDeCapital", capital);
	bodyContent.insert ("pagoDeInteres", 0);
	bodyContent.insert ("pagoDeIva", iva);

	bodyContent.insert ("parent", this->getID ());

	body.setObject (bodyContent);
	nam->post (request, body.toJson ());
}
