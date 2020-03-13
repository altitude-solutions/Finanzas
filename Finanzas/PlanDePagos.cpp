#include "PlanDePagos.h"
// Other imports
#include <QMessageBox>

// Debug imports
#include <QDebug>

// Network imports
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

// completer imports
#include <QCompleter>

// custom widgets
#include "NumberInput.h"


// clipboard to excel
#include <QClipboard>
#include <QApplication>
#include <QAction>
#include <QKeySequence>


PlanDePagos::PlanDePagos (QWidget* parent) : QWidget (parent) {
	ui.setupUi (this);
	// initialize saldos
	this->saldoCapital = this->creditoFiscal = this->saldoCapitalReal = 0;
	this->currentOperation = nullptr;

	// set table headers
	setTableHeaders ();

	// set connections
	setupConnections ();
	setupUiConnections ();

	loadedFromLeftlist = false;
	editingPlan = false;

	setupTableClipboard ();
}

PlanDePagos::~PlanDePagos () {
	delete currentOperation;
}

void PlanDePagos::currencySelected (QString currency) {
	OperacionesFinancieras::Moneda selectedCurrency = OperacionesFinancieras::MapMonedaString (currency);
	ui.label_7->setText ("Monto (" + OperacionesFinancieras::MapMonedaEnum_Short (selectedCurrency) + "):");
	ui.label_15->setText ("IVA (" + OperacionesFinancieras::MapMonedaEnum_Short (selectedCurrency) + "):");
	ui.label_19->setText ("Cuota Inicial (" + OperacionesFinancieras::MapMonedaEnum_Short (selectedCurrency) + "):");
	ui.label_26->setText ("Monto Desembolso 1 (" + OperacionesFinancieras::MapMonedaEnum_Short (selectedCurrency) + "):");
	ui.label_27->setText ("Monto Desembolso 2 (" + OperacionesFinancieras::MapMonedaEnum_Short (selectedCurrency) + "):");
	ui.label_28->setText ("Monto Desembolso 3 (" + OperacionesFinancieras::MapMonedaEnum_Short (selectedCurrency) + "):");
	ui.label_29->setText ("Monto Desembolso 4 (" + OperacionesFinancieras::MapMonedaEnum_Short (selectedCurrency) + "):");
	ui.label_30->setText ("Monto Desembolso 5 (" + OperacionesFinancieras::MapMonedaEnum_Short (selectedCurrency) + "):");
}

void PlanDePagos::rateTypeSelected (QString rateType) {
	OperacionesFinancieras::TipoTasa selectedRateType = OperacionesFinancieras::MapTipoTasaString (rateType);
	if (editingPlan || !loadedFromLeftlist) {
		switch (selectedRateType) {
		case OperacionesFinancieras::TipoTasa::Fijo:
			ui.interesFijo->setEnabled (true);
			ui.interesVariable->setEnabled (false);
			ui.interesVariable->setText ("");
			break;
		case OperacionesFinancieras::TipoTasa::Variable:
			ui.interesFijo->setEnabled (true);
			ui.interesVariable->setEnabled (true);
			break;
		case OperacionesFinancieras::TipoTasa::NONE:
			ui.interesFijo->setEnabled (false);
			ui.interesVariable->setEnabled (false);
			ui.interesFijo->setText ("");
			ui.interesVariable->setText ("");
			break;
		default:
			break;
		}
	}
}

//==================================================================
//========= Enable desembolso only when previous has data ==========
void PlanDePagos::desem_1_changed (QString desem1) {
	if (desem1 == "") {
		ui.fechaDesem_2->setEnabled (false);
		ui.montoDesem_2->setEnabled (false);
		ui.fechaDesem_2->setDate (QDate::currentDate ());
		ui.montoDesem_2->setText ("");
	}
	else {
		ui.fechaDesem_2->setEnabled (true);
		ui.montoDesem_2->setEnabled (true);
	}
}

void PlanDePagos::desem_2_changed (QString desem2) {
	if (desem2 == "") {
		ui.fechaDesem_3->setEnabled (false);
		ui.montoDesem_3->setEnabled (false);
		ui.fechaDesem_3->setDate (QDate::currentDate ());
		ui.montoDesem_3->setText ("");
	}
	else {
		ui.fechaDesem_3->setEnabled (true);
		ui.montoDesem_3->setEnabled (true);
	}
}

void PlanDePagos::desem_3_changed (QString desem3) {
	if (desem3 == "") {
		ui.fechaDesem_4->setEnabled (false);
		ui.montoDesem_4->setEnabled (false);
		ui.fechaDesem_4->setDate (QDate::currentDate ());
		ui.montoDesem_4->setText ("");
	}
	else {
		ui.fechaDesem_4->setEnabled (true);
		ui.montoDesem_4->setEnabled (true);
	}
}

void PlanDePagos::desem_4_changed (QString desem4) {
	if (desem4 == "") {
		ui.fechaDesem_5->setEnabled (false);
		ui.montoDesem_5->setEnabled (false);
		ui.fechaDesem_5->setDate (QDate::currentDate ());
		ui.montoDesem_5->setText ("");
	}
	else {
		ui.fechaDesem_5->setEnabled (true);
		ui.montoDesem_5->setEnabled (true);
	}
}

//==================================================================

void PlanDePagos::termChanged (int value) {
	ui.fechaVencimiento->setDate (ui.fechaFirma->date ().addMonths (value));
}

void PlanDePagos::expirationDateChanged (QDate date) {
	QDate auxDate = ui.fechaFirma->date ();
	int diff = 0;
	while (auxDate < date) {
		auxDate = auxDate.addMonths (1);
		diff++;
	}
	ui.plazo->setValue (diff);
}

void PlanDePagos::signDateChanged (QDate date) {
	ui.fechaVencimiento->setMinimumDate (date);
	ui.fechaVencimiento->setDate (date.addMonths (ui.plazo->value ()));

	ui.fechaDesem_1->setMinimumDate (date);
	ui.fechaDesem_2->setMinimumDate (date);
	ui.fechaDesem_3->setMinimumDate (date);
	ui.fechaDesem_4->setMinimumDate (date);
	ui.fechaDesem_5->setMinimumDate (date);

	ui.fechaDesem_1->setDate (date);
	ui.fechaDesem_2->setDate (date);
	ui.fechaDesem_3->setDate (date);
	ui.fechaDesem_4->setDate (date);
	ui.fechaDesem_5->setDate (date);
}

