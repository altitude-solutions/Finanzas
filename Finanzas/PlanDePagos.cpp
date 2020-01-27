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

// TODO: validator Σ pago capital = monto de la operacion

PlanDePagos::PlanDePagos(QWidget *parent): QWidget(parent) {
	ui.setupUi(this);
	// ===============================================
	// Set all date pickers to popup a calendar
	// ===============================================
	ui.fechaFirma->setCalendarPopup (true);
	ui.fechaDesem_1->setCalendarPopup (true);
	ui.fechaDesem_2->setCalendarPopup (true);
	ui.fechaVencimiento->setCalendarPopup (true);
	ui.fechaPago->setCalendarPopup (true);

	// ===============================================
	// Set all datepickers format to Bolivian format
	// ===============================================
	ui.fechaFirma->setDisplayFormat ("dd/MM/yyyy");
	ui.fechaDesem_1->setDisplayFormat ("dd/MM/yyyy");
	ui.fechaDesem_2->setDisplayFormat ("dd/MM/yyyy");
	ui.fechaPago->setDisplayFormat ("dd/MM/yyyy");
	ui.fechaVencimiento->setDisplayFormat ("dd/MM/yyyy");

	// ===============================================
	// Set all dates to current date. also set minimum and maximum dates to current date
	// ===============================================
	//ui.fechaPago->setDate (QDate::currentDate ());
	ui.fechaPago->setMinimumDate (QDate::currentDate ());
	//ui.fechaFirma->setDate (QDate::currentDate ());
	ui.fechaFirma->setMaximumDate (QDate::currentDate ());
	//ui.fechaVencimiento->setDate (QDate::currentDate ());
	ui.fechaVencimiento->setMinimumDate (QDate::currentDate ());
	//ui.fechaDesem_1->setDate (QDate::currentDate ());
	ui.fechaDesem_1->setMinimumDate (QDate::currentDate ());	// More than fechaFirma
	//ui.fechaDesem_2->setDate (QDate::currentDate ());
	ui.fechaDesem_2->setMinimumDate (QDate::currentDate ());	// More than fechaDesem_1

	// ===============================================
	// Listen to Enter key to perform a virtual click on save buttons
	// ===============================================
	connect (ui.pagoIva, &QLineEdit::returnPressed, ui.saveCuota, &QPushButton::click);
	connect (ui.pagoInteres, &QLineEdit::returnPressed, ui.saveCuota, &QPushButton::click);
	connect (ui.pagoCapital, &QLineEdit::returnPressed, ui.saveCuota, &QPushButton::click);
	connect (ui.montoDesem_1, &QLineEdit::returnPressed, ui.savePlan, &QPushButton::click);
	connect (ui.montoDesem_2, &QLineEdit::returnPressed, ui.savePlan, &QPushButton::click);
	
	// ===============================================
	// When save is clicked then save data
	// ===============================================
	connect (ui.savePlan, &QPushButton::clicked, this, &PlanDePagos::onSavePlan);
	connect (ui.saveCuota, &QPushButton::clicked, this, &PlanDePagos::onSaveCouta);

	// ===============================================
	// Refresh tipo de entidad
	// ===============================================
	connect (ui.nombreEntidad, &QComboBox::currentTextChanged, this, [&](QString data) {
		ui.tipoEntidad->setCurrentText (listaTiposEntidades[listaEntidades[data]["tipoDeEntidad"]]);
		if (ui.tipoDeOperacion->currentText () == QString::fromLatin1("Operación de Línea de Crédito")) {
			loadLineasDeCredito (listaEntidades[data]["id"].toInt());
		}
	});


	// create validators:
	// If you change these arrays length you should modify reset methods as well
	// In such case append validators. DO NOT try to insert new ones in the middle
	/*
		1  [0]  codigo linea de credito no existe
		2  [1]  codigo linea de credito esta en su limite
		3  [2]  monto
		4  [3]  iva
		5  [4]  cuota inicial
		6  [5]  interes fijo
		7  [6]  interes variable
		8  [7]  Monto Desem 1
		9  [8]  Monto Desem 2
		10 [9]  Monto Desem 3
		11 [10] Monto Desem 4
		12 [11] Monto Desem 5
		13 [12] Suma Montos Desem
	*/
	planDataIsCorrect = new bool[13];
	/*
		1  [0] numero de cuota
		2  [1] monto total del pago
		3  [2] pago capital
		4  [3] pago interes
		5  [4] pago iva
		6  [5] Suma de todos los pagos debe ser igual al monto total
 	*/
	cuotaDataIsCorrect = new bool[6];
	
	resetPlanValidators ();
	resetCuotaValidators ();

	// ===============================================
	// Listen to date picker data changed for fechaFirma, fechaDesem_1
	// ===============================================
	connect (ui.fechaFirma, &QDateEdit::dateChanged, this, &PlanDePagos::fechaFirmaChanged);
	connect (ui.fechaDesem_1, &QDateEdit::dateChanged, this, &PlanDePagos::fechaDesem1Changed);

	// ===============================================
	// Listen to Plazo updated to change fechaVencimiento
	// ===============================================
	connect (ui.plazo, SIGNAL (valueChanged (int)), this, SLOT(onPlazoChanged (int)));

	// ===============================================
	// Listen to operation type changed
	// ===============================================
	connect (ui.tipoDeOperacion, SIGNAL (currentIndexChanged (int)), this, SLOT (onOperationTypeChanged (int)));

	// ===============================================
	// Populate table headers
	// ===============================================
	setTableHeaders ();

	// ===============================================
	// Look for linea de credito by codigo
	// ===============================================
	connect (ui.codigoLineaCredito, &QLineEdit::textChanged, this, &PlanDePagos::lookForLineaCredito);

	// ===============================================
	// Tipo tasa changed
	// ===============================================
	connect (ui.tipoTasa, SIGNAL (currentIndexChanged(int)), this, SLOT (tipoTasaChanged (int)));

	// IVA autofill
	connect (ui.monto, &QLineEdit::textChanged, this, &PlanDePagos::ivaAutoFill);

	// Monto changed. Slot for validation
	connect (ui.monto, &QLineEdit::textChanged, this, &PlanDePagos::montoChanged);

	// Pago IVA autofill
	connect (ui.pagoCapital, &QLineEdit::textChanged, this, &PlanDePagos::pagoIvaAutofill);
	connect (ui.pagoInteres, &QLineEdit::textChanged, this, &PlanDePagos::pagoInteresChanged);

	// Intereses changed
	connect (ui.interesFijo, &QLineEdit::textChanged, this, &PlanDePagos::interesFijoChanged);
	connect (ui.interesVariable, &QLineEdit::textChanged, this, &PlanDePagos::interesVariableChanged);

	// Montos desembolso changed
	connect (ui.montoDesem_1, &QLineEdit::textChanged, this, &PlanDePagos::onDesem1Changed);
	connect (ui.montoDesem_2, &QLineEdit::textChanged, this, &PlanDePagos::onDesem2Changed);
	connect (ui.montoDesem_3, &QLineEdit::textChanged, this, &PlanDePagos::onDesem3Changed);
	connect (ui.montoDesem_4, &QLineEdit::textChanged, this, &PlanDePagos::onDesem4Changed);
	connect (ui.montoDesem_5, &QLineEdit::textChanged, this, &PlanDePagos::onDesem5Changed);
	
	//Frecuencia de pagos changed
	connect (ui.frecuencia, &QComboBox::currentTextChanged, this, &PlanDePagos::onFrecuenciaDePagosChanged);

	// numero de cuota changed
	connect (ui.numeroCuota, SIGNAL (valueChanged (int)), this, SLOT (numeroCuotaChanged(int)));
	connect (ui.moneda, &QComboBox::currentTextChanged, this, &PlanDePagos::onMonedaChanged);



		
	// Default to Caso crédito
	clearFields ();
	casoCreditoSetup ();
}

