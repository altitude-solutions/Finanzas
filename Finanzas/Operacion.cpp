#include "Operacion.h"


#include "OperacionCredito.h"
#include "OperacionLineaDeCredito.h"


#include <QEventLoop>

Operacion::Operacion(QObject *parent): QObject(parent) {
	id = 0;
	operationType = OperacionesFinancieras::TiposDeOperacion::NONE;
	contractNumber = "";
	signDate = QDate::currentDate();
	concept = "";
	detail = "";
	currency = OperacionesFinancieras::Moneda::NONE;
	ammount = 0;
	iva = 0;
	rateType = OperacionesFinancieras::TipoTasa::NONE;
	staticRate = 0;
	dynamicRate = 0;
	term = 0;
	frequency = OperacionesFinancieras::FrecuenciaDePagos::NONE;
	expirationDate = QDate::currentDate ();
	// other tables
	enterprise = 0;
	entity = 0;
	creditLine = 0;

	// Desembolsos
	fechaDesem_1 = QDate::currentDate ();
	montoDesem_1 = 0;
	fechaDesem_2 = QDate::currentDate ();
	montoDesem_2 = 0;
	fechaDesem_3 = QDate::currentDate ();
	montoDesem_3 = 0;
	fechaDesem_4 = QDate::currentDate ();
	montoDesem_4 = 0;
	fechaDesem_5 = QDate::currentDate ();
	montoDesem_5 = 0;

	initialDue = 0;
}

Operacion::~Operacion() {

}