//==================================================================
//==================== catch validation errors =====================
void PlanDePagos::catchErrors (OperationValidationErros error, QString errorMessage) {
	ui.savePlan->setEnabled (true);
	switch (error) {
	case OperationValidationErros::CONTRACT_ERROR:
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El número del contrato u operación es necesario"));
		ui.numeroContrato->setFocus ();
		break;
	case OperationValidationErros::CURRENCY_ERROR:
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La moneda es necesaria"));
		ui.moneda->setFocus ();
		break;
	case OperationValidationErros::AMMOUNT_ERROR:
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El mondo de la operación debe ser mayor que cero"));
		ui.monto->setFocus ();
		break;
	case OperationValidationErros::IVA_ERROR:
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El IVA de la operación debe ser mayor que cero"));
		ui.monto->setFocus ();
		break;
	case OperationValidationErros::RATE_TYPE_ERROR:
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El tipo de interés es necesario"));
		ui.tipoTasa->setFocus ();
		break;
	case OperationValidationErros::S_RATE_ERROR:
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El interés fijo debe ser mayor que cero"));
		ui.interesFijo->setFocus ();
		break;
	case OperationValidationErros::D_RATE_ERROR:
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El interés variable debe ser mayor que cero"));
		ui.interesVariable->setFocus ();
		break;
	case OperationValidationErros::TERM_ERROR:
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El plazo debe ser mayor o igual a un mes"));
		ui.plazo->setFocus ();
		break;
	case OperationValidationErros::FREQ_ERROR:
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La frecuencia de pagos es necesaria"));
		ui.frecuencia->setFocus ();
		break;
	case OperationValidationErros::ENTERPRISE_ERROR:
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La empresa es necesaria"));
		ui.empresa->setFocus ();
		break;
	case OperationValidationErros::ENTITY_ERROR:
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La entidad financiera es necesaria"));
		ui.entidad->setFocus ();
		break;
	case OperationValidationErros::DESEM_ERROR:
		QMessageBox::critical (this, "Error", errorMessage);
		ui.montoDesem_1->setFocus ();
		break;
	case OperationValidationErros::INITIAL_DUE_ERROR:
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La cuota inicial debe ser mayor que cero y menor que el monto de la operación"));
		ui.cuotaInicial->setFocus ();
		break;
	case OperationValidationErros::SERVER_SIDE_ERROR:
		QMessageBox::critical (this, QString::fromLatin1 ("Error del servidor"), errorMessage);
		ui.tipoOperacion->setFocus ();
		break;
	case OperationValidationErros::CREDIT_LINE_ERROR:
		QMessageBox::critical (this, QString::fromLatin1 ("Error"), errorMessage);
		ui.lineaDeCredito->setFocus ();
		break;
	case OperationValidationErros::NO_ERROR:
		QMessageBox::information (this, QString::fromLatin1 ("Éxito"), QString::fromLatin1 ("Guardado con éxito"));
		enableDueButtons ();
		reloadSelectedPlan ();
		loadPlanesDePago ();
		break;
	}
}

//==================================================================

void PlanDePagos::loadClickedPlan (QListWidgetItem* itm) {
	delete currentOperation;
	currentOperation = Operacion::load (listaOperaciones[itm->text ()].toInt (), this->targetAddress, this->token, this);
	reloadSelectedPlan ();
	loadedFromLeftlist = true;
}

//==================================================================
//========================== Data loaders ==========================
void PlanDePagos::loadEmpresasGrupo () {
	listaEmpresas.clear ();
	ui.empresa->clear ();
	QNetworkAccessManager* nam = new QNetworkAccessManager (this);
	connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
		QByteArray resBin = reply->readAll ();
		if (reply->error ()) {
			QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
			QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
			return;
		}
		QJsonDocument okJson = QJsonDocument::fromJson (resBin);
		foreach (QJsonValue entidad, okJson.object ().value ("empresas").toArray ()) {
			ui.empresa->addItem (entidad.toObject ().value ("empresa").toString ());
			listaEmpresas.insert (entidad.toObject ().value ("empresa").toString (), QString::number (entidad.toObject ().value ("id").toInt ()));
		}
		resetFields ();
		reply->deleteLater ();
		});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/empresas?status=1"));

	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}

void PlanDePagos::loadEntidadesFinancieras () {
	listaEntidades.clear ();
	ui.entidad->clear ();
	QNetworkAccessManager* nam = new QNetworkAccessManager (this);
	connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
		QByteArray resBin = reply->readAll ();
		if (reply->error ()) {
			QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
			QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
			return;
		}
		QJsonDocument okJson = QJsonDocument::fromJson (resBin);
		foreach (QJsonValue entidad, okJson.object ().value ("entidades").toArray ()) {
			ui.entidad->addItem (entidad.toObject ().value ("nombreEntidad").toString ());
			QHash<QString, QString> current;
			current.insert ("id", QString::number (entidad.toObject ().value ("id").toInt ()));
			current.insert ("nombreEntidad", entidad.toObject ().value ("nombreEntidad").toString ());
			current.insert ("tipoDeEntidad", QString::number (entidad.toObject ().value ("tipos_de_entidad").toObject ().value ("id").toInt ()));
			listaEntidades.insert (entidad.toObject ().value ("nombreEntidad").toString (), current);
		}
		resetFields ();
		reply->deleteLater ();
		});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/entidadFinanciera?status=1"));

	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}

void PlanDePagos::loadTiposDeEntidad () {
	listaTiposEntidades.clear ();
	QNetworkAccessManager* nam = new QNetworkAccessManager (this);
	connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
		QByteArray resBin = reply->readAll ();
		if (reply->error ()) {
			QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
			QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
			return;
		}
		QJsonDocument okJson = QJsonDocument::fromJson (resBin);
		foreach (QJsonValue entidad, okJson.object ().value ("tiposDeEntidad").toArray ()) {
			listaTiposEntidades.insert (QString::number (entidad.toObject ().value ("id").toInt ()), entidad.toObject ().value ("tipoDeEntidad").toString ());
		}
		resetFields ();
		reply->deleteLater ();
		});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/tipo_entidad?status=1"));

	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}

void PlanDePagos::loadLineasDeCredito (int entidad_id, int empresa_ID) {
	lineasDeCredito.clear ();
	ui.lineaDeCredito->clear ();
	if (entidad_id != 0 && empresa_ID != 0) {
		QNetworkAccessManager* nam = new QNetworkAccessManager (this);
		connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
			QByteArray resBin = reply->readAll ();
			if (reply->error ()) {
				QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
				QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
				return;
			}
			QJsonDocument okJson = QJsonDocument::fromJson (resBin);

			foreach (QJsonValue linea, okJson.object ().value ("lineasDeCredito").toArray ()) {
				ui.lineaDeCredito->addItem (linea.toObject ().value ("codigo").toString ());
				QHash <QString, QString> current;
				current.insert ("id", QString::number (linea.toObject ().value ("id").toInt ()));
				current.insert ("monto", QString::number (linea.toObject ().value ("monto").toDouble (), 'f', 2));
				current.insert ("empresa", linea.toObject ().value ("empresas_grupo").toObject ().value ("empresa").toString ());
				current.insert ("entidad", linea.toObject ().value ("entidades_financiera").toObject ().value ("nombreEntidad").toString ());
				current.insert ("moneda", linea.toObject ().value ("moneda").toString ());
				lineasDeCredito.insert (linea.toObject ().value ("codigo").toString (), current);
			}

			ui.lineaDeCredito->setCurrentIndex (-1);
			reply->deleteLater ();
			});
		QNetworkRequest request;
		request.setUrl (QUrl (targetAddress + "/lineaDeCredito?status=1&entidad=" + QString::number (entidad_id) + "&empresa=" + QString::number (empresa_ID)));
		request.setRawHeader ("token", this->token.toUtf8 ());
		request.setRawHeader ("Content-Type", "application/json");
		nam->get (request);
	}
}

void PlanDePagos::loadPlanesDePago (QString query) {
	listaOperaciones.clear ();
	ui.planesList->clear ();

	QNetworkAccessManager* nam = new QNetworkAccessManager (this);
	connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
		QByteArray resBin = reply->readAll ();
		if (reply->error ()) {
			QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
			QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
			return;
		}
		QJsonDocument okJson = QJsonDocument::fromJson (resBin);

		foreach (QJsonValue entidad, okJson.object ().value ("planesDePago").toArray ()) {
			listaOperaciones.insert (entidad.toObject ().value ("numeroDeContratoOperacion").toString (), QString::number (entidad.toObject ().value ("id").toInt ()));
			ui.planesList->addItem (entidad.toObject ().value ("numeroDeContratoOperacion").toString ());
		}
		reply->deleteLater ();
		});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/planDePagos?status=1" + query));

	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}