PlanDePagos::~PlanDePagos() {
	delete[] planDataIsCorrect;
	delete[] cuotaDataIsCorrect;
}

void PlanDePagos::setTableHeaders () {
	ui.tableWidget->setColumnCount (16);
	// set columns names
	QStringList headers;
	headers << QString::fromLatin1 ("Cuota") << QString::fromLatin1 ("Empresa") << QString::fromLatin1 ("Entidad") << QString::fromLatin1 ("Nombre de Empresa")
		<< QString::fromLatin1 ("Detalle") << QString::fromLatin1 ("Nro de contrato/operación") << QString::fromLatin1 ("Año") << QString::fromLatin1 ("Mes")
		<< QString::fromLatin1 ("Fecha de Pago") << QString::fromLatin1 ("Pago Total") << QString::fromLatin1 ("Pago de Capital") << QString::fromLatin1 ("Pago de Interes")
		<< QString::fromLatin1 ("IVA") << QString::fromLatin1 ("Saldo del Capital") << QString::fromLatin1 ("Crédito Fiscal") << QString::fromLatin1 ("Saldo Capital Real");
	ui.tableWidget->setHorizontalHeaderLabels (headers);
	// Set column width to a proper size	// TODO: Dynamic sizes
	ui.tableWidget->setColumnWidth (0, 50);
	ui.tableWidget->setColumnWidth (1, 120);
	ui.tableWidget->setColumnWidth (2, 80);
	ui.tableWidget->setColumnWidth (3, 120);
	ui.tableWidget->setColumnWidth (4, 165);
	ui.tableWidget->setColumnWidth (5, 158);
	ui.tableWidget->setColumnWidth (6, 55);
	ui.tableWidget->setColumnWidth (7, 120);
	ui.tableWidget->setColumnWidth (8, 120);
	ui.tableWidget->setColumnWidth (9, 120);
	ui.tableWidget->setColumnWidth (10, 120);
	ui.tableWidget->setColumnWidth (11, 120);
	ui.tableWidget->setColumnWidth (12, 120);
	ui.tableWidget->setColumnWidth (13, 120);
	ui.tableWidget->setColumnWidth (14, 120);
	ui.tableWidget->setColumnWidth (15, 120);

	// hide vertical headers
	ui.tableWidget->verticalHeader ()->hide ();
}

void PlanDePagos::setAuthData (QString address, QString token, QString userName) {
	this->token = token;
	this->userName = userName;
	this->targetAddress = address;

	loadTiposDeEntidad ();
	loadEntidadesFinancieras ();
	loadEmpresasGrupo ();
}

void PlanDePagos::lookForLineaCredito (QString codigo) {
	// Check if exists
	if (lineasDeCredito.contains (codigo)) {
		planDataIsCorrect[0] = true;
		ui.empresaGrupo->setCurrentText (lineasDeCredito[codigo]["empresa"]);
	}
	else {
		planDataIsCorrect[0] = false;
	}
}

void PlanDePagos::numeroCuotaChanged (int value) {
	if (paidCuotas.contains (value)) {
		cuotaDataIsCorrect[0] = false;
	}
	else {
		cuotaDataIsCorrect[0] = true;
		QString frequency = ui.frecuencia->currentText ();
		int factor = frequency == "Mensual" ? 1 : frequency == "Bimensual" ? 2 : frequency == "Trimestral" ? 3 : frequency == "Semestral" ? 6 : 12;
		ui.fechaPago->setDate (ui.fechaFirma->date ().addMonths (value * factor));
	}
}

void PlanDePagos::pagoCuotaChanged (QString pago) {
	bool isNumber = false;
	double ammount = pago.toDouble (&isNumber);
	if (!isNumber) {
		cuotaDataIsCorrect[1] = false;
		pago.truncate (pago.length () - 1);
		ui.pagoMonto->setText (pago);
		return;
	}
	if (ammount > 0) {
		cuotaDataIsCorrect[1] = true;
	}
	else {
		cuotaDataIsCorrect[1] = false;
	}
}

void PlanDePagos::pagoCapitalChanged (QString capital) {
	bool isNumber = false;
	double ammount = capital.toDouble (&isNumber);
	if (!isNumber) {
		cuotaDataIsCorrect[2] = false;
		capital.truncate (capital.length () - 1);
		ui.pagoCapital->setText (capital);
		return;
	}
	if (ammount > 0) {
		cuotaDataIsCorrect[2] = true;

		int plazo = ui.plazo->value ();
		QString frequency = ui.frecuencia->currentText ();
		int factor = frequency == "Mensual" ? 1 : frequency == "Bimensual" ? 2 : frequency == "Trimestral" ? 3 : frequency == "Semestral" ? 6 : 12;
		int currentCuota = ui.numeroCuota->value ();
		if (currentCuota == (plazo / factor)) {
			if ((totalPaid + ammount) == ui.monto->text ().toDouble ()) {
				cuotaDataIsCorrect[5] = true;
			}
			else {
				cuotaDataIsCorrect[5] = false;
			}
		}
		else {
			if ((totalPaid + ammount) < ui.monto->text ().toDouble ()) {
				cuotaDataIsCorrect[5] = true;
			}
			else {
				cuotaDataIsCorrect[5] = false;
			}
		}
	}
	else {
		cuotaDataIsCorrect[2] = false;
	}
}

void PlanDePagos::pagoIvaChanged (QString iva) {
	bool isNumber = false;
	double ammount = iva.toDouble (&isNumber);
	if (!isNumber) {
		cuotaDataIsCorrect[4] = false;
		iva.truncate (iva.length () - 1);
		ui.pagoCapital->setText (iva);
		return;
	}
	if (ammount > 0) {
		cuotaDataIsCorrect[4] = true;
	}
	else {
		cuotaDataIsCorrect[4] = false;
	}
}

void PlanDePagos::loadEntidadesFinancieras () {
	listaEntidades.clear ();
	ui.nombreEntidad->clear ();
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
			ui.nombreEntidad->addItem (entidad.toObject ().value ("nombreEntidad").toString ());
			QHash<QString, QString> current;
			current.insert ("id", QString::number (entidad.toObject ().value ("id").toInt ()));
			current.insert ("nombreEntidad", entidad.toObject ().value ("nombreEntidad").toString ());
			current.insert ("tipoDeEntidad", QString::number (entidad.toObject ().value ("tipoDeEntidad").toInt ()));
			listaEntidades.insert (entidad.toObject ().value ("nombreEntidad").toString (), current);
		}
		reply->deleteLater ();
	});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/entidadFinanciera?status=1"));

	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}
		
