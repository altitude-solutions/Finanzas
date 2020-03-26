#include "PagosEfectivos.h"

// Network imports
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

// Json imports
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

// Other imports
#include <QMessageBox>


// clipboard to excel
#include <QClipboard>
#include <QApplication>
#include <QAction>
#include <QKeySequence>


PagosEfectivos::PagosEfectivos (QWidget* parent) : QWidget (parent) {
	ui.setupUi (this);
	// ==================================================================
	// Setup filtering
	// ==================================================================
	connect (ui.search, &QLineEdit::textChanged, this, &PagosEfectivos::searchPlan);
	planesDePagoModel = new QStringListModel (this);
	ui.planesList->setModel (planesDePagoModel);
	ui.planesList->setEditTriggers (NULL);
	ui.plannedFee->setEditTriggers (NULL);
	ui.effectiveFee->setEditTriggers (NULL);

	connect (ui.planesList, &QListView::doubleClicked, this, [&](QModelIndex index) {
		this->currentPlan = planesDePagoData[ui.planesList->model ()->itemData (index)[0].toString ()];
		loadSelectedPlan (planesDePagoData[ui.planesList->model ()->itemData (index)[0].toString ()]);
	});

	setPlanTableHeaders ();
	ui.plannedFee->verticalHeader ()->hide ();
	ui.effectiveFee->verticalHeader ()->hide ();

	// Not loaded yet
	planLoaded = false;
	// ==================================================================
	// Add effective Cuota
	// ==================================================================
	connect (ui.addCuota, &QPushButton::clicked, this, [&]() {
		if (planLoaded) {
			OperacionesFinancieras::TiposDeOperacion caso = OperacionesFinancieras::MapOperationString (ui.tipoOperacion->text ());

			AddCuotaEfectiva addWindow (this);
			if (ui.effectiveFee->rowCount () == 0) {
				addWindow.setWindowData (this->targetAddress, this->token, 1, QDate::fromString (ui.fechaFirma->text (), "dd/MM/yyyy"), caso, OperacionesFinancieras::MapFrecuenciaString (ui.frecuencia->text ()), this->currentPlan, QDate::currentDate ());
			}
			else {
				addWindow.setWindowData (this->targetAddress, this->token, ui.effectiveFee->item (ui.effectiveFee->rowCount () - 1, 0)->text ().toInt ()+1, QDate::fromString (ui.effectiveFee->item (ui.effectiveFee->rowCount () - 1, 1)->text (), "dd/MM/yyyy"), caso, OperacionesFinancieras::MapFrecuenciaString (ui.frecuencia->text ()), this->currentPlan, QDate::currentDate ());
			}

			connect (&addWindow, &AddCuotaEfectiva::accepted, this, [&]() {
				loadSelectedPlan (this->currentPlan);
			});

			addWindow.setModal (true);
			addWindow.exec ();
			addWindow.deleteLater ();
		}
		else {
			QMessageBox::warning (this, "Error", QString::fromLatin1 ("Por favor seleccione un plan de pagos"));
			ui.planesList->setFocus ();
		}
		});

	// ==================================================================
	// Edit effective Cuota
	// ==================================================================
	connect (ui.editCuota, &QPushButton::clicked, this, [&]() {
		if (planLoaded) {
			if (ui.effectiveFee->selectedItems ().length () > 0) {
				int row = ui.effectiveFee->selectedItems ().at (0)->row ();
				int id = ui.effectiveFee->item (row, 9)->text ().toInt ();
				QMessageBox::StandardButton answer = QMessageBox::question (this, QString::fromLatin1 ("Actualizar"), QString::fromLatin1 ("¿Actualizar la cuota \"") + ui.effectiveFee->item (row, 0)->text () + QString::fromLatin1 ("\" de la operación \"") + ui.numeroContrato->text () + "\"?");
				if (answer == QMessageBox::StandardButton::Yes) {
					OperacionesFinancieras::TiposDeOperacion caso = OperacionesFinancieras::TiposDeOperacion::CasoCredito;

					if (ui.tipoOperacion->text () == QString::fromLatin1 ("Crédito")) {
						caso = OperacionesFinancieras::TiposDeOperacion::CasoCredito;
					}
					if (ui.tipoOperacion->text () == QString::fromLatin1 ("Operación de Línea de Crédito")) {
						caso = OperacionesFinancieras::TiposDeOperacion::CasoLineaDeCredito;
					}
					if (ui.tipoOperacion->text () == QString::fromLatin1 ("Leasing")) {
						caso = OperacionesFinancieras::TiposDeOperacion::CasoLeasing;
					}
					if (ui.tipoOperacion->text () == QString::fromLatin1 ("Lease Back")) {
						caso = OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack;
					}
					if (ui.tipoOperacion->text () == QString::fromLatin1 ("Seguro")) {
						caso = OperacionesFinancieras::TiposDeOperacion::CasoSeguro;
					}

					AddCuotaEfectiva addWindow (this);
					addWindow.setWindowData (this->targetAddress, this->token, ui.effectiveFee->item (row, 0)->text ().toInt (), QDate::fromString (ui.fechaFirma->text (), "dd/MM/yyyy"), caso, OperacionesFinancieras::MapFrecuenciaString (ui.frecuencia->text ()), this->currentPlan, QDate::fromString (ui.effectiveFee->item (row, 1)->text (), "dd/MM/yyyy"), ui.effectiveFee->item (row, 2)->text ().toDouble (), ui.effectiveFee->item (row, 3)->text ().toDouble (), ui.effectiveFee->item (row, 4)->text ().toDouble (), ui.effectiveFee->item (row, 5)->text ().toDouble (), ui.effectiveFee->item (row, 9)->text ().toInt (), true);

					connect (&addWindow, &AddCuotaEfectiva::accepted, this, [&]() {
						loadSelectedPlan (this->currentPlan);
					});

					addWindow.setModal (true);
					addWindow.exec ();

					addWindow.deleteLater ();
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
	});

	// ==================================================================
	// Delete effective Cuota
	// ==================================================================
	connect (ui.deleteCuota, &QPushButton::clicked, this, [&]() {
		if (planLoaded) {
			if (ui.effectiveFee->selectedItems ().length () > 0) {
				int row = ui.effectiveFee->selectedItems ().at (0)->row ();
				int id = ui.effectiveFee->item (row, 9)->text ().toInt ();
				QMessageBox::StandardButton answer = QMessageBox::question (this, QString::fromLatin1 ("Eliminar"), QString::fromLatin1 ("¿Eliminar la cuota \"") + ui.effectiveFee->item (row, 0)->text () + QString::fromLatin1("\" de la operación \"") + ui.numeroContrato->text() + "\"?");
				if (answer == QMessageBox::StandardButton::Yes) {
					QNetworkAccessManager* nam = new QNetworkAccessManager (this);
					connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
						if (reply->error ()) {
							QMessageBox::critical (this, "Error", QString::fromStdString ("No se pudo eliminar la Cuota"));
						}
						else {
							QMessageBox::information (this, QString::fromLatin1 ("Éxito"), QString::fromLatin1 ("Cuota eliminada con éxito"));
						}
						loadSelectedPlan (this->currentPlan);
						reply->deleteLater ();
					});
					QNetworkRequest request;
					request.setUrl (QUrl (this->targetAddress + "/cuotaEfectiva/" + QString::number (id)));
					request.setRawHeader ("token", this->token.toUtf8 ());
					nam->deleteResource (request);
				}
			}
			else {
				QMessageBox::warning (this, QString::fromLatin1 ("Cuota sin seleccionar"), QString::fromLatin1 ("Por favor seleccione la cuota que desea borrar"));
			}
		}
		else {
			QMessageBox::warning (this, "Error", QString::fromLatin1 ("Por favor seleccione un plan de pagos"));
			ui.planesList->setFocus ();
		}
	});

	setupTableClipboard ();
}

PagosEfectivos::~PagosEfectivos(){

}

void PagosEfectivos::setAuthData (QString address, QString token, QString userName) {
	this->targetAddress = address;
	this->token = token;
	this->userName = userName;

	loadEmpresasGrupo ();
	loadEntidadesFinancieras ();
	loadPlanesData ();
}

// On Tab selected
// Use it to setup current tab
void PagosEfectivos::onTabSelected () {
	
	// Tab setup
	loadEmpresasGrupo ();
	loadEntidadesFinancieras ();
	loadPlanesData ();
}

void PagosEfectivos::searchPlan (QString filter) {
	QString query = "";
	if (filter != "") {
		query = "&q="+filter;
	}
	loadPlanesData (query);
}

void PagosEfectivos::loadSelectedPlan (int id) {
	planLoaded = false;
	QNetworkAccessManager* nam = new QNetworkAccessManager (this);
	connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
		QByteArray resBin = reply->readAll ();
		if (reply->error ()) {
			QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
			QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
			return;
		}
		QJsonDocument okJson = QJsonDocument::fromJson (resBin);
		
		ui.empresa->setText (okJson.object ().value ("planDePagos").toObject ().value ("empresas_grupo").toObject ().value ("empresa").toString ());
		ui.entidad->setText (okJson.object ().value ("planDePagos").toObject ().value ("entidades_financiera").toObject ().value ("nombreEntidad").toString ());
		ui.tipoEntidad->setText (okJson.object ().value ("planDePagos").toObject ().value ("entidades_financiera").toObject ().value ("tipos_de_entidad").toObject ().value ("tipoDeEntidad").toString ());
		ui.tipoOperacion->setText (okJson.object ().value ("planDePagos").toObject ().value ("tipoOperacion").toString ());
		ui.numeroContrato->setText (okJson.object ().value ("planDePagos").toObject ().value ("numeroDeContratoOperacion").toString ());
		ui.fechaFirma->setText (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaFirma").toVariant ().toLongLong ()).toString ("dd/MM/yyyy"));
		ui.moneda->setText (okJson.object ().value ("planDePagos").toObject ().value ("moneda").toString ());
		ui.monto->setText (QString::number (okJson.object ().value ("planDePagos").toObject ().value ("monto").toDouble (), 'f', 2));

		if (!okJson.object ().value ("planDePagos").toObject ().value ("iva").isNull ()) {
			ui.iva->setText (QString::number(okJson.object ().value ("planDePagos").toObject ().value ("iva").toDouble (), 'f', 2));
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
		ui.tipoTasa->setText (okJson.object ().value ("planDePagos").toObject ().value ("tipoDeTasa").toString ());
		ui.interesFijo->setText (QString::number(okJson.object ().value ("planDePagos").toObject ().value ("interesFijo").toDouble(), 'f', 2) + "%");

		if (!okJson.object ().value ("planDePagos").toObject ().value ("interesVariable").isNull ()) {
			ui.interesVariable->setText (QString::number (okJson.object ().value ("planDePagos").toObject ().value ("interesVariable").toDouble (), 'f', 2) + "%");
		}
		else {
			ui.interesVariable->setText ("-");
		}
		ui.frecuencia->setText (okJson.object ().value ("planDePagos").toObject ().value ("frecuenciaDePagos").toString ());
		ui.plazo->setText (QString::number(okJson.object ().value ("planDePagos").toObject ().value ("plazo").toInt()) + " meses");
		ui.fechaVencimiento->setText (QDateTime::fromMSecsSinceEpoch (okJson.object ().value ("planDePagos").toObject ().value ("fechaVencimiento").toVariant ().toLongLong ()).toString ("dd/MM/yyyy"));

		ui.concepto->setText (okJson.object ().value ("planDePagos").toObject ().value ("concepto").toString ());
		ui.detalle->setText (okJson.object ().value ("planDePagos").toObject ().value ("detalle").toString ());

		ui.plannedFee->setRowCount (0);
		
		double saldoCapital = okJson.object ().value ("planDePagos").toObject ().value ("monto").toDouble ();
		double saldoIva = okJson.object ().value ("planDePagos").toObject ().value ("iva").toDouble ();

		foreach (QJsonValue cuota, okJson.object ().value ("planDePagos").toObject ().value ("cuotasPlan").toArray ()) {
			ui.plannedFee->insertRow (ui.plannedFee->rowCount ());

			ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 0, new QTableWidgetItem (QString::number (cuota.toObject ().value ("numeroDeCuota").toInt ())));
			ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 1, new QTableWidgetItem (QDateTime::fromMSecsSinceEpoch (cuota.toObject ().value ("fechaDePago").toVariant ().toLongLong ()).toString ("dd/MM/yyyy")));
			ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 2, new QTableWidgetItem (QString::number (cuota.toObject ().value ("montoTotalDelPago").toDouble (), 'f', 2)));
			ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 3, new QTableWidgetItem (QString::number (cuota.toObject ().value ("pagoDeCapital").toDouble (), 'f', 2)));
			ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 4, new QTableWidgetItem (QString::number (cuota.toObject ().value ("pagoDeInteres").toDouble (), 'f', 2)));
			if (!cuota.toObject ().value ("pagoDeIva").isNull ()) {
				ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 5, new QTableWidgetItem (QString::number (cuota.toObject ().value ("pagoDeIva").toDouble (), 'f', 2)));
			}
			else {
				ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 5, new QTableWidgetItem ("0.00"));
			}
			
			saldoCapital -= cuota.toObject ().value ("pagoDeCapital").toDouble ();
			saldoIva -= cuota.toObject ().value ("pagoDeIva").toDouble ();

			//====================================== primera cuota caso Leasing ============================================================
			OperacionesFinancieras::TiposDeOperacion opera = OperacionesFinancieras::MapOperationString (okJson.object ().value ("planDePagos").toObject ().value ("tipoOperacion").toString ());
			if ((cuota.toObject ().value ("numeroDeCuota").toInt () == 1 && opera == OperacionesFinancieras::TiposDeOperacion::CasoLeasing) || (cuota.toObject ().value ("numeroDeCuota").toInt () == 0 && opera == OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack)) {
				saldoCapital -= okJson.object ().value ("planDePagos").toObject ().value ("iva").toDouble ();
			}

			ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 6, new QTableWidgetItem (QString::number (saldoCapital, 'f', 2)));
			ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 7, new QTableWidgetItem (QString::number (saldoIva, 'f', 2)));
			ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 8, new QTableWidgetItem (QString::number (saldoCapital + saldoIva, 'f', 2)));

			ui.plannedFee->setItem (ui.plannedFee->rowCount () - 1, 9, new QTableWidgetItem (QString::number (cuota.toObject ().value ("id").toInt ())));
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		ui.effectiveFee->setRowCount (0);

		saldoCapital = okJson.object ().value ("planDePagos").toObject ().value ("monto").toDouble ();
		saldoIva = okJson.object ().value ("planDePagos").toObject ().value ("iva").toDouble ();

		foreach (QJsonValue cuota, okJson.object ().value ("planDePagos").toObject ().value ("cuotasEfectivas").toArray ()) {
			ui.effectiveFee->insertRow (ui.effectiveFee->rowCount ());

			ui.effectiveFee->setItem (ui.effectiveFee->rowCount () - 1, 0, new QTableWidgetItem (QString::number (cuota.toObject ().value ("numeroDeCuota").toInt ())));
			ui.effectiveFee->setItem (ui.effectiveFee->rowCount () - 1, 1, new QTableWidgetItem (QDateTime::fromMSecsSinceEpoch (cuota.toObject ().value ("fechaDePago").toVariant ().toLongLong ()).toString ("dd/MM/yyyy")));
			ui.effectiveFee->setItem (ui.effectiveFee->rowCount () - 1, 2, new QTableWidgetItem (QString::number (cuota.toObject ().value ("montoTotalDelPago").toDouble (), 'f', 2)));
			ui.effectiveFee->setItem (ui.effectiveFee->rowCount () - 1, 3, new QTableWidgetItem (QString::number (cuota.toObject ().value ("pagoDeCapital").toDouble (), 'f', 2)));
			ui.effectiveFee->setItem (ui.effectiveFee->rowCount () - 1, 4, new QTableWidgetItem (QString::number (cuota.toObject ().value ("pagoDeInteres").toDouble (), 'f', 2)));
			if (!cuota.toObject ().value ("pagoDeIva").isNull ()) {
				ui.effectiveFee->setItem (ui.effectiveFee->rowCount () - 1, 5, new QTableWidgetItem (QString::number (cuota.toObject ().value ("pagoDeIva").toDouble (), 'f', 2)));
			}
			else {
				ui.effectiveFee->setItem (ui.effectiveFee->rowCount () - 1, 5, new QTableWidgetItem ("0.00"));
			}

			saldoCapital -= cuota.toObject ().value ("pagoDeCapital").toDouble ();
			saldoIva -= cuota.toObject ().value ("pagoDeIva").toDouble ();

			//====================================== primera cuota caso Leasing ============================================================
			OperacionesFinancieras::TiposDeOperacion opera = OperacionesFinancieras::MapOperationString (okJson.object ().value ("planDePagos").toObject ().value ("tipoOperacion").toString ());
			if ((cuota.toObject ().value ("numeroDeCuota").toInt () == 1 && opera == OperacionesFinancieras::TiposDeOperacion::CasoLeasing) || (cuota.toObject ().value ("numeroDeCuota").toInt () == 0 && opera == OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack)) {
				saldoCapital -= okJson.object ().value ("planDePagos").toObject ().value ("iva").toDouble ();
			}

			ui.effectiveFee->setItem (ui.effectiveFee->rowCount () - 1, 6, new QTableWidgetItem (QString::number (saldoCapital, 'f', 2)));
			ui.effectiveFee->setItem (ui.effectiveFee->rowCount () - 1, 7, new QTableWidgetItem (QString::number (saldoIva, 'f', 2)));
			ui.effectiveFee->setItem (ui.effectiveFee->rowCount () - 1, 8, new QTableWidgetItem (QString::number (saldoCapital + saldoIva, 'f', 2)));

			ui.effectiveFee->setItem (ui.effectiveFee->rowCount () - 1, 9, new QTableWidgetItem (QString::number (cuota.toObject ().value ("id").toInt ())));
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		planLoaded = true;
		reply->deleteLater ();
	});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/planDePagos/" + QString::number(id) ));

	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}

