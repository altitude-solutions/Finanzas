#pragma once

#include <QString>


namespace OperacionesFinancieras {
	enum class Moneda {
		Bolivianos,
		Dolares
	};

	enum class TipoTasa {
		Fijo,
		Variable
	};

	enum class FrecuenciaDePagos {
		Mensual,
		Bimensual,
		Trimestral,
		Semestral,
		Anual
	};

	enum class TiposDeOperacion {
		CasoCredito,
		CasoLineaDeCredito,
		CasoLeasing,
		CasoLeaseBack,
		CasoSeguro
	};

	inline TiposDeOperacion MapOperationString (QString op);
	inline QString MapOperationEnum (TiposDeOperacion op);
	inline Moneda MapMonedaString (QString currency);
	inline QString MapMonedaEnum (Moneda currency);
	inline FrecuenciaDePagos MapFrecuenciaString (QString freq);
	inline QString MapFrecuenciaEnum (FrecuenciaDePagos freq);
	inline TipoTasa MapTipoTasaString (QString tipo);
	inline QString MapTipoTasaEnum (TipoTasa tipo);
}


OperacionesFinancieras::TiposDeOperacion OperacionesFinancieras::MapOperationString (QString op) {
	if (op == QString::fromLatin1 ("Crédito")) {
		return OperacionesFinancieras::TiposDeOperacion::CasoCredito;
	}
	if (op == QString::fromLatin1 ("Operación de Línea de Crédito")) {
		return OperacionesFinancieras::TiposDeOperacion::CasoLineaDeCredito;
	}
	if (op == QString::fromLatin1 ("Leasing")) {
		return OperacionesFinancieras::TiposDeOperacion::CasoLeasing;
	}
	if (op == QString::fromLatin1 ("Lease Back")) {
		return OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack;
	}
	return OperacionesFinancieras::TiposDeOperacion::CasoSeguro;
}
QString OperacionesFinancieras::MapOperationEnum (TiposDeOperacion op) {
	if (op == OperacionesFinancieras::TiposDeOperacion::CasoCredito) {
		return QString::fromLatin1 ("Crédito");
	}
	if (op == OperacionesFinancieras::TiposDeOperacion::CasoLineaDeCredito) {
		return QString::fromLatin1 ("Operación de Línea de Crédito");
	}
	if (op == OperacionesFinancieras::TiposDeOperacion::CasoLeasing) {
		return QString::fromLatin1 ("Leasing");
	}
	if (op == OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack) {
		return QString::fromLatin1 ("Lease Back");
	}
	return QString::fromLatin1 ("Seguro");
}
OperacionesFinancieras::Moneda OperacionesFinancieras::MapMonedaString (QString currency) {
	if (currency == QString::fromLatin1 ("Bolivianos (BOB)")) {
		return Moneda::Bolivianos;
	}
	return Moneda::Dolares;
}
QString OperacionesFinancieras::MapMonedaEnum (OperacionesFinancieras::Moneda currency) {
	if (currency == OperacionesFinancieras::Moneda::Bolivianos) {
		return QString::fromLatin1 ("Bolivianos (BOB)");
	}
	return QString::fromLatin1 ("Dólares ($us)");
}
OperacionesFinancieras::FrecuenciaDePagos OperacionesFinancieras::MapFrecuenciaString (QString freq) {
	if (freq == QString::fromLatin1 ("Mensual")) {
		return OperacionesFinancieras::FrecuenciaDePagos::Mensual;
	}
	if (freq == QString::fromLatin1 ("Bimensual")) {
		return OperacionesFinancieras::FrecuenciaDePagos::Bimensual;
	}
	if (freq == QString::fromLatin1 ("Trimestral")) {
		return OperacionesFinancieras::FrecuenciaDePagos::Trimestral;
	}
	if (freq == QString::fromLatin1 ("Semestral")) {
		return OperacionesFinancieras::FrecuenciaDePagos::Semestral;
	}
	return OperacionesFinancieras::FrecuenciaDePagos::Anual;
}
QString OperacionesFinancieras::MapFrecuenciaEnum (OperacionesFinancieras::FrecuenciaDePagos freq) {
	if (freq == OperacionesFinancieras::FrecuenciaDePagos::Mensual) {
		return QString::fromLatin1 ("Mensual");
	}
	if (freq == OperacionesFinancieras::FrecuenciaDePagos::Bimensual) {
		return QString::fromLatin1 ("Bimensual");
	}
	if (freq == OperacionesFinancieras::FrecuenciaDePagos::Trimestral) {
		return QString::fromLatin1 ("Trimestral");
	}
	if (freq == OperacionesFinancieras::FrecuenciaDePagos::Semestral) {
		return QString::fromLatin1 ("Semestral");
	}
	return QString::fromLatin1 ("Anual");
}
OperacionesFinancieras::TipoTasa OperacionesFinancieras::MapTipoTasaString (QString tipo) {
	if (tipo == QString::fromLatin1 ("Fijo")) {
		return OperacionesFinancieras::TipoTasa::Fijo;
	}
	return OperacionesFinancieras::TipoTasa::Variable;
}
QString OperacionesFinancieras::MapTipoTasaEnum (OperacionesFinancieras::TipoTasa tipo) {
	if (tipo == OperacionesFinancieras::TipoTasa::Fijo) {
		return QString::fromLatin1 ("Fijo");
	}
	return QString::fromLatin1 ("Variable");
}