void PlanDePagos::loadTiposDeEntidad () {
	ui.tipoEntidad->clear ();
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
			ui.tipoEntidad->addItem (entidad.toObject ().value ("tipoDeEntidad").toString ());
			listaTiposEntidades.insert (QString::number (entidad.toObject ().value ("id").toInt ()), entidad.toObject ().value ("tipoDeEntidad").toString ());
		}
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
			listaLineas << linea.toObject().value("codigo").toString();
			QHash <QString, QString> current;
			current.insert ("id", QString::number (linea.toObject ().value ("id").toInt ()));
			current.insert ("monto", QString::number (linea.toObject ().value ("monto").toDouble (), 'g', 15));
			current.insert ("empresa", linea.toObject ().value ("empresas_grupo").toObject ().value ("empresa").toString ());
			current.insert ("entidad", linea.toObject ().value ("entidades_financiera").toObject ().value ("nombreEntidad").toString ());
			lineasDeCredito.insert (linea.toObject ().value ("codigo").toString (), current);
		}
		QCompleter* lineasCompleter = new QCompleter (listaLineas, this);
		lineasCompleter->setCaseSensitivity (Qt::CaseSensitivity::CaseInsensitive);
		ui.codigoLineaCredito->setCompleter (lineasCompleter);

		reply->deleteLater ();
	});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/lineaDeCredito?status=1&entidad=" + QString::number(entidad_id)));
	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}

void PlanDePagos::loadEmpresasGrupo () {
	listaEmpresas.clear ();
	ui.empresaGrupo->clear ();
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
			ui.empresaGrupo->addItem (entidad.toObject ().value ("empresa").toString ());
			listaEmpresas.insert (entidad.toObject ().value ("empresa").toString (), QString::number (entidad.toObject ().value ("id").toInt ()));
		}
		reply->deleteLater ();
	});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/empresas?status=1"));

	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}

void PlanDePagos::fechaDesem1Changed (QDate date) {
	if (ui.fechaDesem_2->date () < date) {
		ui.fechaDesem_2->setDate (date);
	}
	ui.fechaDesem_2->setMinimumDate (date);
}

void PlanDePagos::montoChanged (QString data) {
	bool isNumber = false;
	double ammount = data.toDouble (&isNumber);
	if (!isNumber) {
		planDataIsCorrect[2] = false;
		data.truncate (data.length () - 1);
		ui.monto->setText (data);
		return;
	}
	if (ammount > 0) {
		planDataIsCorrect[2] = true;
	}
	else {
		planDataIsCorrect[2] = false;
	}
}

void PlanDePagos::tipoTasaChanged (int type) {
	if (type == 1) {
		ui.interesVariable->setEnabled (true);
	}
	else {
		ui.interesVariable->setEnabled (false);
		ui.interesVariable->setText ("");
	}
}

void PlanDePagos::interesFijoChanged (QString interes) {
	bool isNumber = false;
	double ammount = interes.toDouble (&isNumber);
	if (!isNumber) {
		planDataIsCorrect[5] = false;
		interes.truncate (interes.length () - 1);
		ui.interesFijo->setText (interes);
		return;
	}
	if (ammount > 0) {
		planDataIsCorrect[5] = true;
	}
	else {
		planDataIsCorrect[5] = false;
	}
}

void PlanDePagos::interesVariableChanged (QString interes) {
	bool isNumber = false;
	double ammount = interes.toDouble (&isNumber);
	if (!isNumber) {
		planDataIsCorrect[6] = false;
		interes.truncate (interes.length () - 1);
		ui.interesVariable->setText (interes);
		return;
	}
	if (ammount > 0) {
		planDataIsCorrect[6] = true;
	}
	else {
		planDataIsCorrect[6] = false;
	}
}

void PlanDePagos::ivaAutoFill (QString monto) {
	if (ui.tipoDeOperacion->currentText () == QString::fromLatin1 ("Lease Back")) {
		double usable = monto.toDouble ();
		double computado = usable * 0.13;
		ui.iva->setText (QString::number (computado));
	}
}

void PlanDePagos::pagoIvaAutofill (QString monto) {
	if (ui.tipoDeOperacion->currentText () == QString::fromLatin1 ("Leasing") || ui.tipoDeOperacion->currentText () == QString::fromLatin1 ("Lease Back")) {
		double usable = monto.toDouble ();
		double computado = 0.13 * usable / 0.87;
		ui.pagoIva->setText (QString::number (computado));
	}
}

void PlanDePagos::cuotaInicialChanged (QString cuotaInicial) {
	bool isNumber = false;
	double ammount = cuotaInicial.toDouble (&isNumber);
	if (!isNumber) {
		planDataIsCorrect[4] = false;
		cuotaInicial.truncate (cuotaInicial.length () - 1);
		ui.cuotaInicial->setText (cuotaInicial);
		return;
	}
	if (ammount > 0) {
		planDataIsCorrect[4] = true;
	}
	else {
		planDataIsCorrect[4] = false;
	}
}

void PlanDePagos::pagoInteresChanged (QString interes) {
	bool isNumber = false;
	double ammount = interes.toDouble (&isNumber);
	if (!isNumber) {
		cuotaDataIsCorrect[3] = false;
		interes.truncate (interes.length () - 1);
		ui.pagoInteres->setText (interes);
		return;
	}
	if (ammount >= 0) {
		cuotaDataIsCorrect[3] = true;
	}
	else {
		cuotaDataIsCorrect[3] = false;
	}
}

void PlanDePagos::onPlazoChanged (int value) {
	QDate plazo = ui.fechaFirma->date();
	ui.fechaVencimiento->setDate (plazo.addMonths(value));
}

void PlanDePagos::onDesem1Changed (QString desem1) {
	bool isNumber = false;
	double ammount = desem1.toDouble (&isNumber);
	if (!isNumber) {
		planDataIsCorrect[7] = false;
		desem1.truncate (desem1.length () - 1);
		ui.montoDesem_1->setText (desem1);
		return;
	}
	if (ammount > 0) {
		planDataIsCorrect[7] = true;
	}
	else {
		planDataIsCorrect[7] = false;
	}

	double ammount_2 = ui.montoDesem_2->text ().toDouble (),
		ammount_3 = ui.montoDesem_3->text ().toDouble (),
		ammount_4 = ui.montoDesem_4->text ().toDouble (),
		ammount_5 = ui.montoDesem_5->text ().toDouble ();
	
	double sum = ammount + ammount_2 + ammount_3 + ammount_4 + ammount_5;
	if (ui.monto->text ().toDouble () == sum) {
		planDataIsCorrect[12] = true;
	}
	else {
		planDataIsCorrect[12] = false;
	}
}

void PlanDePagos::onDesem2Changed (QString desem2) {
	bool isNumber = false;
	double ammount = desem2.toDouble (&isNumber);
	if (!isNumber) {
		planDataIsCorrect[8] = false;
		desem2.truncate (desem2.length () - 1);
		ui.montoDesem_2->setText (desem2);
		return;
	}
	if (ammount > 0) {
		planDataIsCorrect[8] = true;
	}
	else {
		planDataIsCorrect[8] = false;
	}


	double ammount_2 = ui.montoDesem_1->text ().toDouble (),
		ammount_3 = ui.montoDesem_3->text ().toDouble (),
		ammount_4 = ui.montoDesem_4->text ().toDouble (),
		ammount_5 = ui.montoDesem_5->text ().toDouble ();

	double sum = ammount + ammount_2 + ammount_3 + ammount_4 + ammount_5;
	if (ui.monto->text ().toDouble () == sum) {
		planDataIsCorrect[12] = true;
	}
	else {
		planDataIsCorrect[12] = false;
	}
}