void PagosEfectivos::loadEmpresasGrupo () {
	listaEmpresas.clear ();
	//ui.grupo->clear ();
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
			//ui.grupo->addItem (entidad.toObject ().value ("empresa").toString ());
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

void PagosEfectivos::loadEntidadesFinancieras () {
	listaEntidades.clear ();
	//ui.entidad->clear ();
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
			//ui.entidad->addItem (entidad.toObject ().value ("nombreEntidad").toString ());
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

void PagosEfectivos::loadPlanesData (QString query) {
	planesDePagoData.clear ();

	QNetworkAccessManager* nam = new QNetworkAccessManager (this);
	connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
		QByteArray resBin = reply->readAll ();
		if (reply->error ()) {
			QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
			QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
			return;
		}
		QJsonDocument okJson = QJsonDocument::fromJson (resBin);

		QStringList dataModel;
		foreach (QJsonValue entidad, okJson.object ().value ("planesDePago").toArray ()) {
			planesDePagoData.insert (entidad.toObject ().value ("numeroDeContratoOperacion").toString (), entidad.toObject ().value ("id").toInt ());
			dataModel.append (entidad.toObject ().value ("numeroDeContratoOperacion").toString ());
		}
		planesDePagoModel->setStringList (dataModel);


		reply->deleteLater ();
	});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/planDePagos?status=1" + query));

	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}

