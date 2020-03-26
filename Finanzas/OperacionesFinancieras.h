#pragma once

#include <QString>


namespace OperacionesFinancieras {
	enum class Moneda {
		Bolivianos,
		Dolares,
		NONE
	};

	enum class TipoTasa {
		Fijo,
		Variable,
		NONE
	};

	enum class FrecuenciaDePagos {
		Mensual,
		Bimensual,
		Trimestral,
		Semestral,
		Anual,
		NONE
	};

	enum class TiposDeOperacion {
		CasoCredito,
		CasoLineaDeCredito,
		CasoLeasing,
		CasoLeaseBack,
		CasoSeguro,
		CasoImpuestosNacionales,
		NONE
	};

	inline TiposDeOperacion MapOperationString (QString op);
	inline QString MapOperationEnum (TiposDeOperacion op);
	inline Moneda MapMonedaString (QString currency);
	inline QString MapMonedaEnum (Moneda currency);
	inline QString MapMonedaEnum_Short (Moneda currency);
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
	if (op == QString::fromLatin1 ("Seguro")) {
		return OperacionesFinancieras::TiposDeOperacion::CasoSeguro;
	}
	if (op == QString::fromLatin1 ("Impuestos Nacionales")) {
		return OperacionesFinancieras::TiposDeOperacion::CasoImpuestosNacionales;
	}
	return OperacionesFinancieras::TiposDeOperacion::NONE;
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
	if (op == OperacionesFinancieras::TiposDeOperacion::CasoSeguro) {
		return QString::fromLatin1 ("Seguro");
	}
	if (op == OperacionesFinancieras::TiposDeOperacion::CasoImpuestosNacionales) {
		return QString::fromLatin1 ("Impuestos Nacionales");
	}
	return QString::fromLatin1 ("-");
}

OperacionesFinancieras::Moneda OperacionesFinancieras::MapMonedaString (QString currency) {
	if (currency == QString::fromLatin1 ("Bolivianos (BOB)")) {
		return Moneda::Bolivianos;
	}
	if (currency == QString::fromLatin1 ("Dólares ($us)")) {
		return Moneda::Dolares;
	}
	return Moneda::NONE;
}

QString OperacionesFinancieras::MapMonedaEnum (OperacionesFinancieras::Moneda currency) {
	if (currency == OperacionesFinancieras::Moneda::Bolivianos) {
		return QString::fromLatin1 ("Bolivianos (BOB)");
	}
	if (currency == OperacionesFinancieras::Moneda::Dolares) {
		return QString::fromLatin1 ("Dólares ($us)");
	}
	return QString::fromLatin1 ("");
}

QString OperacionesFinancieras::MapMonedaEnum_Short (OperacionesFinancieras::Moneda currency) {
	if (currency == OperacionesFinancieras::Moneda::Bolivianos) {
		return QString::fromLatin1 ("Bs");
	}
	if (currency == OperacionesFinancieras::Moneda::Dolares) {
		return QString::fromLatin1 ("$us");
	}
	return QString::fromLatin1 ("-");
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
	if (freq == QString::fromLatin1 ("Anual")) {
		return OperacionesFinancieras::FrecuenciaDePagos::Anual;
	}
	return OperacionesFinancieras::FrecuenciaDePagos::NONE;
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
	if (freq == OperacionesFinancieras::FrecuenciaDePagos::Anual) {
		return QString::fromLatin1 ("Anual");
	}
	return QString::fromLatin1 ("-");
}

OperacionesFinancieras::TipoTasa OperacionesFinancieras::MapTipoTasaString (QString tipo) {
	if (tipo == QString::fromLatin1 ("Fijo")) {
		return OperacionesFinancieras::TipoTasa::Fijo;
	}
	if (tipo == QString::fromLatin1 ("Variable")) {
		return OperacionesFinancieras::TipoTasa::Variable;
	}
	return OperacionesFinancieras::TipoTasa::NONE;
}

QString OperacionesFinancieras::MapTipoTasaEnum (OperacionesFinancieras::TipoTasa tipo) {
	if (tipo == OperacionesFinancieras::TipoTasa::Fijo) {
		return QString::fromLatin1 ("Fijo");
	}
	if (tipo == OperacionesFinancieras::TipoTasa::Variable) {
		return QString::fromLatin1 ("Variable");
	}
	return QString::fromLatin1 ("-");
}