Operacion* Operacion::load (int id, QString targetURL, QString token, QObject* parent) {
	Operacion* op = nullptr;

	QNetworkAccessManager* nam = new QNetworkAccessManager (parent);
	/*connect (nam, &QNetworkAccessManager::finished, parent, [&](QNetworkReply* reply) {
		QByteArray resBin = reply->readAll ();
		if (reply->error ()) {
			QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
			return;
		}
		QJsonDocument okJson = QJsonDocument::fromJson (resBin);

		OperacionesFinancieras::TiposDeOperacion opType = OperacionesFinancieras::MapOperationString (okJson.object ().value ("planDePagos").toObject ().value ("tipoOperacion").toString ());
		switch (opType) {
		case OperacionesFinancieras::TiposDeOperacion::CasoCredito:
			op = new OperacionCredito (parent);
			break;
		case OperacionesFinancieras::TiposDeOperacion::CasoLineaDeCredito:
			op = new OperacionLineaDeCredito (parent);
			break;
		case OperacionesFinancieras::TiposDeOperacion::CasoLeasing:
			break;
		case OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack:
			break;
		case OperacionesFinancieras::TiposDeOperacion::CasoSeguro:
			break;
		case OperacionesFinancieras::TiposDeOperacion::NONE:
			return;
			break;
		}
		op->setID (okJson.object ().value ("planDePagos").toObject ().value ("id").toInt());
		op->setContractNumber (okJson.object ().value ("planDePagos").toObject ().value ("numeroDeContratoOperacion").toString ());
		op->setSignDate (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaFirma").toVariant ().toLongLong ()).date ());
		op->setConcept (okJson.object ().value ("planDePagos").toObject ().value ("concepto").toString ());
		op->setDetail (okJson.object ().value ("planDePagos").toObject ().value ("detalle").toString ());
		op->setCurrency (OperacionesFinancieras::MapMonedaString (okJson.object ().value ("planDePagos").toObject ().value ("moneda").toString ()));
		op->setAmmount (okJson.object ().value ("planDePagos").toObject ().value ("monto").toDouble ());
		if (!okJson.object ().value ("planDePagos").toObject ().value ("iva").isNull ()) {
			op->setIVA (okJson.object ().value ("planDePagos").toObject ().value ("iva").toDouble ());
		}
		op->setRateType (OperacionesFinancieras::MapTipoTasaString (okJson.object ().value ("planDePagos").toObject ().value ("tipoDeTasa").toString ()));
		op->setStaticRate (okJson.object ().value ("planDePagos").toObject ().value ("interesFijo").toDouble ());
		if (!okJson.object ().value ("planDePagos").toObject ().value ("interesVariable").isNull ()) {
			op->setDynamicRate (okJson.object ().value ("planDePagos").toObject ().value ("interesVariable").toDouble ());
		}
		op->setTerm (okJson.object ().value ("planDePagos").toObject ().value ("plazo").toInt ());
		op->setFrequency (OperacionesFinancieras::MapFrecuenciaString (okJson.object ().value ("planDePagos").toObject ().value ("frecuenciaDePagos").toString ()));
		op->setExpirationDate (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaVencimiento").toVariant ().toLongLong ()).date ());
		op->setEnterprise (okJson.object ().value ("planDePagos").toObject ().value ("empresas_grupo").toObject ().value ("id").toInt());
		op->setEntity (okJson.object ().value ("planDePagos").toObject ().value ("entidades_financiera").toObject ().value ("id").toInt());
		if (!okJson.object ().value ("planDePagos").toObject ().value ("lineas_de_credito").isNull ()) {
			op->setCreditLine (okJson.object ().value ("planDePagos").toObject ().value ("lineas_de_credito").toObject ().value ("id").toInt ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("cuotaInicial").isNull ()) {
			op->setInitialDue (okJson.object ().value ("planDePagos").toObject ().value ("cuotaInicial").toDouble ());
		}

		if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_1").isNull ()) {
			op->setMontoDesem_1 (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_1").toDouble ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_1").isNull ()) {
			op->setFechaDesem_1 (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_1").toVariant ().toLongLong ()).date ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_2").isNull ()) {
			op->setMontoDesem_2 (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_2").toDouble ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_2").isNull ()) {
			op->setFechaDesem_2 (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_2").toVariant ().toLongLong ()).date ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_3").isNull ()) {
			op->setMontoDesem_3 (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_3").toDouble ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_3").isNull ()) {
			op->setFechaDesem_3 (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_3").toVariant ().toLongLong ()).date ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_4").isNull ()) {
			op->setMontoDesem_4 (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_4").toDouble ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_4").isNull ()) {
			op->setFechaDesem_4 (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_4").toVariant ().toLongLong ()).date ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_5").isNull ()) {
			op->setMontoDesem_5 (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_5").toDouble ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_5").isNull ()) {
			op->setFechaDesem_5 (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_5").toVariant ().toLongLong ()).date ());
		}

		reply->deleteLater ();
		});*/

	QNetworkRequest request;
	request.setUrl (QUrl (targetURL + "/planDePagos/" + QString::number (id)));

	request.setRawHeader ("token", token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	QNetworkReply* reply = nam->get (request);

	QEventLoop eventLoop;
	connect (reply, SIGNAL (finished ()), &eventLoop, SLOT (quit ()));
	eventLoop.exec ();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	QByteArray resBin = reply->readAll ();
	if (reply->error ()) {
		QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
	}
	else {
		QJsonDocument okJson = QJsonDocument::fromJson (resBin);

		OperacionesFinancieras::TiposDeOperacion opType = OperacionesFinancieras::MapOperationString (okJson.object ().value ("planDePagos").toObject ().value ("tipoOperacion").toString ());
		switch (opType) {
		case OperacionesFinancieras::TiposDeOperacion::CasoCredito:
			op = new OperacionCredito (parent);
			break;
		case OperacionesFinancieras::TiposDeOperacion::CasoLineaDeCredito:
			op = new OperacionLineaDeCredito (parent);
			break;
		case OperacionesFinancieras::TiposDeOperacion::CasoLeasing:
			break;
		case OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack:
			break;
		case OperacionesFinancieras::TiposDeOperacion::CasoSeguro:
			break;
		case OperacionesFinancieras::TiposDeOperacion::NONE:
			break;
		}
		op->setID (okJson.object ().value ("planDePagos").toObject ().value ("id").toInt ());
		op->setContractNumber (okJson.object ().value ("planDePagos").toObject ().value ("numeroDeContratoOperacion").toString ());
		op->setSignDate (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaFirma").toVariant ().toLongLong ()).date ());
		op->setConcept (okJson.object ().value ("planDePagos").toObject ().value ("concepto").toString ());
		op->setDetail (okJson.object ().value ("planDePagos").toObject ().value ("detalle").toString ());
		op->setCurrency (OperacionesFinancieras::MapMonedaString (okJson.object ().value ("planDePagos").toObject ().value ("moneda").toString ()));
		op->setAmmount (okJson.object ().value ("planDePagos").toObject ().value ("monto").toDouble ());
		if (!okJson.object ().value ("planDePagos").toObject ().value ("iva").isNull ()) {
			op->setIVA (okJson.object ().value ("planDePagos").toObject ().value ("iva").toDouble ());
		}
		op->setRateType (OperacionesFinancieras::MapTipoTasaString (okJson.object ().value ("planDePagos").toObject ().value ("tipoDeTasa").toString ()));
		op->setStaticRate (okJson.object ().value ("planDePagos").toObject ().value ("interesFijo").toDouble ());
		if (!okJson.object ().value ("planDePagos").toObject ().value ("interesVariable").isNull ()) {
			op->setDynamicRate (okJson.object ().value ("planDePagos").toObject ().value ("interesVariable").toDouble ());
		}
		op->setTerm (okJson.object ().value ("planDePagos").toObject ().value ("plazo").toInt ());
		op->setFrequency (OperacionesFinancieras::MapFrecuenciaString (okJson.object ().value ("planDePagos").toObject ().value ("frecuenciaDePagos").toString ()));
		op->setExpirationDate (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaVencimiento").toVariant ().toLongLong ()).date ());
		op->setEnterprise (okJson.object ().value ("planDePagos").toObject ().value ("empresas_grupo").toObject ().value ("id").toInt ());
		op->setEntity (okJson.object ().value ("planDePagos").toObject ().value ("entidades_financiera").toObject ().value ("id").toInt ());
		if (!okJson.object ().value ("planDePagos").toObject ().value ("lineas_de_credito").isNull ()) {
			op->setCreditLine (okJson.object ().value ("planDePagos").toObject ().value ("lineas_de_credito").toObject ().value ("id").toInt ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("cuotaInicial").isNull ()) {
			op->setInitialDue (okJson.object ().value ("planDePagos").toObject ().value ("cuotaInicial").toDouble ());
		}

		if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_1").isNull ()) {
			op->setMontoDesem_1 (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_1").toDouble ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_1").isNull ()) {
			op->setFechaDesem_1 (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_1").toVariant ().toLongLong ()).date ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_2").isNull ()) {
			op->setMontoDesem_2 (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_2").toDouble ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_2").isNull ()) {
			op->setFechaDesem_2 (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_2").toVariant ().toLongLong ()).date ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_3").isNull ()) {
			op->setMontoDesem_3 (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_3").toDouble ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_3").isNull ()) {
			op->setFechaDesem_3 (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_3").toVariant ().toLongLong ()).date ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_4").isNull ()) {
			op->setMontoDesem_4 (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_4").toDouble ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_4").isNull ()) {
			op->setFechaDesem_4 (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_4").toVariant ().toLongLong ()).date ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_5").isNull ()) {
			op->setMontoDesem_5 (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_5").toDouble ());
		}
		if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_5").isNull ()) {
			op->setFechaDesem_5 (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_5").toVariant ().toLongLong ()).date ());
		}
	}

	reply->deleteLater ();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return op;
}

// Delete CRUD operation
void Operacion::deleteRes (QString targetURL, QString token) {

}


//===========================================================================================================================
//========================================================= Setters =========================================================
void Operacion::setContractNumber (QString contractNumber) {
	this->contractNumber = contractNumber;
}

void Operacion::setSignDate (QDate date) {
	this->signDate = date;
}

void Operacion::setConcept (QString concept) {
	this->concept = concept;
}

void Operacion::setDetail (QString detail) {
	this->detail = detail;
}

void Operacion::setCurrency (OperacionesFinancieras::Moneda currency) {
	this->currency = currency;
}

void Operacion::setAmmount (double ammount) {
	this->ammount = ammount;
}

void Operacion::setIVA (double iva) {
	this->iva = iva;
}

void Operacion::setRateType (OperacionesFinancieras::TipoTasa rateType) {
	this->rateType = rateType;
}

void Operacion::setStaticRate (double staticRate) {
	this->staticRate = staticRate;
}

void Operacion::setDynamicRate (double dynamicRate) {
	this->dynamicRate = dynamicRate;
}

void Operacion::setTerm (int term) {
	this->term = term;
}

void Operacion::setFrequency (OperacionesFinancieras::FrecuenciaDePagos freq) {
	this->frequency = freq;
}

void Operacion::setExpirationDate (QDate date) {
	this->expirationDate = date;
}

void Operacion::setEnterprise (int enterprise_ID) {
	this->enterprise = enterprise_ID;
}

void Operacion::setEntity (int entity_ID) {
	this->entity = entity_ID;
}

void Operacion::setCreditLine (int creditLine_ID) {
	this->creditLine = creditLine_ID;
}

OperacionesFinancieras::TiposDeOperacion Operacion::getOperationType () {
	return this->operationType;
}

void Operacion::setFechaDesem_1 (QDate date) {
	this->fechaDesem_1 = date;
}

void Operacion::setMontoDesem_1 (double ammount) {
	this->montoDesem_1 = ammount;
}

void Operacion::setFechaDesem_2 (QDate date) {
	this->fechaDesem_2 = date;
}

void Operacion::setMontoDesem_2 (double ammount) {
	this->montoDesem_2 = ammount;
}

void Operacion::setFechaDesem_3 (QDate date) {
	this->fechaDesem_3 = date;
}

void Operacion::setMontoDesem_3 (double ammount) {
	this->montoDesem_3 = ammount;
}

void Operacion::setFechaDesem_4 (QDate date) {
	this->fechaDesem_4 = date;
}

void Operacion::setMontoDesem_4 (double ammount) {
	this->montoDesem_4 = ammount;
}

void Operacion::setFechaDesem_5 (QDate date) {
	this->fechaDesem_5 = date;
}

void Operacion::setMontoDesem_5 (double ammount) {
	this->montoDesem_5 = ammount;
}

void Operacion::setInitialDue (double ammount) {
	this->initialDue = ammount;
}
//===========================================================================================================================
//======================================================== getters ==========================================================
int Operacion::getID () {
	return this->id;
}

QString Operacion::getContractNumber () {
	return this->contractNumber;
}

QDate Operacion::getSignDate () {
	return this->signDate;
}

QString Operacion::getConcept () {
	return this->concept;
}

QString Operacion::getDetail () {
	return this->detail;
}

OperacionesFinancieras::Moneda Operacion::getCurrency () {
	return this->currency;
}

double Operacion::getAmmount () {
	return this->ammount;
}

double Operacion::getIVA () {
	return this->iva;
}

OperacionesFinancieras::TipoTasa Operacion::getRateType () {
	return this->rateType;
}

double Operacion::getStaticRate () {
	return this->staticRate;
}

double Operacion::getDynamicRate () {
	return this->dynamicRate;
}

OperacionesFinancieras::FrecuenciaDePagos Operacion::getFrequency () {
	return this->frequency;
}

QDate Operacion::getExpirationDate () {
	return this->expirationDate;
}

int Operacion::getEnterprise () {
	return this->enterprise;
}

int Operacion::getEntity () {
	return this->entity;
}

int Operacion::getCreditLine () {
	return this->creditLine;
}

QDate Operacion::getFechaDesem_1 () {
	return this->fechaDesem_1;
}

double Operacion::getMontoDesem_1 () {
	return this->montoDesem_1;
}

QDate Operacion::getFechaDesem_2 () {
	return this->fechaDesem_2;
}

double Operacion::getMontoDesem_2 () {
	return this->montoDesem_2;
}

QDate Operacion::getFechaDesem_3 () {
	return this->fechaDesem_3;
}

double Operacion::getMontoDesem_3 () {
	return this->montoDesem_3;
}

QDate Operacion::getFechaDesem_4 () {
	return this->fechaDesem_4;
}

double Operacion::getMontoDesem_4 () {
	return this->montoDesem_4;
}

QDate Operacion::getFechaDesem_5 () {
	return this->fechaDesem_5;
}

double Operacion::getMontoDesem_5 () {
	return this->montoDesem_5;
}

double Operacion::getInitialDue () {
	return this->initialDue;
}

void Operacion::setID (int id) {
	this->id = id;
}
//===========================================================================================================================