void PlanDePagos::reloadSelectedPlan () {
	if (currentOperation != nullptr) {
		QNetworkAccessManager* nam = new QNetworkAccessManager (this);
		connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
			QByteArray resBin = reply->readAll ();
			if (reply->error ()) {
				QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
				QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
				return;
			}
			QJsonDocument okJson = QJsonDocument::fromJson (resBin);
			ui.empresa->setCurrentText (okJson.object ().value ("planDePagos").toObject ().value ("empresas_grupo").toObject ().value ("empresa").toString ());
			ui.entidad->setCurrentText (okJson.object ().value ("planDePagos").toObject ().value ("entidades_financiera").toObject ().value ("nombreEntidad").toString ());
			ui.tipoEntidad->setText (okJson.object ().value ("planDePagos").toObject ().value ("entidades_financiera").toObject ().value ("tipos_de_entidad").toObject ().value ("tipoDeEntidad").toString ());
			ui.tipoOperacion->setCurrentText (okJson.object ().value ("planDePagos").toObject ().value ("tipoOperacion").toString ());
			ui.numeroContrato->setText (okJson.object ().value ("planDePagos").toObject ().value ("numeroDeContratoOperacion").toString ());
			ui.fechaFirma->setDate (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaFirma").toVariant ().toLongLong ()).date ());
			ui.moneda->setCurrentText (okJson.object ().value ("planDePagos").toObject ().value ("moneda").toString ());
			ui.monto->setText (QString::number (okJson.object ().value ("planDePagos").toObject ().value ("monto").toDouble (), 'f', 2));
			if (!okJson.object ().value ("planDePagos").toObject ().value ("iva").isNull ()) {
				ui.iva->setText (QString::number (okJson.object ().value ("planDePagos").toObject ().value ("iva").toDouble (), 'f', 2));
			}
			else {
				ui.iva->setText ("-");
			}
			if (!okJson.object ().value ("planDePagos").toObject ().value ("cuotaInicial").isNull ()) {
				ui.cuotaInicial->setText (QString::number (okJson.object ().value ("planDePagos").toObject ().value ("cuotaInicial").toDouble (), 'f', 2));
			}
			else {
				ui.cuotaInicial->setText ("-");
			}
			ui.tipoTasa->setCurrentText (okJson.object ().value ("planDePagos").toObject ().value ("tipoDeTasa").toString ());
			ui.interesFijo->setValue (okJson.object ().value ("planDePagos").toObject ().value ("interesFijo").toDouble ());

			if (!okJson.object ().value ("planDePagos").toObject ().value ("interesVariable").isNull ()) {
				ui.interesVariable->setValue (okJson.object ().value ("planDePagos").toObject ().value ("interesVariable").toDouble ());
			}
			else {
				ui.interesVariable->setText ("-");
			}
			ui.frecuencia->setCurrentText (okJson.object ().value ("planDePagos").toObject ().value ("frecuenciaDePagos").toString ());
			ui.plazo->setValue (okJson.object ().value ("planDePagos").toObject ().value ("plazo").toInt ());
			ui.fechaVencimiento->setDate (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaVencimiento").toVariant ().toLongLong ()).date ());

			ui.concepto->setText (okJson.object ().value ("planDePagos").toObject ().value ("concepto").toString ());
			ui.detalle->setText (okJson.object ().value ("planDePagos").toObject ().value ("detalle").toString ());

			if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_1").isNull ()) {
				ui.montoDesem_1->setValue (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_1").toDouble ());
			}
			if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_2").isNull ()) {
				ui.montoDesem_2->setValue (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_2").toDouble ());
			}
			if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_3").isNull ()) {
				ui.montoDesem_3->setValue (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_3").toDouble ());
			}
			if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_4").isNull ()) {
				ui.montoDesem_4->setValue (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_4").toDouble ());
			}
			if (!okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_5").isNull ()) {
				ui.montoDesem_5->setValue (okJson.object ().value ("planDePagos").toObject ().value ("montoDesembolso_5").toDouble ());
			}

			if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_1").isNull ()) {
				ui.fechaDesem_1->setDate (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_1").toVariant ().toLongLong ()).date ());
			}
			if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_2").isNull ()) {
				ui.fechaDesem_2->setDate (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_2").toVariant ().toLongLong ()).date ());
			}
			if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_3").isNull ()) {
				ui.fechaDesem_3->setDate (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_3").toVariant ().toLongLong ()).date ());
			}
			if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_4").isNull ()) {
				ui.fechaDesem_4->setDate (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_4").toVariant ().toLongLong ()).date ());
			}
			if (!okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_5").isNull ()) {
				ui.fechaDesem_5->setDate (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaDesembolso_5").toVariant ().toLongLong ()).date ());
			}

			ui.plannedFee->setRowCount (0);

			double saldoCapital = okJson.object ().value ("planDePagos").toObject ().value ("monto").toDouble ();
			double saldoIva = okJson.object ().value ("planDePagos").toObject ().value ("iva").toDouble ();
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			foreach (QJsonValue cuota, okJson.object ().value ("planDePagos").toObject ().value ("cuotasPlan").toArray ()) {
				ui.plannedFee->insertRow (ui.plannedFee->rowCount ());

				ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 0, new QTableWidgetItem (QString::number (cuota.toObject ().value ("numeroDeCuota").toInt ())));
				ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 1, new QTableWidgetItem (QString::number (QDateTime::fromMSecsSinceEpoch (cuota.toObject ().value ("fechaDePago").toVariant ().toLongLong ()).date ().year ())));
				ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 2, new QTableWidgetItem (QDate::shortMonthName (QDateTime::fromMSecsSinceEpoch (cuota.toObject ().value ("fechaDePago").toVariant ().toLongLong ()).date ().month ()) + "/" + QString::number (QDateTime::fromMSecsSinceEpoch (cuota.toObject ().value ("fechaDePago").toVariant ().toLongLong ()).date ().year ())));
				ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 3, new QTableWidgetItem (QDateTime::fromMSecsSinceEpoch (cuota.toObject ().value ("fechaDePago").toVariant ().toLongLong ()).toString ("dd/MM/yyyy")));
				ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 4, new QTableWidgetItem (QString::number (cuota.toObject ().value ("montoTotalDelPago").toDouble (), 'f', 2)));
				ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 5, new QTableWidgetItem (QString::number (cuota.toObject ().value ("pagoDeCapital").toDouble (), 'f', 2)));
				ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 6, new QTableWidgetItem (QString::number (cuota.toObject ().value ("pagoDeInteres").toDouble (), 'f', 2)));
				if (!cuota.toObject ().value ("pagoDeIva").isNull ()) {
					ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 7, new QTableWidgetItem (QString::number (cuota.toObject ().value ("pagoDeIva").toDouble (), 'f', 2)));
				}
				else {
					ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 7, new QTableWidgetItem ("0.00"));
				}

				saldoCapital -= cuota.toObject ().value ("pagoDeCapital").toDouble ();
				saldoIva -= cuota.toObject ().value ("pagoDeIva").toDouble ();

				//================================ primera cuota caso Leasing =======================================
				OperacionesFinancieras::TiposDeOperacion opera = OperacionesFinancieras::MapOperationString (okJson.object ().value ("planDePagos").toObject ().value ("tipoOperacion").toString ());
				if ((cuota.toObject ().value ("numeroDeCuota").toInt () == 1 && opera == OperacionesFinancieras::TiposDeOperacion::CasoLeasing) || (cuota.toObject ().value ("numeroDeCuota").toInt () == 0 && opera == OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack)) {
					saldoCapital -= okJson.object ().value ("planDePagos").toObject ().value ("iva").toDouble ();
				}

				ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 8, new QTableWidgetItem (QString::number (saldoCapital, 'f', 2)));
				ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 9, new QTableWidgetItem (QString::number (saldoIva, 'f', 2)));
				ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 10, new QTableWidgetItem (QString::number (saldoCapital + saldoIva, 'f', 2)));

				ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 11, new QTableWidgetItem (QString::number (cuota.toObject ().value ("id").toInt ())));
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			enableDueButtons ();
			reply->deleteLater ();
			});
		QNetworkRequest request;
		request.setUrl (QUrl (targetAddress + "/planDePagos/" + QString::number (this->currentOperation->getID ())));

		request.setRawHeader ("token", this->token.toUtf8 ());
		request.setRawHeader ("Content-Type", "application/json");
		nam->get (request);
	}
}
//==================================================================

