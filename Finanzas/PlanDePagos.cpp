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
}

PlanDePagos::~PlanDePagos () {

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
	switch (selectedRateType)
	{
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

//==================================================================
//==================== catch validation errors =====================
void PlanDePagos::catchErrors (OperationValidationErros error, QString errorMessage) {
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
		QMessageBox::information (this, QString::fromLatin1 ("Error del servidor"), errorMessage);
		ui.tipoOperacion->setFocus ();
		break;
	case OperationValidationErros::NO_ERROR:
		QMessageBox::information (this, QString::fromLatin1 ("Éxito"), QString::fromLatin1 ("Guardado con éxito"));
		break;
	}
	ui.savePlan->setEnabled (true);
}
//==================================================================

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

void PlanDePagos::loadLineasDeCredito (int entidad_id) {
	lineasDeCredito.clear ();
	QNetworkAccessManager* nam = new QNetworkAccessManager (this);
	connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
		QByteArray resBin = reply->readAll ();
		if (reply->error ()) {
			QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
			QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
			return;
		}
		QJsonDocument okJson = QJsonDocument::fromJson (resBin);
		QStringList listaLineas;
		foreach (QJsonValue linea, okJson.object ().value ("lineasDeCredito").toArray ()) {
			listaLineas << linea.toObject ().value ("codigo").toString ();
			QHash <QString, QString> current;
			current.insert ("id", QString::number (linea.toObject ().value ("id").toInt ()));
			current.insert ("monto", QString::number (linea.toObject ().value ("monto").toDouble (), 'f', 2));
			current.insert ("empresa", linea.toObject ().value ("empresas_grupo").toObject ().value ("empresa").toString ());
			current.insert ("entidad", linea.toObject ().value ("entidades_financiera").toObject ().value ("nombreEntidad").toString ());
			current.insert ("moneda", linea.toObject ().value ("moneda").toString ());
			lineasDeCredito.insert (linea.toObject ().value ("codigo").toString (), current);
		}
		QCompleter* lineasCompleter = new QCompleter (listaLineas, this);
		lineasCompleter->setCaseSensitivity (Qt::CaseSensitivity::CaseInsensitive);
		ui.lineaDeCredito->setCompleter (lineasCompleter);

		reply->deleteLater ();
		});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/lineaDeCredito?status=1&entidad=" + QString::number (entidad_id)));
	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}
//==================================================================


void PlanDePagos::setTableHeaders () {
	ui.plannedFee->setColumnCount (11);
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
}

// Set auth data
void PlanDePagos::setAuthData (QString address, QString token, QString userName) {
	this->token = token;
	this->userName = userName;
	this->targetAddress = address;

	loadEmpresasGrupo ();
	loadEntidadesFinancieras ();
	loadTiposDeEntidad ();
}