void PagosEfectivos::setPlanTableHeaders () {
	ui.plannedFee->setColumnCount (10);
	ui.effectiveFee->setColumnCount (10);

	QStringList headers = {
		"Cuota",
		"Fecha de Pago",
		"Monto total",
		"Pago Capital",
		QString::fromLatin1("Pago Interés"),
		"Pago IVA",
		"Saldo Capital",
		QString::fromLatin1("Crédito Fiscal"),
		"Saldo Capital Real",
		"id"
	};
	ui.plannedFee->setHorizontalHeaderLabels (headers);
	ui.effectiveFee->setHorizontalHeaderLabels (headers);

	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (0, QHeaderView::ResizeMode::ResizeToContents);	// cuota
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (1, QHeaderView::ResizeMode::Stretch);	// Fecha pago
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (2, QHeaderView::ResizeMode::Stretch);	// Monto
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (3, QHeaderView::ResizeMode::Stretch);	// capital
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (4, QHeaderView::ResizeMode::Stretch);	// interes
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (5, QHeaderView::ResizeMode::Stretch);	// iva
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (6, QHeaderView::ResizeMode::Stretch);	// saldo capital
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (7, QHeaderView::ResizeMode::Stretch);	// credito fiscal
	ui.plannedFee->horizontalHeader ()->setSectionResizeMode (8, QHeaderView::ResizeMode::Stretch);	// saldo real

	ui.plannedFee->hideColumn (9);
	//ui.plannedFee->horizontalHeader ()->setSectionResizeMode (9, QHeaderView::ResizeMode::Stretch);	// id

	ui.effectiveFee->horizontalHeader ()->setSectionResizeMode (0, QHeaderView::ResizeMode::ResizeToContents);	// cuota
	ui.effectiveFee->horizontalHeader ()->setSectionResizeMode (1, QHeaderView::ResizeMode::Stretch);	// Fecha pago
	ui.effectiveFee->horizontalHeader ()->setSectionResizeMode (2, QHeaderView::ResizeMode::Stretch);	// Monto
	ui.effectiveFee->horizontalHeader ()->setSectionResizeMode (3, QHeaderView::ResizeMode::Stretch);	// capital
	ui.effectiveFee->horizontalHeader ()->setSectionResizeMode (4, QHeaderView::ResizeMode::Stretch);	// interes
	ui.effectiveFee->horizontalHeader ()->setSectionResizeMode (5, QHeaderView::ResizeMode::Stretch);	// iva
	ui.effectiveFee->horizontalHeader ()->setSectionResizeMode (6, QHeaderView::ResizeMode::Stretch);	// saldo capital
	ui.effectiveFee->horizontalHeader ()->setSectionResizeMode (7, QHeaderView::ResizeMode::Stretch);	// credito  fiscal
	ui.effectiveFee->horizontalHeader ()->setSectionResizeMode (8, QHeaderView::ResizeMode::Stretch);	// saldo real

	ui.effectiveFee->hideColumn (9);
	//ui.effectiveFee->horizontalHeader ()->setSectionResizeMode (9, QHeaderView::ResizeMode::Stretch);	// id
}

void PagosEfectivos::setupTableClipboard () {
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
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	QAction* copyEffe = new QAction (ui.plannedFee);

	copyEffe->setShortcut (QKeySequence::Copy);
	copyEffe->setShortcutContext (Qt::ShortcutContext::WidgetShortcut);

	connect (copyEffe, &QAction::triggered, this, [&]() {
		if (ui.effectiveFee->selectedItems ().length () > 0) {
			QClipboard* clipboard = QApplication::clipboard ();

			int currentRow = ui.effectiveFee->selectedItems ().at (0)->row ();
			QString toClipboard = "";
			foreach (QTableWidgetItem * itm, ui.effectiveFee->selectedItems ()) {
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

	ui.effectiveFee->addAction (copyEffe);
}