void PlanDePagos::setTableHeaders () {
	ui.plannedFee->setColumnCount (12);
	// set columns names
	QStringList headers;
	headers << QString::fromLatin1 ("Cuota") << QString::fromLatin1 ("Año") << QString::fromLatin1 ("Mes") << QString::fromLatin1 ("Fecha de Pago") << QString::fromLatin1 ("Pago Total")
		<< QString::fromLatin1 ("Pago de Capital") << QString::fromLatin1 ("Pago de Interes") << QString::fromLatin1 ("IVA") << QString::fromLatin1 ("Saldo del Capital")
		<< QString::fromLatin1 ("Crédito Fiscal") << QString::fromLatin1 ("Saldo Capital Real") << QString::fromLatin1 ("id");
	ui.plannedFee->setHorizontalHeaderLabels (headers);

	// Get table widget size
	QSize tableSize = ui.plannedFee->size ();

	double totalWidth = tableSize.width ();
	// Set column width to a proper size
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (0, QHeaderView::ResizeMode::ResizeToContents);	// cuota
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (1, QHeaderView::ResizeMode::ResizeToContents);	// año
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (2, QHeaderView::ResizeMode::Stretch);				// mes
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (3, QHeaderView::ResizeMode::Stretch);				// fecha de pago
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (4, QHeaderView::ResizeMode::Stretch);				// pago total
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (5, QHeaderView::ResizeMode::Stretch);				// pago capital
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (6, QHeaderView::ResizeMode::Stretch);				// pago interes
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (7, QHeaderView::ResizeMode::Stretch);				// pago iva
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (8, QHeaderView::ResizeMode::Stretch);				// saldo capital
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (9, QHeaderView::ResizeMode::Stretch);				// credito fiscal
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (10, QHeaderView::ResizeMode::Stretch);			// saldo capital real
	ui.plannedFee->hideColumn (11);																				// id


	// hide vertical headers
	ui.plannedFee->verticalHeader ()->hide ();
}

// On tab selected, reload data to refresh tab
void PlanDePagos::onTabSelected () {
	// Tab setup
	loadEmpresasGrupo ();
	loadEntidadesFinancieras ();
	loadTiposDeEntidad ();
	loadPlanesDePago ();
}

// Set auth data
void PlanDePagos::setAuthData (QString address, QString token, QString userName) {
	this->token = token;
	this->userName = userName;
	this->targetAddress = address;

	loadEmpresasGrupo ();
	loadEntidadesFinancieras ();
	loadTiposDeEntidad ();
	loadPlanesDePago ();
}

void PlanDePagos::operationTypeSelected (QString operation) {
	if (!loadedFromLeftlist) {
		// delete current operation, then asign a new one
		delete currentOperation;
	}
		if (operation != "") {
			OperacionesFinancieras::TiposDeOperacion caso = OperacionesFinancieras::MapOperationString (QString::fromLatin1 (operation.toLatin1 ()));

			if (!loadedFromLeftlist) {
				switch (caso) {
				case OperacionesFinancieras::TiposDeOperacion::CasoCredito:
					currentOperation = new OperacionCredito (this);
					break;
				case OperacionesFinancieras::TiposDeOperacion::CasoLineaDeCredito:
					currentOperation = new OperacionLineaDeCredito (this);
					break;
				case OperacionesFinancieras::TiposDeOperacion::CasoLeasing:
					currentOperation = new OperacionLeasing (this);
					break;
				case OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack:
					currentOperation = new OperacionLeaseBack (this);
					break;
				case OperacionesFinancieras::TiposDeOperacion::CasoSeguro:
					currentOperation = nullptr;
					break;
				case OperacionesFinancieras::TiposDeOperacion::NONE:
					currentOperation = nullptr;
					break;
				}
			}

			// catch validation errors
			if (currentOperation != nullptr) {
				connect (currentOperation, &Operacion::notifyValidationStatus, this, &PlanDePagos::catchErrors);
			}
			// enable and disable desembolsos on caso
			switch (caso) {
			case OperacionesFinancieras::TiposDeOperacion::CasoCredito:
				ui.fechaDesem_1->setEnabled (true);
				ui.montoDesem_1->setEnabled (true);
				ui.iva->setText ("");
				ui.lineaDeCredito->setEnabled (false);
				ui.lineaDeCredito->setCurrentIndex (-1);
				ui.moneda->setEnabled (true);
				ui.cuotaInicial->setEnabled (false);
				break;
			case OperacionesFinancieras::TiposDeOperacion::CasoLineaDeCredito:
				ui.fechaDesem_1->setEnabled (true);
				ui.montoDesem_1->setEnabled (true);
				ui.iva->setText ("");
				ui.lineaDeCredito->setEnabled (true);
				ui.lineaDeCredito->setCurrentIndex (-1);
				ui.moneda->setEnabled (false);
				ui.cuotaInicial->setEnabled (false);
				break;
			case OperacionesFinancieras::TiposDeOperacion::CasoLeasing:
				ui.fechaDesem_1->setEnabled (false);
				ui.montoDesem_1->setEnabled (false);
				ui.fechaDesem_1->setDate (QDate::currentDate ());
				ui.montoDesem_1->setText ("");
				ui.lineaDeCredito->setEnabled (false);
				ui.lineaDeCredito->setCurrentIndex (-1);
				ui.moneda->setEnabled (true);
				ui.cuotaInicial->setEnabled (true);
				break;
			case OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack:
				ui.fechaDesem_1->setEnabled (true);
				ui.montoDesem_1->setEnabled (true);
				ui.lineaDeCredito->setEnabled (false);
				ui.lineaDeCredito->setCurrentIndex (-1);
				ui.moneda->setEnabled (true);
				ui.cuotaInicial->setEnabled (true);
				break;
			case OperacionesFinancieras::TiposDeOperacion::CasoSeguro:
				ui.fechaDesem_1->setEnabled (true);
				ui.montoDesem_1->setEnabled (true);
				ui.iva->setText ("");
				ui.lineaDeCredito->setEnabled (false);
				ui.lineaDeCredito->setCurrentIndex (-1);
				ui.moneda->setEnabled (true);
				ui.cuotaInicial->setEnabled (false);
				break;
			case OperacionesFinancieras::TiposDeOperacion::NONE:
				ui.fechaDesem_1->setEnabled (false);
				ui.montoDesem_1->setEnabled (false);
				ui.fechaDesem_1->setDate (QDate::currentDate ());
				ui.montoDesem_1->setText ("");
				ui.iva->setText ("");
				ui.lineaDeCredito->setEnabled (false);
				ui.lineaDeCredito->setCurrentIndex (-1);
				ui.moneda->setEnabled (false);
				ui.cuotaInicial->setEnabled (false);
				break;
			default:
				break;
			}
			// impuestos nacionales does not have desembolsos
			if (ui.entidad->currentText () == QString::fromLatin1 ("Impuestos Nacionales")) {
				ui.montoDesem_1->setEnabled (false);
				ui.fechaDesem_1->setEnabled (false);
				if (currentOperation != nullptr) {
					currentOperation->isEntity_ImpuestosNacionales = true;
				}
			}
			else {
				if (currentOperation != nullptr) {
					currentOperation->isEntity_ImpuestosNacionales = false;
				}
				if (OperacionesFinancieras::MapOperationString (ui.tipoOperacion->currentText ()) != OperacionesFinancieras::TiposDeOperacion::CasoLeasing) {
					ui.montoDesem_1->setEnabled (true);
					ui.fechaDesem_1->setEnabled (true);
				}
			}
		}
		else {
			currentOperation = nullptr;
		}
}