void PlanDePagos::onDesem3Changed (QString desem3) {
	bool isNumber = false;
	double ammount = desem3.toDouble (&isNumber);
	if (!isNumber) {
		planDataIsCorrect[9] = false;
		desem3.truncate (desem3.length () - 1);
		ui.montoDesem_3->setText (desem3);
		return;
	}
	if (ammount > 0) {
		planDataIsCorrect[9] = true;
	}
	else {
		planDataIsCorrect[9] = false;
	}
	

	double ammount_2 = ui.montoDesem_2->text ().toDouble (),
		ammount_3 = ui.montoDesem_1->text ().toDouble (),
		ammount_4 = ui.montoDesem_4->text ().toDouble (),
		ammount_5 = ui.montoDesem_5->text ().toDouble ();

	double sum = ammount + ammount_2 + ammount_3 + ammount_4 + ammount_5;
	if (ui.monto->text ().toDouble () == sum) {
		planDataIsCorrect[12] = true;
	}
	else {
		planDataIsCorrect[12] = false;
	}
}

void PlanDePagos::onDesem4Changed (QString desem4) {
	bool isNumber = false;
	double ammount = desem4.toDouble (&isNumber);
	if (!isNumber) {
		planDataIsCorrect[10] = false;
		desem4.truncate (desem4.length () - 1);
		ui.montoDesem_4->setText (desem4);
		return;
	}
	if (ammount > 0) {
		planDataIsCorrect[10] = true;
	}
	else {
		planDataIsCorrect[10] = false;
	}
	

	double ammount_2 = ui.montoDesem_2->text ().toDouble (),
		ammount_3 = ui.montoDesem_3->text ().toDouble (),
		ammount_4 = ui.montoDesem_1->text ().toDouble (),
		ammount_5 = ui.montoDesem_5->text ().toDouble ();

	double sum = ammount + ammount_2 + ammount_3 + ammount_4 + ammount_5;
	if (ui.monto->text ().toDouble () == sum) {
		planDataIsCorrect[12] = true;
	}
	else {
		planDataIsCorrect[12] = false;
	}
}

void PlanDePagos::onDesem5Changed (QString desem5) {
	bool isNumber = false;
	double ammount = desem5.toDouble (&isNumber);
	if (!isNumber) {
		planDataIsCorrect[11] = false;
		desem5.truncate (desem5.length () - 1);
		ui.montoDesem_5->setText (desem5);
		return;
	}
	if (ammount > 0) {
		planDataIsCorrect[11] = true;
	}
	else {
		planDataIsCorrect[11] = false;
	}
	

	double ammount_2 = ui.montoDesem_2->text ().toDouble (),
		ammount_3 = ui.montoDesem_3->text ().toDouble (),
		ammount_4 = ui.montoDesem_4->text ().toDouble (),
		ammount_5 = ui.montoDesem_1->text ().toDouble ();

	double sum = ammount + ammount_2 + ammount_3 + ammount_4 + ammount_5;
	if (ui.monto->text ().toDouble () == sum) {
		planDataIsCorrect[12] = true;
	}
	else {
		planDataIsCorrect[12] = false;
	}
}

void PlanDePagos::onFrecuenciaDePagosChanged (QString frecuencia) {
	QDate plazo = ui.fechaFirma->date ();
	QString frequency = ui.frecuencia->currentText ();
	int factor = frequency == "Mensual" ? 1 : frequency == "Bimensual" ? 2 : frequency == "Trimestral" ? 3 : frequency == "Semestral" ? 6 : 12;
	ui.fechaPago->setDate (plazo.addMonths (factor));
}

void PlanDePagos::fechaFirmaChanged (QDate date) {
	if (ui.fechaDesem_1->date () < date) {
		ui.fechaDesem_1->setDate (date);
	}
	ui.fechaDesem_1->setMinimumDate (date);
	ui.fechaVencimiento->setMinimumDate (date);
	QDate plazo = date.addMonths (ui.plazo->value ());
	ui.fechaVencimiento->setDate (plazo);

	QString frequency = ui.frecuencia->currentText ();
	int factor = frequency == "Mensual" ? 1 : frequency == "Bimensual" ? 2 : frequency == "Trimestral" ? 3 : frequency == "Semestral" ? 6 : 12;
	ui.fechaPago->setDate (date.addMonths (factor));
}

void PlanDePagos::onOperationTypeChanged (int type) {
	switch (type) {
	case 0:
		// Caso crédito
		casoCreditoSetup ();
		break;
	case 1:
		// Caso línea de crédito
		casoLineaDeCreditoSetup ();
		break;
	case 2:
		// Leasing
		casoLeasingSetup ();
		break;
	case 3:
		// Lease Back
		casoLeaseBackSetup ();
		break;
	case 4:
		// Seguro

		break;
	}
}

