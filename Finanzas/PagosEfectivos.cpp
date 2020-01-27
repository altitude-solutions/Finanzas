#include "PagosEfectivos.h"


// Json imports
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>


PagosEfectivos::PagosEfectivos(QWidget *parent): QWidget(parent) {
	ui.setupUi(this);

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

	// ==================================================================
	// Load planes de pago info
	// ==================================================================
}

PagosEfectivos::~PagosEfectivos(){

}

// get authentication data
void PagosEfectivos::setAuthToken (QString token) {
	this->token = token;
}

void PagosEfectivos::setUserName (QString userName) {
	this->userName = userName;
}

// Finish cycle
void PagosEfectivos::saveButtonClicked () {

}

// Start cycle
void PagosEfectivos::findButtonClicked () {

}

void PagosEfectivos::loadPlanesData () {

}