//==================================================================
//========================= fields utility =========================
void PlanDePagos::clearFields () {
	// first column
	ui.tipoOperacion->setCurrentIndex (-1);  // ======== instead of setCurrentText("")
	ui.numeroContrato->setText ("");
	ui.fechaFirma->setDate (QDate::currentDate ());
	ui.fechaFirma->setMaximumDate (QDate::currentDate ());
	ui.empresa->setCurrentIndex (-1);  // ======== instead of setCurrentText("")
	ui.entidad->setCurrentIndex (-1);  // ======== instead of setCurrentText("")
	ui.tipoEntidad->setText ("");
	ui.lineaDeCredito->setCurrentIndex (-1);
	// second column
	ui.monto->setText ("");
	ui.moneda->setCurrentIndex (-1);  // ======== instead of setCurrentText("")
	ui.iva->setText ("");
	ui.cuotaInicial->setText ("");
	ui.tipoTasa->setCurrentIndex (-1);  // ======== instead of setCurrentText("")
	ui.interesFijo->setText ("");
	ui.interesVariable->setText ("");
	// third column
	ui.frecuencia->setCurrentIndex (-1);  // ======== instead of setCurrentText("")
	ui.plazo->setValue (1);
	ui.fechaVencimiento->setDate (QDate::currentDate ().addMonths (1));
	// fourth column
	ui.fechaDesem_1->setDate (QDate::currentDate ());
	ui.montoDesem_1->setText ("");
	ui.fechaDesem_2->setDate (QDate::currentDate ());
	ui.montoDesem_2->setText ("");
	ui.fechaDesem_3->setDate (QDate::currentDate ());
	ui.montoDesem_3->setText ("");
	ui.fechaDesem_4->setDate (QDate::currentDate ());
	ui.montoDesem_4->setText ("");
	ui.fechaDesem_5->setDate (QDate::currentDate ());
	ui.montoDesem_5->setText ("");
	// extra row
	ui.concepto->setText ("");
	ui.detalle->setText ("");
	// clear table
	ui.plannedFee->setRowCount (0);
}

void PlanDePagos::blockFields () {
	ui.tipoOperacion->setEnabled (false);
	ui.numeroContrato->setEnabled (false);
	ui.fechaFirma->setEnabled (false);
	ui.empresa->setEnabled (false);
	ui.entidad->setEnabled (false);
	ui.lineaDeCredito->setEnabled (false);
	// second column
	ui.monto->setEnabled (false);
	ui.moneda->setEnabled (false);
	ui.cuotaInicial->setEnabled (false);
	ui.tipoTasa->setEnabled (false);
	ui.interesFijo->setEnabled (false);
	ui.interesVariable->setEnabled (false);
	// third column
	ui.frecuencia->setEnabled (false);
	ui.plazo->setEnabled (false);
	ui.fechaVencimiento->setEnabled (false);
	// fourth column
	ui.fechaDesem_1->setEnabled (false);
	ui.montoDesem_1->setEnabled (false);
	ui.fechaDesem_2->setEnabled (false);
	ui.montoDesem_2->setEnabled (false);
	ui.fechaDesem_3->setEnabled (false);
	ui.montoDesem_3->setEnabled (false);
	ui.fechaDesem_4->setEnabled (false);
	ui.montoDesem_4->setEnabled (false);
	ui.fechaDesem_5->setEnabled (false);
	ui.montoDesem_5->setEnabled (false);
	// extra row
	ui.concepto->setEnabled (false);
	ui.detalle->setEnabled (false);
	// Buttons
		// Planes buttons
	ui.editPlan->setEnabled (false);
	ui.clearButton->setEnabled (false);
	ui.deletePlan->setEnabled (false);
	ui.savePlan->setEnabled (false);
	// Cuotas buttons
	ui.addCuota->setEnabled (false);
}

void PlanDePagos::resetFields () {
	clearFields ();
	blockFields ();
}

void PlanDePagos::unlockField () {
	// first column, except linea de credito
	ui.tipoOperacion->setEnabled (true);
	ui.numeroContrato->setEnabled (true);
	ui.fechaFirma->setEnabled (true);
	ui.empresa->setEnabled (true);
	ui.entidad->setEnabled (true);
	// second column, except rates and cuota inicial
	ui.monto->setEnabled (true);
	ui.moneda->setEnabled (true);
	ui.tipoTasa->setEnabled (true);
	// third column
	ui.frecuencia->setEnabled (true);
	ui.plazo->setEnabled (true);
	ui.fechaVencimiento->setEnabled (true);
	// extra row
	ui.concepto->setEnabled (true);
	ui.detalle->setEnabled (true);
	// Buttons
		// Planes buttons
	ui.clearButton->setEnabled (true);
	ui.savePlan->setEnabled (true);
}

void PlanDePagos::enableDueButtons () {
	ui.savePlan->setEnabled (false);
	ui.editPlan->setEnabled (true);
	ui.deletePlan->setEnabled (true);

	ui.addCuota->setEnabled (true);
	ui.editCuota->setEnabled (true);
	ui.deleteCuota->setEnabled (true);

	//=====================================
	ui.tipoOperacion->setEnabled (false);
	ui.numeroContrato->setEnabled (false);
	ui.fechaFirma->setEnabled (false);
	ui.empresa->setEnabled (false);
	ui.entidad->setEnabled (false);
	ui.lineaDeCredito->setEnabled (false);
	// second column
	ui.monto->setEnabled (false);
	ui.moneda->setEnabled (false);
	ui.cuotaInicial->setEnabled (false);
	ui.tipoTasa->setEnabled (false);
	ui.interesFijo->setEnabled (false);
	ui.interesVariable->setEnabled (false);
	// third column
	ui.frecuencia->setEnabled (false);
	ui.plazo->setEnabled (false);
	ui.fechaVencimiento->setEnabled (false);
	// fourth column
	ui.fechaDesem_1->setEnabled (false);
	ui.montoDesem_1->setEnabled (false);
	ui.fechaDesem_2->setEnabled (false);
	ui.montoDesem_2->setEnabled (false);
	ui.fechaDesem_3->setEnabled (false);
	ui.montoDesem_3->setEnabled (false);
	ui.fechaDesem_4->setEnabled (false);
	ui.montoDesem_4->setEnabled (false);
	ui.fechaDesem_5->setEnabled (false);
	ui.montoDesem_5->setEnabled (false);
	// extra row
	ui.concepto->setEnabled (false);
	ui.detalle->setEnabled (false);
}