void PlanDePagos::operationTypeSelected (QString operation) {
	// delete current operation, then asign a new one
	delete currentOperation;
	if (operation != "") {
		OperacionesFinancieras::TiposDeOperacion caso = OperacionesFinancieras::MapOperationString (QString::fromLatin1 (operation.toLatin1 ()));

		//if (caso == OperacionesFinancieras::TiposDeOperacion::CasoCredito) {
		currentOperation = new OperacionCredito (this);

		//}
		//else {
		//	currentOperation = nullptr;
		//	QMessageBox::information (this, "no", "not implemented");
		//	// back to credito
		//	ui.tipoOperacion->setCurrentText (OperacionesFinancieras::MapOperationEnum (OperacionesFinancieras::TiposDeOperacion::CasoCredito));
		//}


		// impuestos nacionales does not have desembolsos
		if (ui.entidad->currentText () == QString::fromLatin1 ("Impuestos Nacionales")) currentOperation->isEntity_ImpuestosNacionales = true;
		else currentOperation->isEntity_ImpuestosNacionales = false;
		// catch validation errors
		connect (currentOperation, &Operacion::notifyValidationStatus, this, &PlanDePagos::catchErrors);
		// enable and disable desembolsos on caso
		switch (caso) {
		case OperacionesFinancieras::TiposDeOperacion::CasoCredito:
			ui.fechaDesem_1->setEnabled (true);
			ui.montoDesem_1->setEnabled (true);
			break;
		case OperacionesFinancieras::TiposDeOperacion::CasoLineaDeCredito:
			ui.fechaDesem_1->setEnabled (true);
			ui.montoDesem_1->setEnabled (true);
			break;
		case OperacionesFinancieras::TiposDeOperacion::CasoLeasing:
			ui.fechaDesem_1->setEnabled (false);
			ui.montoDesem_1->setEnabled (false);
			ui.fechaDesem_1->setDate (QDate::currentDate ());
			ui.montoDesem_1->setText ("");
			break;
		case OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack:
			ui.fechaDesem_1->setEnabled (true);
			ui.montoDesem_1->setEnabled (true);
			break;
		case OperacionesFinancieras::TiposDeOperacion::CasoSeguro:
			ui.fechaDesem_1->setEnabled (true);
			ui.montoDesem_1->setEnabled (true);
			break;
		case OperacionesFinancieras::TiposDeOperacion::NONE:
			ui.fechaDesem_1->setEnabled (false);
			ui.montoDesem_1->setEnabled (false);
			ui.fechaDesem_1->setDate (QDate::currentDate ());
			ui.montoDesem_1->setText ("");
			break;
		default:
			break;
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
	ui.empresa->setCurrentIndex (-1);  // ======== instead of setCurrentText("")
	ui.entidad->setCurrentIndex (-1);  // ======== instead of setCurrentText("")
	ui.tipoEntidad->setText ("");
	ui.lineaDeCredito->setText ("");
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
	ui.fechaVencimiento->setDate (QDate::currentDate ());
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
	ui.editCuota->setEnabled (false);
	ui.deleteCuota->setEnabled (false);
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
	ui.editPlan->setEnabled (true);
	ui.clearButton->setEnabled (true);
	ui.deletePlan->setEnabled (true);
	ui.savePlan->setEnabled (true);
}
//==================================================================

void PlanDePagos::onNewClicked () {
	unlockField ();
}

void PlanDePagos::onClearClicked () {
	resetFields ();
	delete currentOperation;
	currentOperation = nullptr;
}

void PlanDePagos::onSaveClicked () {
	if (currentOperation != nullptr) {
		ui.savePlan->setEnabled (false);
		updateModel ();
		currentOperation->save (this->targetAddress, this->token);		// Validation is inside this method. If there are errores object will notify through signal notifyValidationStatus
	}
	else {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Por favor seleccione un tipo de operación"));
		ui.tipoOperacion->setFocus ();
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
	//======================================================================================
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
			}
		}
		});

	// desembolsos changed: enable next if there is data or disable next if there is a blank
	connect (ui.montoDesem_1, &QLineEdit::textChanged, this, &PlanDePagos::desem_1_changed);
	connect (ui.montoDesem_2, &QLineEdit::textChanged, this, &PlanDePagos::desem_2_changed);
	connect (ui.montoDesem_3, &QLineEdit::textChanged, this, &PlanDePagos::desem_3_changed);
	connect (ui.montoDesem_4, &QLineEdit::textChanged, this, &PlanDePagos::desem_4_changed);

	// Monto changed, then changes iva if leasing or leaseback
	connect (ui.monto, &NumberInput::valueChanged, this, [&](double value) {
		ui.iva->setText (QString::number (0.13 * value, 'f', 2));
		});


	//=====================================================================================================
	//========================================= enterkey pressed ==========================================
	connect (ui.numeroContrato, &QLineEdit::returnPressed, ui.savePlan, &QPushButton::click);
	connect (ui.lineaDeCredito, &QLineEdit::returnPressed, ui.savePlan, &QPushButton::click);
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
	currentOperation->setFechaDesem_1 (ui.fechaDesem_1->date ()); //===============//========================================================> Set desem 1 
	currentOperation->setMontoDesem_1 (ui.montoDesem_1->getValue ()); //==========//
	currentOperation->setFechaDesem_2 (ui.fechaDesem_2->date ()); //===============//========================================================> Set desem 2
	currentOperation->setMontoDesem_2 (ui.montoDesem_2->getValue ()); //==========//
	currentOperation->setFechaDesem_3 (ui.fechaDesem_3->date ()); //===============//========================================================> Set desem 3
	currentOperation->setMontoDesem_3 (ui.montoDesem_3->getValue ()); //==========//
	currentOperation->setFechaDesem_4 (ui.fechaDesem_4->date ()); //===============//========================================================> Set desem 4
	currentOperation->setMontoDesem_4 (ui.montoDesem_4->getValue ()); //==========//
	currentOperation->setFechaDesem_5 (ui.fechaDesem_5->date ()); //===============//========================================================> Set desem 5
	currentOperation->setMontoDesem_5 (ui.montoDesem_5->getValue ()); //==========//
	currentOperation->setInitialDue (ui.cuotaInicial->getValue ()); //=======================================================================> Set cuota inicial
}

