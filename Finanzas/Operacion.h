#pragma once

#include <QObject>

// Qt imports
#include <QDate>
#include <QDateTime>
#include <QList>

#include <QDebug>

// Other imports
#include "OperacionesFinancieras.h"

// Json imports
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

// network imports
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

enum class OperationValidationErros {
	CONTRACT_ERROR,
	CURRENCY_ERROR,
	AMMOUNT_ERROR,
	IVA_ERROR,
	RATE_TYPE_ERROR,
	S_RATE_ERROR,
	D_RATE_ERROR,
	TERM_ERROR,
	FREQ_ERROR,
	ENTERPRISE_ERROR,
	ENTITY_ERROR,
	CREDIT_LINE_ERROR,
	DESEM_ERROR,
	INITIAL_DUE_ERROR,
	NO_ERROR,
	SERVER_SIDE_ERROR
};

class Operacion : public QObject {
	Q_OBJECT

public:
	Operacion (QObject* parent);
	~Operacion();

	bool isEntity_ImpuestosNacionales;

public:
	virtual bool validate () = 0;
	virtual void save (QString targetURL, QString token) = 0;
	static Operacion* load (int id, QString targetURL, QString token, QObject* parent = nullptr);
	virtual void update (QString targetURL, QString token) = 0;
	bool deleteRes (QString targetURL, QString token);

	// getters and setters
	void setID (int id);
	void setContractNumber (QString contractNumber);
	void setSignDate (QDate date);
	void setConcept (QString concept);
	void setDetail (QString detail);
	void setCurrency (OperacionesFinancieras::Moneda currency);
	void setAmmount (double ammount);
	void setIVA (double iva);
	void setRateType (OperacionesFinancieras::TipoTasa rateType);
	void setStaticRate (double staticRate);
	void setDynamicRate (double dynamicRate);
	void setTerm (int term);
	void setFrequency (OperacionesFinancieras::FrecuenciaDePagos freq);
	void setExpirationDate (QDate date);

	void setEnterprise (int enterprise_ID);
	void setEntity (int entity_ID);
	void setCreditLine (int creditLine_ID);

	OperacionesFinancieras::TiposDeOperacion getOperationType ();

	void setFechaDesem_1 (QDate date);
	void setMontoDesem_1 (double ammount);
	void setFechaDesem_2 (QDate date);
	void setMontoDesem_2 (double ammount);
	void setFechaDesem_3 (QDate date);
	void setMontoDesem_3 (double ammount);
	void setFechaDesem_4 (QDate date);
	void setMontoDesem_4 (double ammount);
	void setFechaDesem_5 (QDate date);
	void setMontoDesem_5 (double ammount);
	void setFechaDesem_6 (QDate date);
	void setMontoDesem_6 (double ammount);

	void setInitialDue (double ammount);

	void setWarranty (double warranty);


	int getID ();
	QString getContractNumber ();
	QDate getSignDate ();
	QString getConcept ();
	QString getDetail ();
	OperacionesFinancieras::Moneda getCurrency ();
	double getAmmount ();
	double getIVA ();
	OperacionesFinancieras::TipoTasa getRateType ();
	double getStaticRate ();
	double getDynamicRate ();
	OperacionesFinancieras::FrecuenciaDePagos getFrequency ();
	QDate getExpirationDate ();
	int getEnterprise ();
	int getEntity ();
	int getCreditLine ();
	QDate getFechaDesem_1 ();
	double getMontoDesem_1 ();
	QDate getFechaDesem_2 ();
	double getMontoDesem_2 ();
	QDate getFechaDesem_3 ();
	double getMontoDesem_3 ();
	QDate getFechaDesem_4 ();
	double getMontoDesem_4 ();
	QDate getFechaDesem_5 ();
	double getMontoDesem_5 ();
	QDate getFechaDesem_6 ();
	double getMontoDesem_6 ();

	double getInitialDue ();

	double getWarranty ();

signals:
	void operacionUpdated ();
	// TODO: que cosas no :v
	void notifyValidationStatus (OperationValidationErros status, QString errorMessage = "");

protected:
	int id;
	OperacionesFinancieras::TiposDeOperacion operationType;
	QString contractNumber;
	QDate signDate;
	QString concept;
	QString detail;
	OperacionesFinancieras::Moneda currency;
	double ammount;
	double iva;
	OperacionesFinancieras::TipoTasa rateType;
	double staticRate;
	double dynamicRate;
	int term;
	OperacionesFinancieras::FrecuenciaDePagos frequency;
	QDate expirationDate;
	// other tables
	int enterprise;
	int entity;
	int creditLine;
	// desembolsos
	QDate fechaDesem_1;
	double montoDesem_1;
	QDate fechaDesem_2;
	double montoDesem_2;
	QDate fechaDesem_3;
	double montoDesem_3;
	QDate fechaDesem_4;
	double montoDesem_4;
	QDate fechaDesem_5;
	double montoDesem_5;
	QDate fechaDesem_6;
	double montoDesem_6;
	// initial due
	double initialDue;
	// warranty, impuestos nacionales
	double warranty;
};