void PlanDePagos::makePlanEditable () {
	editingPlan = true;
	ui.savePlan->setEnabled (true);
	ui.clearButton->setEnabled (true);
	//====================== first column except  credit line ======================
	ui.tipoOperacion->setEnabled (true);
	ui.numeroContrato->setEnabled (true);
	ui.fechaFirma->setEnabled (true);
	ui.empresa->setEnabled (true);
	ui.entidad->setEnabled (true);
	//====================== second column except initial due =======================
	ui.moneda->setEnabled (true);
	ui.monto->setEnabled (true);
	ui.tipoTasa->setEnabled (true);
	if (OperacionesFinancieras::MapTipoTasaString (ui.tipoTasa->currentText ()) == OperacionesFinancieras::TipoTasa::Fijo) {
		ui.interesFijo->setEnabled (true);
	}
	else {
		if (OperacionesFinancieras::MapTipoTasaString (ui.tipoTasa->currentText ()) == OperacionesFinancieras::TipoTasa::Variable) {
			ui.interesFijo->setEnabled (true);
			ui.interesVariable->setEnabled (true);
		}
	}
	//====================== third column =============================================
	ui.frecuencia->setEnabled (true);
	ui.plazo->setEnabled (true);
	ui.fechaVencimiento->setEnabled (true);
	//========================= fourth column =========================================
	ui.montoDesem_1->setEnabled (true);
	ui.fechaDesem_1->setEnabled (true);
	//============================== extra row ====================================
	ui.concepto->setEnabled (true);
	ui.detalle->setEnabled (true);
}
//==================================================================

void PlanDePagos::onNewClicked () {
	loadedFromLeftlist = false;
	editingPlan = false;
	resetFields ();
	unlockField ();
	clearFields ();
	ui.tipoOperacion->setFocus ();
}

void PlanDePagos::onClearClicked () {
	resetFields ();
	delete currentOperation;
	currentOperation = nullptr;

	loadedFromLeftlist = false;
	editingPlan = false;
}

void PlanDePagos::onSaveClicked () {
	if (currentOperation != nullptr) {
		ui.savePlan->setEnabled (false);
		updateModel ();
		if (!editingPlan) {
			currentOperation->save (this->targetAddress, this->token);		// Validation is inside this method. If there are errores object will notify through signal notifyValidationStatus
		}
		else {
			currentOperation->update (this->targetAddress, this->token);		// Validation is inside this method. If there are errores object will notify through signal notifyValidationStatus
		}
	}
	else {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Por favor seleccione un tipo de operación"));
		ui.tipoOperacion->setFocus ();
	}
}

void PlanDePagos::onAddDue () {
	AddCuotaDelPlan addWindow (this);
	if (ui.plannedFee->rowCount () == 0) {
		//addWindow.setWindowData (this->targetAddress, this->token, 1, QDate::fromString (ui.fechaFirma->text (), "dd/MM/yyyy"), caso, this->currentPlan);
		addWindow.setValidationParams (this->targetAddress, this->token, this->currentOperation);
	}
	else {
		//addWindow.setWindowData (this->targetAddress, this->token, ui.effectiveFee->rowCount () + 1, QDate::fromString (ui.effectiveFee->item (ui.effectiveFee->rowCount () - 1, 1)->text (), "dd/MM/yyyy"), caso, this->currentPlan);
		CuotasPlanDePagos lastDue(this);
		lastDue.setDueNumber (ui.plannedFee->item (ui.plannedFee->rowCount () - 1, 0)->text ().toInt ());
		lastDue.setDueDate (QDate::fromString (ui.plannedFee->item (ui.plannedFee->rowCount () - 1, 3)->text (), "dd/MM/yyyy"));

		addWindow.setValidationParams (this->targetAddress, this->token, this->currentOperation, &lastDue, false);
	}

	connect (&addWindow, &AddCuotaDelPlan::accepted, this, &PlanDePagos::reloadSelectedPlan);

	addWindow.setModal (true);
	addWindow.exec ();
	addWindow.deleteLater ();
}

void PlanDePagos::onDeletePlan () {
	QMessageBox::StandardButton answer = QMessageBox::question (this, "Eliminar", QString::fromLatin1 ("¿Eliminar la operación \"") + currentOperation->getContractNumber () + QString::fromLatin1 ("\"?"));
	if (answer == QMessageBox::StandardButton::Yes) {
		if (currentOperation->deleteRes (this->targetAddress, this->token)) {
			QMessageBox::information (this, QString::fromLatin1 ("Éxito"), QString::fromLatin1 ("Borrado con éxito"));
			delete currentOperation;
			currentOperation = nullptr;
			resetFields ();
			loadPlanesDePago ();
		}
		else {
			QMessageBox::information (this, "Error", QString::fromLatin1 ("No se pudo completar la operación"));
		}
	}
}

void PlanDePagos::onUpdatePlan () {
	makePlanEditable ();
	ui.editPlan->setEnabled (false);
}