void PlanDePagos::casoCredito () {
	if (checkCasoCredito ()) {
		// General info
		QString tipoOperacion = ui.tipoDeOperacion->currentText ();
		QString numeroDeContratoOperacion = ui.numeroContratoOperacion->text ();
		QDateTime fechaFirma = ui.fechaFirma->dateTime();
		QString concepto = ui.concepto->text ();
		QString detalle = ui.detalle->text ();
		QString moneda = ui.moneda->currentText ();
		double monto = ui.monto->text ().toDouble ();
		QString tipoTasa = ui.tipoTasa->currentText ();
		double interesFijo = ui.interesFijo->text ().toDouble ();
		int plazo = ui.plazo->value ();
		QString frecuenciaDePagos = ui.frecuencia->currentText ();
		QDateTime fechaVencimiento = ui.fechaVencimiento->dateTime ();
		QDateTime fechaDesembolso_1 = ui.fechaDesem_1->dateTime ();
		double montoDesembolso_1 = ui.montoDesem_1->text ().toDouble ();
		int empresaGrupo = listaEmpresas[ui.empresaGrupo->currentText()].toInt();
		int entidadFinanciera = listaEntidades[ui.nombreEntidad->currentText ()]["id"].toInt ();

		// Network manager and request
		QNetworkAccessManager* nam = new QNetworkAccessManager (this);
		QNetworkRequest request;
		request.setUrl (QUrl (this->targetAddress + "/planDePagos"));
		request.setRawHeader("Content-Type", "application/json");
		request.setRawHeader ("token", this->token.toUtf8());

		// On response lambda
		connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
			QJsonDocument jsonReply = QJsonDocument::fromJson (reply->readAll());
			if (reply->error ()) {
				if (jsonReply.object ().value ("err").toObject ().contains ("message")) {
					// If there is a known error
					QMessageBox::critical (this, "Error", QString::fromLatin1 (jsonReply.object ().value ("err").toObject ().value ("message").toString ().toLatin1 ()));
				}
				else {
					if (reply->error () == QNetworkReply::ConnectionRefusedError) {
						QMessageBox::critical (this, QString::fromLatin1 ("Error de conexión"), QString::fromLatin1 ("No se pudo establecer conexión con el servidor"));
					}
					else {
						// If there is a server error
						QMessageBox::critical (this, "Error en base de datos", "Por favor enviar un reporte de error con una captura de pantalla de esta venta.\n" + QString::fromStdString (jsonReply.toJson ().toStdString ()));
					}
				}
				casoCreditoSetup ();
				ui.savePlan->setEnabled (true);
			}
			else {
				this->planID = jsonReply.object ().value ("planDePagos").toObject ().value ("id").toInt ();
				qDebug () << "El ID del plan de pagos es " << this->planID;
				lockGeneraInfoEnableCuotaInfo ();
				ui.pagoMonto->setFocus ();
			}
		});

		// Request body
		// if tipoTasa == Variable load interesVariable
		// if enableDesem_2 is checked load Desem_2
		QJsonDocument body;
		QJsonObject bodyContent;
		bodyContent.insert ("tipoOperacion", tipoOperacion);
		bodyContent.insert ("numeroDeContratoOperacion", numeroDeContratoOperacion);
		bodyContent.insert ("fechaFirma", fechaFirma.toMSecsSinceEpoch());
		bodyContent.insert ("concepto", concepto);
		bodyContent.insert ("detalle", detalle);
		bodyContent.insert ("moneda", moneda);
		bodyContent.insert ("monto", monto);
		bodyContent.insert ("tipoDeTasa", tipoTasa);
		bodyContent.insert ("interesFijo", interesFijo);
		if (tipoTasa == "Variable") {
			bodyContent.insert ("interesVariable", ui.interesVariable->text().toDouble());
		}
		bodyContent.insert ("plazo", plazo);
		bodyContent.insert ("frecuenciaDePagos", frecuenciaDePagos);
		bodyContent.insert ("fechaVencimiento", fechaVencimiento.toMSecsSinceEpoch());
		bodyContent.insert ("fechaDesembolso_1", fechaDesembolso_1.toMSecsSinceEpoch());
		bodyContent.insert ("montoDesembolso_1", montoDesembolso_1);
		bodyContent.insert ("empresaGrupo", empresaGrupo);
		bodyContent.insert ("entidadFinanciera", entidadFinanciera);
		if (ui.montoDesem_2->text() != "") {
			QDateTime fechaDesem_2 = ui.fechaDesem_2->dateTime ();
			double montoDesem_2 = ui.montoDesem_2->text ().toDouble ();
			bodyContent.insert ("fechaDesembolso_2", fechaDesem_2.toMSecsSinceEpoch());
			bodyContent.insert ("montoDesembolso_2", montoDesem_2);
		}
		if (ui.montoDesem_3->text() != "") {
			QDateTime fechaDesem_3 = ui.fechaDesem_3->dateTime ();
			double montoDesem_3 = ui.montoDesem_3->text ().toDouble ();
			bodyContent.insert ("fechaDesembolso_3", fechaDesem_3.toMSecsSinceEpoch());
			bodyContent.insert ("montoDesembolso_3", montoDesem_3);
		}
		if (ui.montoDesem_4->text() != "") {
			QDateTime fechaDesem_4 = ui.fechaDesem_4->dateTime ();
			double montoDesem_4 = ui.montoDesem_4->text ().toDouble ();
			bodyContent.insert ("fechaDesembolso_4", fechaDesem_4.toMSecsSinceEpoch());
			bodyContent.insert ("montoDesembolso_4", montoDesem_4);
		}
		if (ui.montoDesem_5->text() != "") {
			QDateTime fechaDesem_5 = ui.fechaDesem_5->dateTime ();
			double montoDesem_5 = ui.montoDesem_5->text ().toDouble ();
			bodyContent.insert ("fechaDesembolso_5", fechaDesem_5.toMSecsSinceEpoch());
			bodyContent.insert ("montoDesembolso_5", montoDesem_5);
		}


		body.setObject (bodyContent);
		nam->post (request, body.toJson ());
	}
}

void PlanDePagos::casoLineaDeCredito () {
	if (checkCasoLineaDeCredito ()) {
		// General info
		int codigoLinea_ID = lineasDeCredito[ui.codigoLineaCredito->text ()]["id"].toInt();
		int empresaGrupo_ID = listaEmpresas[ui.empresaGrupo->currentText ()].toInt ();
		int entidad_ID = listaEntidades[ui.nombreEntidad->currentText ()]["id"].toInt ();
		QString numeroContratoOperacion = ui.numeroContratoOperacion->text ();
		QDateTime fechaFirma_date = ui.fechaFirma->dateTime ();
		QString concepto = ui.concepto->text ();
		QString detalle = ui.detalle->text ();
		QString moneda = ui.moneda->currentText ();
		double monto = ui.monto->text ().toDouble ();
		QString tipoTasa = ui.tipoTasa->currentText ();
		double interesFijo = ui.interesFijo->text ().toDouble ();
		double interesVariable = ui.interesVariable->text ().toDouble ();
		int plazo = ui.plazo->value ();
		QDateTime fechaVencimiento_date = ui.fechaVencimiento->dateTime ();
		QDateTime fechaDesem_1_date = ui.fechaDesem_1->dateTime ();
		double montoDesem_1 = ui.montoDesem_1->text ().toDouble();
		QDateTime fechaDesem_2_date = ui.fechaDesem_2->dateTime ();
		double montoDesem_2 = ui.montoDesem_2->text ().toDouble();

		QNetworkAccessManager* nam = new QNetworkAccessManager (this);
		QNetworkRequest request;
		request.setUrl (QUrl (this->targetAddress + "/"));

	}
}

void PlanDePagos::casoLeasing () {
	QString empresaGrupo = ui.empresaGrupo->currentText (),
		entidad = ui.tipoEntidad->currentText (),
		nombreEmpresa = ui.nombreEntidad->currentText (),
		numeroContratoOperacion = ui.numeroContratoOperacion->text (),
		fechaFirma = ui.fechaFirma->date ().toString ("dd/MM/yyyy"),
		concepto = ui.concepto->text (),
		detalle = ui.detalle->text (),
		moneda = ui.moneda->currentText (),
		monto = ui.monto->text (),
		cuotaInicial = ui.cuotaInicial->text(),
		tipoTasa = ui.tipoTasa->currentText (),
		interesFijo = ui.interesFijo->text (),
		interesVariable = ui.interesVariable->text (),
		plazo = ui.plazo->text (),
		fechaVenc = ui.fechaVencimiento->date ().toString ("dd/MM/yyyy"),
		fechaDesem_1 = ui.fechaDesem_1->date ().toString ("dd/MM/yyyy"),
		montoDesem_1 = ui.montoDesem_1->text (),
		fechaDesem_2 = ui.fechaDesem_2->date ().toString ("dd/MM/yyyy"),
		montoDesem_2 = ui.montoDesem_2->text ();


	QString frecuencia = ui.frecuencia->currentText (),
		fechaPago = ui.fechaPago->date ().toString ("dd/MM/yyyy"),
		montoTotalPago = ui.pagoMonto->text (),
		pagoCapital = ui.pagoCapital->text (),
		pagoInteres = ui.pagoInteres->text (),
		pagoIva = ui.pagoIva->text ();

	int numeroCuota = ui.numeroCuota->value ();

	QMessageBox::critical (this, "Error", QString::fromLatin1 ("Caso Leasing no implementado"));
	ui.tipoDeOperacion->setFocus ();
}

