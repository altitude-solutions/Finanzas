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
	static void load (int id);
	virtual void load (QJsonDocument object) = 0;
	virtual void update (QJsonDocument object) = 0;
	static void deleteRes (int id);

signals:
	void operacionUpdated ();

private:
	int id;
	QString tipoOperacion;
	QString numeroContrato;
	QDate fechaFirma;
	QString concepto;
	QString detalle;
	OperacionesFinancieras::Moneda moneda;
	double monto;
	double iva;
	OperacionesFinancieras::TipoTasa tipoTasa;
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