void PlanDePagos::onUpdateDue () {
	if (currentOperation!=nullptr) {
		if (ui.plannedFee->selectedItems ().length () > 0) {
			int row = ui.plannedFee->selectedItems ().at (0)->row ();
			int id = ui.plannedFee->item (row, 9)->text ().toInt ();
			QMessageBox::StandardButton answer = QMessageBox::question (this, QString::fromLatin1 ("Actualizar"), QString::fromLatin1 ("¿Actualizar la cuota \"") + ui.plannedFee->item (row, 0)->text () + QString::fromLatin1 ("\" de la operación \"") + ui.numeroContrato->text () + "\"?");
			if (answer == QMessageBox::StandardButton::Yes) {
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				AddCuotaDelPlan addWindow (this);
				if (row == 0) {
					CuotasPlanDePagos currentDue (&addWindow);
					currentDue.setID (ui.plannedFee->item (row, 11)->text ().toInt ());
					currentDue.setDueNumber (ui.plannedFee->item (row, 0)->text ().toInt ());
					currentDue.setDueDate (QDate::fromString (ui.plannedFee->item (row, 3)->text (), "dd/MM/yyyy"));
					currentDue.setTotal (ui.plannedFee->item (row, 4)->text ().toDouble ());
					currentDue.setCapital (ui.plannedFee->item (row, 5)->text ().toDouble ());
					currentDue.setInterest (ui.plannedFee->item (row, 6)->text ().toDouble ());
					currentDue.setIva (ui.plannedFee->item (row, 7)->text ().toDouble ());

					addWindow.setValidationParams (this->targetAddress, this->token, this->currentOperation, nullptr, true, &currentDue);
				}
				else {
					CuotasPlanDePagos lastDue (this);
					lastDue.setDueNumber (ui.plannedFee->item (row - 1, 0)->text ().toInt ());
					lastDue.setDueDate (QDate::fromString (ui.plannedFee->item (row - 1, 3)->text (), "dd/MM/yyyy"));

					CuotasPlanDePagos currentDue (&addWindow);
					currentDue.setID (ui.plannedFee->item (row, 11)->text ().toInt ());
					currentDue.setDueNumber (ui.plannedFee->item (row, 0)->text ().toInt ());
					currentDue.setDueDate (QDate::fromString (ui.plannedFee->item (row, 3)->text (), "dd/MM/yyyy"));
					currentDue.setTotal (ui.plannedFee->item (row, 4)->text ().toDouble ());
					currentDue.setCapital (ui.plannedFee->item (row, 5)->text ().toDouble ());
					currentDue.setInterest (ui.plannedFee->item (row, 6)->text ().toDouble ());
					currentDue.setIva (ui.plannedFee->item (row, 7)->text ().toDouble ());


					addWindow.setValidationParams (this->targetAddress, this->token, this->currentOperation, &lastDue, true, &currentDue);
				}

				connect (&addWindow, &AddCuotaDelPlan::accepted, this, &PlanDePagos::reloadSelectedPlan);

				addWindow.setModal (true);
				addWindow.exec ();
				addWindow.deleteLater ();
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
		}
		else {
			QMessageBox::warning (this, QString::fromLatin1 ("Cuota sin seleccionar"), QString::fromLatin1 ("Por favor seleccione la cuota que desea editar"));
		}
	}
	else {
		QMessageBox::warning (this, "Error", QString::fromLatin1 ("Por favor seleccione un plan de pagos"));
		ui.planesList->setFocus ();
	}
}

void PlanDePagos::onDeleteDue () {
	if (currentOperation != nullptr) {
		if (ui.plannedFee->selectedItems ().length () > 0) {
			int row = ui.plannedFee->selectedItems ().at (0)->row ();
			int id = ui.plannedFee->item (row, 11)->text ().toInt ();
			QMessageBox::StandardButton answer = QMessageBox::question (this, QString::fromLatin1 ("Eliminar"), QString::fromLatin1 ("¿Eliminar la cuota \"") + ui.plannedFee->item (row, 0)->text () + QString::fromLatin1 ("\" de la operación \"") + ui.numeroContrato->text () + "\"?");
			if (answer == QMessageBox::StandardButton::Yes) {
				if (CuotasPlanDePagos::deleteRes (this->targetAddress, this->token, id)) {
					QMessageBox::information (this, QString::fromLatin1 ("Éxito"), QString::fromLatin1 ("Cuota eliminada con éxito"));
					reloadSelectedPlan ();
				}
				else {
					QMessageBox::critical (this, QString::fromLatin1 ("Error"), QString::fromLatin1 ("No se pudo eliminar la Cuota"));
				}
			}
		}
		else {
			QMessageBox::warning (this, QString::fromLatin1 ("Cuota sin seleccionar"), QString::fromLatin1 ("Por favor seleccione la cuota que desea eliminar"));
		}
	}
	else {
		QMessageBox::warning (this, "Error", QString::fromLatin1 ("Por favor seleccione un plan de pagos"));
		ui.planesList->setFocus ();
	}
}

void PlanDePagos::setupConnections () {
	//======================================================================================
	//======================================== plan ========================================
	// "nuevo" clicked
	connect (ui.newPlan, &QPushButton::clicked, this, &PlanDePagos::onNewClicked);
	// "borrar" clicked
	connect (ui.clearButton, &QPushButton::clicked, this, &PlanDePagos::onClearClicked);
	// "registrar" clicked
	connect (ui.savePlan, &QPushButton::clicked, this, &PlanDePagos::onSaveClicked);
	// "eliminar" clicked
	connect (ui.deletePlan, &QPushButton::clicked, this, &PlanDePagos::onDeletePlan);
	// "actualizar" clicked
	connect (ui.editPlan, &QPushButton::clicked, this, &PlanDePagos::onUpdatePlan);
	//======================================================================================

	//======================================================================================
	//======================================= cuotas =======================================
	// "añadir cuota" clicked
	connect (ui.addCuota, &QPushButton::clicked, this, &PlanDePagos::onAddDue);
	// "actualizar" clicked
	connect (ui.editCuota, &QPushButton::clicked, this, &PlanDePagos::onUpdateDue);
	// "eliminar" clicked
	connect (ui.deleteCuota, &QPushButton::clicked, this, &PlanDePagos::onDeleteDue);
	//======================================================================================

	// load plan selected on left panel
	connect (ui.planesList, &QListWidget::itemDoubleClicked, this, &PlanDePagos::loadClickedPlan);
}

void PlanDePagos::setupUiConnections () {
	// user selected an operation type
	connect (ui.tipoOperacion, &QComboBox::currentTextChanged, this, &PlanDePagos::operationTypeSelected);

	// moneda changed, update moneda everywhere
	connect (ui.moneda, &QComboBox::currentTextChanged, this, &PlanDePagos::currencySelected);

	// rate type changed, update rate type
	connect (ui.tipoTasa, &QComboBox::currentTextChanged, this, &PlanDePagos::rateTypeSelected);

	//entity  changed, update entity type
	connect (ui.entidad, &QComboBox::currentTextChanged, this, [&](QString entity) {
		ui.tipoEntidad->setText (listaTiposEntidades[listaEntidades[entity]["tipoDeEntidad"]]);
		if (entity == QString::fromLatin1 ("Impuestos Nacionales")) {
			ui.montoDesem_1->setEnabled (false);
			ui.fechaDesem_1->setEnabled (false);
			if (currentOperation != nullptr) {
				currentOperation->isEntity_ImpuestosNacionales = true;
			}
		}
		else {
			if (currentOperation != nullptr) {
				currentOperation->isEntity_ImpuestosNacionales = false;
			}
			if (OperacionesFinancieras::MapOperationString (ui.tipoOperacion->currentText ()) != OperacionesFinancieras::TiposDeOperacion::CasoLeasing ) {
				ui.montoDesem_1->setEnabled (true);
				ui.fechaDesem_1->setEnabled (true);
			}
		}
		if (currentOperation != nullptr) {
			if (currentOperation->getOperationType () == OperacionesFinancieras::TiposDeOperacion::CasoLineaDeCredito) {
				loadLineasDeCredito (this->listaEntidades[entity]["id"].toInt (), listaEmpresas[ui.empresa->currentText ()].toInt ());
			}
		}
		});

	// enterprise changed, load credit lines for current enterprise
	connect (ui.empresa, &QComboBox::currentTextChanged, this, [&](QString enterprise) {
		if (currentOperation != nullptr) {
			if (currentOperation->getOperationType () == OperacionesFinancieras::TiposDeOperacion::CasoLineaDeCredito) {
				loadLineasDeCredito (this->listaEntidades[ui.entidad->currentText ()]["id"].toInt (), listaEmpresas[enterprise].toInt ());
			}
		}
		});

	// term changed, update expiration date
	connect (ui.plazo, qOverload<int> (&QSpinBox::valueChanged), this, &PlanDePagos::termChanged);
	
	// sign date and expiration date changed
	connect (ui.fechaFirma, &QDateEdit::dateChanged, this, &PlanDePagos::signDateChanged);
	connect (ui.fechaVencimiento, &QDateEdit::dateChanged, this, &PlanDePagos::expirationDateChanged);

	// desembolsos changed: enable next if there is data or disable next if there is a blank
	connect (ui.montoDesem_1, &QLineEdit::textChanged, this, &PlanDePagos::desem_1_changed);
	connect (ui.montoDesem_2, &QLineEdit::textChanged, this, &PlanDePagos::desem_2_changed);
	connect (ui.montoDesem_3, &QLineEdit::textChanged, this, &PlanDePagos::desem_3_changed);
	connect (ui.montoDesem_4, &QLineEdit::textChanged, this, &PlanDePagos::desem_4_changed);

	// Monto changed, then changes iva if leasing or leaseback
	connect (ui.monto, &NumberInput::valueChanged, this, [&](double value) {
		if (currentOperation != nullptr) {
			if (currentOperation->getOperationType () == OperacionesFinancieras::TiposDeOperacion::CasoLeasing || currentOperation->getOperationType () == OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack) {
				ui.iva->setText (QString::number (0.13 * value, 'f', 2));
			}
		}
		});

	//=====================================================================================================
	//========================================= enterkey pressed ==========================================
	connect (ui.numeroContrato, &QLineEdit::returnPressed, ui.savePlan, &QPushButton::click);
	connect (ui.monto, &NumberInput::returnPressed, ui.savePlan, &QPushButton::click);
	connect (ui.cuotaInicial, &NumberInput::returnPressed, ui.savePlan, &QPushButton::click);
	connect (ui.interesFijo, &NumberInput::returnPressed, ui.savePlan, &QPushButton::click);
	connect (ui.interesVariable, &NumberInput::returnPressed, ui.savePlan, &QPushButton::click);
	connect (ui.concepto, &QLineEdit::returnPressed, ui.savePlan, &QPushButton::click);
	connect (ui.detalle, &QLineEdit::returnPressed, ui.savePlan, &QPushButton::click);
	connect (ui.montoDesem_1, &NumberInput::returnPressed, ui.savePlan, &QPushButton::click);
	connect (ui.montoDesem_2, &NumberInput::returnPressed, ui.savePlan, &QPushButton::click);
	connect (ui.montoDesem_3, &NumberInput::returnPressed, ui.savePlan, &QPushButton::click);
	connect (ui.montoDesem_4, &NumberInput::returnPressed, ui.savePlan, &QPushButton::click);
	connect (ui.montoDesem_5, &NumberInput::returnPressed, ui.savePlan, &QPushButton::click);
	//=====================================================================================================

	//=====================================================================================================
	//========================================= search operations =========================================
	connect (ui.search, &QLineEdit::textChanged, this, [&](QString query) {
		if (query != "") {
			query = "&q=" + query;
		}
		loadPlanesDePago (query);
		});
	//=====================================================================================================

	//=====================================================================================================
	//======================================= credit line selected ========================================
	connect (ui.lineaDeCredito, &QComboBox::currentTextChanged, this, [&](QString creditLine) {
		if (currentOperation->getOperationType () == OperacionesFinancieras::TiposDeOperacion::CasoLineaDeCredito) {
			ui.moneda->setCurrentText (lineasDeCredito[creditLine]["moneda"]);
		}
		});
	//=====================================================================================================
}

void PlanDePagos::setupTableClipboard () {
	QAction* copyPlan = new QAction (ui.plannedFee);

	copyPlan->setShortcut (QKeySequence::Copy);
	copyPlan->setShortcutContext (Qt::ShortcutContext::WidgetShortcut);

	connect (copyPlan, &QAction::triggered, this, [&]() {
		if (ui.plannedFee->selectedItems ().length () > 0) {
			QClipboard* clipboard = QApplication::clipboard ();

			int currentRow = ui.plannedFee->selectedItems ().at (0)->row ();
			QString toClipboard = "";
			foreach (QTableWidgetItem * itm, ui.plannedFee->selectedItems ()) {
				if (currentRow == itm->row ()) {
					toClipboard += itm->text () + "\t";
				}
				else {
					toClipboard += "\n" + itm->text () + "\t";
				}
				currentRow = itm->row ();
			}

			clipboard->setText (toClipboard);
		}
		});

	ui.plannedFee->addAction (copyPlan);
}

void PlanDePagos::updateModel () {
	currentOperation->setContractNumber (ui.numeroContrato->text ()); //=====================================================================> Set contract number
	currentOperation->setSignDate (ui.fechaFirma->date ()); //==============================================================================> Set sign date
	currentOperation->setConcept (ui.concepto->text ()); //===================================================================================> Set concept
	currentOperation->setDetail (ui.detalle->text ()); //=====================================================================================> Set detail
	OperacionesFinancieras::Moneda currency = OperacionesFinancieras::MapMonedaString (ui.moneda->currentText ()); //========================> Set currency
	currentOperation->setCurrency (currency); //==================================================================//
	currentOperation->setAmmount (ui.monto->getValue ()); //=================================================================================> Set ammount
	currentOperation->setIVA (ui.iva->text ().toDouble ()); //===============================================================================> Set iva
	OperacionesFinancieras::TipoTasa rateType = OperacionesFinancieras::MapTipoTasaString (ui.tipoTasa->currentText ()); //==================> Set rate type
	currentOperation->setRateType (rateType); //========================================================================//
	currentOperation->setStaticRate (ui.interesFijo->getValue ()); //========================================================================> Set static rate
	currentOperation->setDynamicRate (ui.interesVariable->getValue ()); //===================================================================> Set dynamic rate
	currentOperation->setTerm (ui.plazo->value ()); //=======================================================================================> Set term
	OperacionesFinancieras::FrecuenciaDePagos frequency = OperacionesFinancieras::MapFrecuenciaString (ui.frecuencia->currentText ()); //====> Set frequency
	currentOperation->setFrequency (frequency); //====================================================================================//
	currentOperation->setExpirationDate (ui.fechaVencimiento->date ()); //===================================================================> Set expiration date
	currentOperation->setEnterprise (listaEmpresas[ui.empresa->currentText ()].toInt ()); //=================================================> Set enterprise
	currentOperation->setEntity (listaEntidades[ui.entidad->currentText ()]["id"].toInt ()); //==============================================> Set entity
	currentOperation->setFechaDesem_1 (ui.fechaDesem_1->date ()); //===================//====================================================> Set desem 1 
	currentOperation->setMontoDesem_1 (ui.montoDesem_1->getValue ()); //==============//
	currentOperation->setFechaDesem_2 (ui.fechaDesem_2->date ()); //=================//======================================================> Set desem 2
	currentOperation->setMontoDesem_2 (ui.montoDesem_2->getValue ()); //============//
	currentOperation->setFechaDesem_3 (ui.fechaDesem_3->date ()); //===============//========================================================> Set desem 3
	currentOperation->setMontoDesem_3 (ui.montoDesem_3->getValue ()); //==========//
	currentOperation->setFechaDesem_4 (ui.fechaDesem_4->date ()); //=============//==========================================================> Set desem 4
	currentOperation->setMontoDesem_4 (ui.montoDesem_4->getValue ()); //========//
	currentOperation->setFechaDesem_5 (ui.fechaDesem_5->date ()); //===========//============================================================> Set desem 5
	currentOperation->setMontoDesem_5 (ui.montoDesem_5->getValue ()); //======//
	currentOperation->setInitialDue (ui.cuotaInicial->getValue ()); //=======================================================================> Set cuota inicial
	currentOperation->setCreditLine (lineasDeCredito[ui.lineaDeCredito->currentText ()]["id"].toInt ()); //==================================> Set linea de credito
}