void PlanDePagos::casoLeaseBack () {
	QString codigoLinea = ui.codigoLineaCredito->text (),
		empresaGrupo = ui.empresaGrupo->currentText (),
		entidad = ui.tipoEntidad->currentText (),
		nombreEmpresa = ui.nombreEntidad->currentText (),
		numeroContratoOperacion = ui.numeroContratoOperacion->text (),
		fechaFirma = ui.fechaFirma->date ().toString ("dd/MM/yyyy"),
		concepto = ui.concepto->text (),
		detalle = ui.detalle->text (),
		moneda = ui.moneda->currentText (),
		monto = ui.monto->text (),
		iva = ui.iva->text (),
		cuotaInicial = ui.cuotaInicial->text (),
		tipoTasa = ui.tipoTasa->currentText (),
		interesFijo = ui.interesFijo->text (),
		interesVariable = ui.interesVariable->text (),
		plazo = ui.plazo->text (),
		fechaVenc = ui.fechaVencimiento->date ().toString ("dd/MM/yyyy"),
		fechaDesem_1 = ui.fechaDesem_1->date ().toString ("dd/MM/yyyy"),
		montoDesem_1 = ui.montoDesem_1->text (),
		fechaDesem_2 = ui.fechaDesem_2->date ().toString ("dd/MM/yyyy"),
		montoDesem_2 = ui.montoDesem_2->text ();

	QString frecuencia = ui.frecuencia->currentText (),
		fechaPago = ui.fechaPago->date ().toString ("dd/MM/yyyy"),
		montoTotalPago = ui.pagoMonto->text (),
		pagoCapital = ui.pagoCapital->text (),
		pagoInteres = ui.pagoInteres->text (),
		pagoIva = ui.pagoIva->text ();

	int numeroCuota = ui.numeroCuota->value ();

	QMessageBox::critical (this, "Error", QString::fromLatin1 ("Caso Lease Back no implementado"));
	ui.tipoDeOperacion->setFocus ();
}

void PlanDePagos::casoSeguro (){
	QMessageBox::critical (this, "Error", QString::fromLatin1 ("Caso Seguro no implementado"));
	ui.tipoDeOperacion->setFocus ();
}

void PlanDePagos::casoCreditoSetup () {
	ui.iva->setEnabled (false);
	ui.pagoIva->setEnabled (false);
	ui.empresaGrupo->setEnabled (true);
	ui.nombreEntidad->setEnabled (true);
	ui.cuotaInicial->setEnabled (false);
	ui.codigoLineaCredito->setEnabled (false);
	ui.interesVariable->setEnabled (false);


	ui.codigoLineaCredito->setText ("");
	ui.cuotaInicial->setText ("");
	ui.pagoIva->setText ("");
	ui.iva->setText ("");
}

void PlanDePagos::casoLineaDeCreditoSetup () {
	loadLineasDeCredito (1);
	// Enable Codigo lineas de credito
	ui.codigoLineaCredito->setEnabled (true);
	// Disable and autofill with data from linea de crédito
	ui.empresaGrupo->setEnabled (false);
	ui.empresaGrupo->setEnabled (false);
	ui.nombreEntidad->setEnabled (true);
	// Disable
	ui.iva->setEnabled (false);
	ui.cuotaInicial->setEnabled (false);
	ui.pagoIva->setEnabled (false);

	ui.cuotaInicial->setText ("");
	ui.pagoIva->setText ("");
	ui.iva->setText ("");
}

void PlanDePagos::casoLeasingSetup () {
	ui.iva->setEnabled (false);
	ui.pagoIva->setEnabled (false);
	ui.empresaGrupo->setEnabled (true);
	ui.nombreEntidad->setEnabled (true);
	ui.cuotaInicial->setEnabled (true);
	ui.codigoLineaCredito->setEnabled (false);
	ui.interesVariable->setEnabled (false);

	ui.codigoLineaCredito->setText ("");
	ui.cuotaInicial->setText ("");
	ui.pagoIva->setText ("0.0");
	ui.iva->setText ("");
}

void PlanDePagos::casoLeaseBackSetup () {
	ui.iva->setEnabled (false);
	ui.pagoIva->setEnabled (false);
	ui.empresaGrupo->setEnabled (true);
	ui.nombreEntidad->setEnabled (true);
	ui.cuotaInicial->setEnabled (true);
	ui.codigoLineaCredito->setEnabled (false);
	ui.interesVariable->setEnabled (false);

	ui.codigoLineaCredito->setText ("");
	ui.cuotaInicial->setText ("");
	ui.pagoIva->setText ("0.0");
	ui.iva->setText ("0.0");
}

bool PlanDePagos::checkCasoCredito () {
	if (!planDataIsCorrect[2]) {
		// 3[2]  monto
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Es necesario que el monto de la operación sea mayor a cero"));
		ui.monto->setFocus ();
		return false;
	}
	if (!planDataIsCorrect[5]) {
		// 6[5]  interes fijo
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La tasa fija debe ser mayor a cero"));
		ui.interesFijo->setFocus ();
		return false;
	}
	if (!planDataIsCorrect[6]) {
		//7  [6]  interes variable
		if (ui.tipoTasa->currentText () != "Fijo") {
			QMessageBox::critical (this, "Error", QString::fromLatin1 ("La tasa variable debe ser mayor o igual a cero"));
			ui.interesVariable->setFocus ();
			return false;
		}
	}
	if (!planDataIsCorrect[7]) {
		//8  [7]  Monto Desem 1
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto del desembolso 1 debe ser mayor a cero"));
		ui.montoDesem_1->setFocus ();
		return false;
	}
	if (!planDataIsCorrect[8]) {
		//9  [8]  Monto Desem 2
		if (ui.montoDesem_2->text () != "") {
			QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto del desembolso 2 debe ser mayor a cero"));
			ui.montoDesem_2->setFocus ();
			return false;
		}
	}
	if (!planDataIsCorrect[9]) {
		//10  [9]  Monto Desem 3
		if (ui.montoDesem_3->text () != "") {
			QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto del desembolso 3 debe ser mayor a cero"));
			ui.montoDesem_2->setFocus ();
			return false;
		}
	}
	if (!planDataIsCorrect[10]) {
		//11  [10]  Monto Desem 4
		if (ui.montoDesem_4->text () != "") {
			QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto del desembolso 4 debe ser mayor a cero"));
			ui.montoDesem_2->setFocus ();
			return false;
		}
	}
	if (!planDataIsCorrect[11]) {
		//12  [11]  Monto Desem 5
		if (ui.montoDesem_5->text () != "") {
			QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto del desembolso 5 debe ser mayor a cero"));
			ui.montoDesem_2->setFocus ();
			return false;
		}
	}
	if (!planDataIsCorrect[12]) {
		//13[12]  Suma Montos Desem
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La suma de los desembolsos debe ser igual al monto de la operación"));
		ui.montoDesem_1->setFocus ();
		return false;
	}
	double interesFijo = ui.interesFijo->text ().toDouble ();
	if (interesFijo >= 20) {
		QMessageBox::StandardButton btn = QMessageBox::question (this, "Advertencia", QString::fromLatin1 ("La tasa de interés fijo es mayor o igual al 20%\n¿Desea Continuar?"));
		if (btn != QMessageBox::StandardButton::Yes) {
			return false;
		}
	}
	return true;
}

