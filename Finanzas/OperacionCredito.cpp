#include "OperacionCredito.h"

OperacionCredito::OperacionCredito (QObject* parent) : Operacion (parent) {
	this->operationType = OperacionesFinancieras::TiposDeOperacion::CasoCredito;
}

OperacionCredito::~OperacionCredito () {

}

bool OperacionCredito::validate () {
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
	if (this->montoDesem_1 <= 0 && !this->isEntity_ImpuestosNacionales) {
		emit notifyValidationStatus (OperationValidationErros::DESEM_ERROR, "El desembolso 1 debe ser mayor a cero");
		return  false;
	}
	if (this->montoDesem_2 < 0) {
		emit notifyValidationStatus (OperationValidationErros::DESEM_ERROR, "El desembolso 2 debe ser mayor a cero");
		return  false;
	}
	if (this->montoDesem_3 < 0) {
		emit notifyValidationStatus (OperationValidationErros::DESEM_ERROR, "El desembolso 3 debe ser mayor a cero");
		return  false;
	}
	if (this->montoDesem_4 < 0) {
		emit notifyValidationStatus (OperationValidationErros::DESEM_ERROR, "El desembolso 4 debe ser mayor a cero");
		return  false;
	}
	if (this->montoDesem_5 < 0) {
		emit notifyValidationStatus (OperationValidationErros::DESEM_ERROR, "El desembolso 5 debe ser mayor a cero");
		return  false;
	}
	double desemTotal = 0;
	desemTotal += montoDesem_1;
	desemTotal += montoDesem_2;
	desemTotal += montoDesem_3;
	desemTotal += montoDesem_4;
	desemTotal += montoDesem_5;
	if (this->ammount != desemTotal) {
		emit notifyValidationStatus (OperationValidationErros::DESEM_ERROR, QString::fromLatin1 ("La suma de los desembolsos debe ser igual al monto de la operación"));
		return  false;
	}
	return true;
}

void OperacionCredito::save (QString targetURL, QString token) {
	if (validate ()) {
		QNetworkAccessManager* nam = new QNetworkAccessManager (this);
		QNetworkRequest request;
		request.setUrl (QUrl (targetURL + "/planDePagos"));
		request.setRawHeader ("Content-Type", "application/json");
		request.setRawHeader ("token", token.toUtf8 ());

		connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
			QJsonDocument response = QJsonDocument::fromJson (reply->readAll ());
			if (reply->error ()) {
				qDebug () << "response error " << reply->error ();
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
		//bodyContent.insert ("iva", );
		//bodyContent.insert ("cuotaInicial", );
		bodyContent.insert ("tipoDeTasa", OperacionesFinancieras::MapTipoTasaEnum (this->rateType));
		bodyContent.insert ("interesFijo", this->staticRate);
		if (this->rateType == OperacionesFinancieras::TipoTasa::Variable) {
			bodyContent.insert ("interesVariable", this->dynamicRate);
		}
		bodyContent.insert ("plazo", this->term);
		bodyContent.insert ("frecuenciaDePagos", OperacionesFinancieras::MapFrecuenciaEnum (this->frequency));
		bodyContent.insert ("fechaVencimiento", QDateTime (this->expirationDate).toMSecsSinceEpoch ());
		bodyContent.insert ("fechaDesembolso_1", QDateTime (this->fechaDesem_1).toMSecsSinceEpoch ());
		bodyContent.insert ("montoDesembolso_1", this->montoDesem_1);
		if (this->montoDesem_2 != 0) {
			bodyContent.insert ("fechaDesembolso_2", QDateTime (this->fechaDesem_2).toMSecsSinceEpoch ());
			bodyContent.insert ("montoDesembolso_2", this->montoDesem_2);
		}
		if (this->montoDesem_3 != 0) {
			bodyContent.insert ("fechaDesembolso_3", QDateTime (this->fechaDesem_3).toMSecsSinceEpoch ());
			bodyContent.insert ("montoDesembolso_3", this->montoDesem_3);
		}
		if (this->montoDesem_4 != 0) {
			bodyContent.insert ("fechaDesembolso_4", QDateTime (this->fechaDesem_4).toMSecsSinceEpoch ());
			bodyContent.insert ("montoDesembolso_4", this->montoDesem_4);
		}
		if (this->montoDesem_5 != 0) {
			bodyContent.insert ("fechaDesembolso_5", QDateTime (this->fechaDesem_5).toMSecsSinceEpoch ());
			bodyContent.insert ("montoDesembolso_5", this->montoDesem_5);
		}
		//bodyContent.insert ("lineaDeCredito", );
		bodyContent.insert ("empresaGrupo", this->enterprise);
		bodyContent.insert ("entidadFinanciera", this->entity);


		body.setObject (bodyContent);
		nam->post (request, body.toJson ());
	}
}

void OperacionCredito::load (int id, QString targetURL, QString token) {

}

void OperacionCredito::update (QString targetURL, QString token) {

}

