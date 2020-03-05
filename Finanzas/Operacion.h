#pragma once

#include <QObject>

// Qt imports
#include <QDate>
#include <QList>

// Other imports
#include "OperacionesFinancieras.h"
#include "CuotasPlanDePagos.h"
#include "CuotasEfectivas.h"

// Json imports
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

// network imports
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>


class Operacion : public QObject {
	Q_OBJECT

public:
	Operacion (QObject* parent);
	~Operacion();

public:
	virtual bool validate () = 0;
	virtual void save () = 0;
	virtual void load (int id) = 0;
	virtual void update () = 0;
	static void deleteRes (int id);

	// getters and setters
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
	void setLifetime (int lifetime);
	void setFrequency (OperacionesFinancieras::FrecuenciaDePagos freq);
	void setExpirationDate (QDate date);

	void setEnterprise (int enterprise_ID);
	void setEntity (int entity_ID);

signals:
	void operacionUpdated ();
	void operationSaved ();

private:
	int id;
	OperacionesFinancieras::TiposDeOperacion tipoOperacion;
	QString numeroContrato;
	QDate fechaFirma;
	QString concepto;
	QString detalle;
	OperacionesFinancieras::Moneda currency;
	double monto;
	double iva;
	OperacionesFinancieras::TipoTasa rateType;
	double tasaFija;
	double tasaVariable;
	int plazo;
	OperacionesFinancieras::FrecuenciaDePagos frecuencia;
	QDate fechaVencimiento;
	// cuotas
	QList<CuotasPlanDePagos> cuotasPlan;
	QList<CuotasEfectivas> cuotasEfectivas;
	// other tables
	int empresaGrupo;
	int entidadFinanciera;
};