bool PlanDePagos::checkCasoLineaDeCredito () {
	if (!planDataIsCorrect[0]) {
		// 1[0]  codigo linea de credito no existe
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La línea de crédito no existe"));
		ui.codigoLineaCredito->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[2]) {
		// 3[2]  monto
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Es necesario que el monto de la operación sea mayor a cero"));
		ui.monto->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[5]) {
		// 6[5]  interes fijo
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La tasa fija debe ser mayor a cero"));
		ui.interesFijo->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[6]) {
		//7  [6]  interes variable
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La tasa variable debe ser mayor o igual a cero"));
		ui.interesVariable->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[7]) {
		//8  [7]  Monto Desem 1
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto del desembolso debe ser mayor a cero"));
		ui.montoDesem_1->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[8]) {
		//9  [8]  Monto Desem 2
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto del desembolso debe ser mayor a cero"));
		ui.montoDesem_2->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[9]) {
		//10[9]  Suma Montos Desem
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La suma de los desembolsos debe ser igual al monto de la operación"));
		ui.montoDesem_1->setFocus ();
		return false;
	}
	else {
		return true;
	}
}

bool PlanDePagos::checkCasoLeasing () {
	if (!planDataIsCorrect[0]) {
		// 1[0]  codigo linea de credito no existe
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La línea de crédito no existe"));
		ui.codigoLineaCredito->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[2]) {
		// 3[2]  monto
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Es necesario que el monto de la operación sea mayor a cero"));
		ui.monto->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[3]) {
		// 4[3]  iva
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Es necesario que el IVA sea mayor a cero"));
		ui.iva->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[4]) {
		// 5[4]  cuota inicial
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La cuota inicial debe ser mayor a cero"));
		ui.cuotaInicial->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[5]) {
		// 6[5]  interes fijo
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La tasa fija debe ser mayor a cero"));
		ui.interesFijo->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[6]) {
		//7  [6]  interes variable
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La tasa variable debe ser mayor o igual a cero"));
		ui.interesVariable->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[7]) {
		//8  [7]  Monto Desem 1
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto del desembolso debe ser mayor a cero"));
		ui.montoDesem_1->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[8]) {
		//9  [8]  Monto Desem 2
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto del desembolso debe ser mayor a cero"));
		ui.montoDesem_2->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[9]) {
		//10[9]  Suma Montos Desem
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La suma de los desembolsos debe ser igual al monto de la operación"));
		ui.montoDesem_1->setFocus ();
		return false;
	}
	else {
		return true;
	}
}

bool PlanDePagos::checkCasoLeaseBack () {
	if (!planDataIsCorrect[0]) {
		// 1[0]  codigo linea de credito no existe
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La línea de crédito no existe"));
		ui.codigoLineaCredito->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[2]) {
		// 3[2]  monto
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Es necesario que el monto de la operación sea mayor a cero"));
		ui.monto->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[3]) {
		// 4[3]  iva
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Es necesario que el IVA sea mayor a cero"));
		ui.iva->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[4]) {
		// 5[4]  cuota inicial
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La cuota inicial debe ser mayor a cero"));
		ui.cuotaInicial->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[5]) {
		// 6[5]  interes fijo
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La tasa fija debe ser mayor a cero"));
		ui.interesFijo->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[6]) {
		//7  [6]  interes variable
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La tasa variable debe ser mayor o igual a cero"));
		ui.interesVariable->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[7]) {
		//8  [7]  Monto Desem 1
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto del desembolso debe ser mayor a cero"));
		ui.montoDesem_1->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[8]) {
		//9  [8]  Monto Desem 2
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto del desembolso debe ser mayor a cero"));
		ui.montoDesem_2->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[9]) {
		//10[9]  Suma Montos Desem
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La suma de los desembolsos debe ser igual al monto de la operación"));
		ui.montoDesem_1->setFocus ();
		return false;
	}
	else {
		return true;
	}
}

bool PlanDePagos::checkCasoSeguro () {
	if (!planDataIsCorrect[0]) {
		// 1[0]  codigo linea de credito no existe
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La línea de crédito no existe"));
		ui.codigoLineaCredito->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[2]) {
		// 3[2]  monto
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Es necesario que el monto de la operación sea mayor a cero"));
		ui.monto->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[3]) {
		// 4[3]  iva
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Es necesario que el IVA sea mayor a cero"));
		ui.iva->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[4]) {
		// 5[4]  cuota inicial
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La cuota inicial debe ser mayor a cero"));
		ui.cuotaInicial->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[5]) {
		// 6[5]  interes fijo
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La tasa fija debe ser mayor a cero"));
		ui.interesFijo->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[6]) {
		//7  [6]  interes variable
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La tasa variable debe ser mayor o igual a cero"));
		ui.interesVariable->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[7]) {
		//8  [7]  Monto Desem 1
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto del desembolso debe ser mayor a cero"));
		ui.montoDesem_1->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[8]) {
		//9  [8]  Monto Desem 2
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto del desembolso debe ser mayor a cero"));
		ui.montoDesem_2->setFocus ();
		return false;
	}
	else if (!planDataIsCorrect[9]) {
		//10[9]  Suma Montos Desem
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La suma de los desembolsos debe ser igual al monto de la operación"));
		ui.montoDesem_1->setFocus ();
		return false;
	}
	else {
		return true;
	}
}

void PlanDePagos::onMonedaChanged (QString moneda) {
	if (moneda == "Bolivianos (BOB)") {
		ui.label_9->setText ("Monto (Bs)");
		ui.label_16->setText ("Monto desembolso 1 (Bs)");
		ui.label_17->setText ("Monto desembolso 2 (Bs)");
		ui.label_20->setText ("Monto total del pago (Bs)");
		ui.label_21->setText ("Pago de capital (Bs)");
		ui.label_22->setText (QString::fromLatin1("Pago de interés (Bs)"));
		ui.label_23->setText ("Pago de IVA (Bs)");
	}
	else {
		ui.label_9->setText ("Monto ($us)");
		ui.label_16->setText ("Monto desembolso 1 ($us)");
		ui.label_17->setText ("Monto desembolso 2 ($us)");
		ui.label_20->setText ("Monto total del pago ($us)");
		ui.label_21->setText ("Pago de capital ($us)");
		ui.label_22->setText (QString::fromLatin1("Pago de interés ($us)"));
		ui.label_23->setText ("Pago de IVA ($us)");
	}
}

