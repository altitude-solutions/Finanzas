#include "CuotasEfectivas.h"

CuotasEfectivas::CuotasEfectivas(QObject *parent): QObject(parent) {
	id = 0;
	numeroCuota = 0;
	fechaPago = QDate::currentDate ();
	montoTotal = 0;
	pagoCapital = 0;
	pagoInteres = 0;
	pagoIva = 0;
}

CuotasEfectivas::~CuotasEfectivas() {

}
