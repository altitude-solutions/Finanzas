#include "Operacion.h"
#include "Operacion.h"

Operacion::Operacion(QObject *parent): QObject(parent) {
	id = 0;
	tipoOperacion = "";
	numeroContrato = "";
	fechaFirma = QDate::currentDate();
	concepto = "";
	detalle = "";
	moneda = OperacionesFinancieras::Moneda::Bolivianos;
	monto = 0;
	iva = 0;
	tipoTasa = OperacionesFinancieras::TipoTasa::Fijo;
	tasaFija = 0;
	tasaVariable = 0;
	plazo = 0;
	frecuencia = OperacionesFinancieras::FrecuenciaDePagos::Mensual;
	fechaVencimiento = QDate::currentDate ();
	// other tables
	empresaGrupo = 0;
	entidadFinanciera = 0;
}

Operacion::~Operacion() {

}

void Operacion::load (int id) {

}

void Operacion::deleteRes (int id) {

}