bool PlanDePagos::checkCuota () {
	//	1[0] numero de cuota
	//	2[1] monto total del pago
	//	3[2] pago capital
	//	4[3] pago interes
	//	5[4] pago iva
	//	6[5] Suma de todos los pagos debe ser igual al monto total
	if (!cuotaDataIsCorrect[0]) {
		QMessageBox::critical (this, "Error", "El número de cuota no puede repetirse");
		return false;
	}
	if (!cuotaDataIsCorrect[1]) {
		QMessageBox::critical (this, "Error", "El monto del pago debe ser mayor a cero y menor o igual que el saldo");
		ui.pagoMonto->setFocus ();
		return false;
	}
	if (!cuotaDataIsCorrect[2]) {
		QMessageBox::critical (this, "Error", "El monto del capital debe ser mayor o igual que cero y menor o igual que el monto del pago");
		ui.pagoCapital->setFocus ();
		return false;
	}
	if (!cuotaDataIsCorrect[3]) {
		QMessageBox::critical (this, "Error", "El monto del interés debe ser mayor o igual que cero y menor o igual que el monto del pago");
		ui.pagoInteres->setFocus ();
		return false;
	}
	if (!cuotaDataIsCorrect[4]) {
		if (ui.tipoDeOperacion->currentText () == "Leasing" || ui.tipoDeOperacion->currentText () == "Lease Back") {
			QMessageBox::critical (this, "Error", "El monto del IVA debe ser mayor o igual que cero y menor o igual que el monto del pago");
			ui.pagoIva->setFocus ();
			return false;
		}
	}
	if (!cuotaDataIsCorrect[5]) {
		QMessageBox::critical (this, "Error", "La suma de los pagos debe ser igual al monto total del pago");
		ui.pagoMonto->setFocus ();
		return false;
	}
	return true;
}

void PlanDePagos::lockGeneraInfoEnableCuotaInfo () {
	ui.tipoDeOperacion->setEnabled (false);
	ui.codigoLineaCredito->setEnabled (false);
	ui.empresaGrupo->setEnabled (false);
	ui.nombreEntidad->setEnabled (false);
	ui.numeroContratoOperacion->setEnabled (false);
	ui.fechaFirma->setEnabled (false);
	ui.concepto->setEnabled (false);
	ui.detalle->setEnabled (false);
	ui.moneda->setEnabled (false);
	ui.monto->setEnabled (false);
	ui.iva->setEnabled (false);
	ui.cuotaInicial->setEnabled (false);
	ui.tipoTasa->setEnabled (false);
	ui.interesFijo->setEnabled (false);
	ui.interesVariable->setEnabled (false);
	ui.plazo->setEnabled (false);
	ui.frecuencia->setEnabled (false);
	ui.fechaVencimiento->setEnabled (false);
	ui.fechaDesem_1->setEnabled (false);
	ui.montoDesem_1->setEnabled (false);
	ui.fechaDesem_2->setEnabled (false);
	ui.montoDesem_2->setEnabled (false);
	ui.savePlan->setEnabled (false);

	ui.numeroCuota->setEnabled (false);
	ui.fechaPago->setEnabled (true);
	ui.pagoMonto->setEnabled (true);
	ui.pagoCapital->setEnabled (true);
	ui.pagoInteres->setEnabled (true);
	ui.saveCuota->setEnabled (true);
	ui.pagoIva->setEnabled (false);
}


void PlanDePagos::clearFields () {
	// Enable all fields
	ui.tipoDeOperacion->setEnabled (true);
	ui.codigoLineaCredito->setEnabled (true);
	ui.empresaGrupo->setEnabled (true);
	ui.nombreEntidad->setEnabled (true);
	ui.numeroContratoOperacion->setEnabled (true);
	ui.fechaFirma->setEnabled (true);
	ui.concepto->setEnabled (true);
	ui.detalle->setEnabled (true);
	ui.moneda->setEnabled (true);
	ui.monto->setEnabled (true);
	ui.iva->setEnabled (true);
	ui.cuotaInicial->setEnabled (true);
	ui.tipoTasa->setEnabled (true);
	ui.interesFijo->setEnabled (true);
	ui.interesVariable->setEnabled (true);
	ui.plazo->setEnabled (true);
	ui.frecuencia->setEnabled (true);
	ui.fechaVencimiento->setEnabled (true);
	ui.fechaDesem_1->setEnabled (true);
	ui.montoDesem_1->setEnabled (true);
	ui.fechaDesem_2->setEnabled (true);
	ui.montoDesem_2->setEnabled (true);
	ui.savePlan->setEnabled (true);

	ui.numeroCuota->setEnabled (false);
	ui.fechaPago->setEnabled (false);
	ui.pagoMonto->setEnabled (false);
	ui.pagoCapital->setEnabled (false);
	ui.pagoInteres->setEnabled (false);
	ui.saveCuota->setEnabled (false);
	//ui.pagoIva->setEnabled (false);

	// Set Crédito as initial state for next input
	ui.tipoDeOperacion->setCurrentText (QString::fromLatin1 ("Crédito"));

	// Erase everything
	ui.codigoLineaCredito->setText ("");
	ui.empresaGrupo->setCurrentIndex (0);
	ui.nombreEntidad->setCurrentIndex (0);
	ui.numeroContratoOperacion->setText ("");
	ui.fechaFirma->setDate(QDate::currentDate());
	ui.concepto->setText ("");
	ui.detalle->setText ("");
	ui.moneda->setCurrentIndex (0);
	ui.monto->setText ("");
	ui.iva->setText ("");
	ui.cuotaInicial->setText ("");
	ui.tipoTasa->setCurrentIndex (0);
	ui.interesFijo->setText ("");
	ui.interesVariable->setText ("");
	ui.plazo->setValue (0);
	ui.frecuencia->setCurrentIndex (0);
	ui.fechaVencimiento->setDate (QDate::currentDate ());
	ui.fechaDesem_1->setDate (QDate::currentDate ());
	ui.montoDesem_1->setText ("");
	ui.fechaDesem_2->setDate (QDate::currentDate ());
	ui.montoDesem_2->setText ("");

	ui.numeroCuota->setValue (1);
	ui.fechaPago->setDate (QDate::currentDate ().addMonths(1));
	ui.pagoMonto->setText ("");
	ui.pagoCapital->setText ("");
	ui.pagoInteres->setText ("");
	ui.pagoIva->setText ("");
}

void PlanDePagos::resetPlanValidators () {
	for (int i = 0; i < 13; i++) {
		planDataIsCorrect[i] = false;
	}
	this->planID = -1;
	this->totalPaid = 0;
}

void PlanDePagos::resetCuotaValidators () {
	for (int i = 0; i < 6; i++) {
		cuotaDataIsCorrect[i] = false;
	}
}

void PlanDePagos::onSaveCouta () {
	if (checkCuota ()) {
		QMessageBox::information (this, "Sin implementar", QString::fromLatin1 ("Cuota") + QString::number(this->planID));
		ui.numeroCuota->setValue (ui.numeroCuota->value () + 1);
	}
	else {
		QMessageBox::critical(this, "Error", "validation failed");
	}
}

void PlanDePagos::onSavePlan () {
	int tipoOperacion = ui.tipoDeOperacion->currentIndex ();

	switch (tipoOperacion) {
	case CasosPlanDePagos_enum::CasoCredito:
		casoCredito ();
		break;
	case CasosPlanDePagos_enum::CasoLineaDeCredito:
		casoLineaDeCredito ();
		break;
	case CasosPlanDePagos_enum::CasoLeasing:
		casoLeasing ();
		break;
	case CasosPlanDePagos_enum::CasoLeaseBack:
		casoLeaseBack ();
		break;
	case CasosPlanDePagos_enum::CasoSeguro:
		casoSeguro ();
		break;
	default:
		QMessageBox::information (this, "Sin implementar", QString::fromLatin1 ("No implementado aún"));
		break;
	}
}