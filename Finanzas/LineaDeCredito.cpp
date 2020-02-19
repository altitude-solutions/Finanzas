#include "LineaDeCredito.h"
#include <QMessageBox>
#include <QFile>
#include <QDir>

// Json imports
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

// Network imports
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

// Debuging imports
#include <QDebug>


LineaDeCredito::LineaDeCredito(QWidget *parent): QWidget(parent) {
	ui.setupUi(this);
	// ===============================================
	// Listen to save Button clicked to save data
	// ===============================================
	connect (ui.monto, &QLineEdit::returnPressed, ui.saveButton, &QPushButton::click);
	connect (ui.saveButton, &QPushButton::clicked, this, &LineaDeCredito::onSaveClicked);

	// ===============================================
	// Setup date picker to use a calendar popup and it's current, maximum date 
	// ===============================================
	ui.fechaFirma->setCalendarPopup (true);
	ui.fechaFirma->setDate (QDate::currentDate ());
	ui.fechaFirma->setMaximumDate (QDate::currentDate());
	ui.fechaVencimiento->setMinimumDate (QDate::currentDate ());
	
	// ===============================================
	// Set date picker to use Bolivian date format
	// ===============================================
	ui.fechaFirma->setDisplayFormat ("dd/MM/yyyy");
	ui.fechaVencimiento->setDisplayFormat ("dd/MM/yyyy");

	// TODO: Debe existir otra forma de hacer que no se pueda editar
	ui.tableWidget->setEditTriggers ( NULL );

	// ===============================================
	// Populate table
	// ===============================================
	setTableHeaders ();
	
	// ===============================================
	// Set validaros according to each input
	// ===============================================
	connect (ui.codigoLinea, &QLineEdit::textChanged, this, [&](QString data) {
		if (data == "") {
			dataIsCorrect[0] = false;
		} else {
			dataIsCorrect[0] = true;
		}
	});
	connect (ui.monto, &QLineEdit::textChanged, this, [&](QString data) {
		bool isNumber = false;
		double ammount = data.toDouble (&isNumber);
		if (!isNumber) {
			dataIsCorrect[1] = false;
			data.truncate (data.length () - 1);
			ui.monto->setText (data);
			return;
		}
		if (ammount > 0) {
			dataIsCorrect[1] = true;
		}
		else {
			dataIsCorrect[1] = false;
		}
	});

	// ===============================================
	// Refresh tipo de entidad
	// ===============================================
	connect (ui.nombreEntidad, &QComboBox::currentTextChanged, this, [&](QString data) {
		ui.tipoEntidad->setCurrentText (listaTiposEntidades[listaEntidades[data]["tipoDeEntidad"]]);
	});

	// ===============================================
	// Refresh monto unit
	// ===============================================
	connect (ui.moneda, &QComboBox::currentTextChanged, this, [&](QString data) {
		if ( data == "Bolivianos (BOB)" ) {
			ui.label_31->setText ("Monto (Bs)");
		}
		else {
			if (data == QString::fromLatin1("Dólares (USD)")) {
				ui.label_31->setText ("Monto ($us)");
			}
			else {
				ui.label_31->setText ("Monto (-)");
			}
		}
	});

	// ===============================================
	// Set fecha firma as fecha vencimiento minimum date
	// ===============================================
	connect (ui.fechaFirma, &QDateEdit::dateChanged, this, [&](QDate date) {
		ui.fechaVencimiento->setMinimumDate (date);
	});

	// ===============================================
	// Cancel button handler to clear fields
	// ===============================================
	connect (ui.cancelButton, &QPushButton::clicked, this, &LineaDeCredito::onCancelClicked);

	// ===============================================
	// On Edit clicked
	// ===============================================
	connect (ui.editButton, &QPushButton::clicked, this, &LineaDeCredito::onEditClicked);

	// ===============================================
	// On Delete clicked
	// ===============================================
	connect (ui.deleteButton, &QPushButton::clicked, this, &LineaDeCredito::onDeleteClicked);



	// Set validator to false
	dataIsCorrect = new bool[2];
	resetValidarors ();

	editing = false;
	editingID = 0;
}

LineaDeCredito::~LineaDeCredito() {
	delete[] dataIsCorrect;
}

// Set authentication data
void LineaDeCredito::setAuthData (QString address, QString token, QString userName) {
	this->targetAddress = address;
	this->token = token;
	this->userName = userName;
	loadEntidadesFinancieras ();
	loadTiposDeEntidad ();
	loadEmpresasGrupo ();

	loadLineasDeCredito ();
}

// On tab selected slot
// Use it to setup the current tab
void LineaDeCredito::onTabSelected () {
	// Tab setup
}

// save linea de credito
void LineaDeCredito::onSaveClicked () {
	if ( check() ) {
		QMessageBox::StandardButton answer = QMessageBox::StandardButton::No;
		if (editing) {
			answer = QMessageBox::question (this, QString::fromLatin1 ("Actualizar"), QString::fromLatin1 ("¿Actualizar la línea de crédito \"") + lineasDeCredito[editingID].object ().value ("codigo").toString () + "\"?");
		}
		if (answer == QMessageBox::StandardButton::Yes || !editing) {
			QNetworkAccessManager* nam = new QNetworkAccessManager (this);
			connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
				QByteArray binReply = reply->readAll ();
				if (reply->error ()) {
					QJsonDocument errorJson = QJsonDocument::fromJson (binReply);
					if (errorJson.object ().value ("err").toObject ().contains ("message")) {
						QMessageBox::critical (this, "Error", QString::fromLatin1 (errorJson.object ().value ("err").toObject ().value ("message").toString ().toLatin1 ()));
					} else {
						QMessageBox::critical (this, "Error en base de datos", "Por favor enviar un reporte de error con una captura de pantalla de esta venta.\n" + QString::fromStdString (errorJson.toJson ().toStdString ()));
					}
					ui.saveButton->setEnabled (true);
					return;
				}
				// everything went right then i have a response with the new row of the table
				// FIXME: Should be other methond
				loadLineasDeCredito ();

				resetValidarors ();
				ui.saveButton->setEnabled (true);
				clearFields ();
				reply->deleteLater ();
			});
			QNetworkRequest request;
			if (editing) {
				request.setUrl (QUrl (targetAddress + "/lineaDeCredito/" + QString::number(editingID)));
			}
			else {
				request.setUrl (QUrl (targetAddress + "/lineaDeCredito"));
			}
			request.setRawHeader ("token", token.toUtf8 ());
			request.setRawHeader ("Content-Type", "application/json");

			QJsonDocument body;
			QJsonObject bodyContent;
			bodyContent.insert ("codigo", ui.codigoLinea->text ());
			bodyContent.insert ("moneda", ui.moneda->currentText ());
			QDateTime fecha = QDateTime (ui.fechaFirma->date ());
			bodyContent.insert ("fechaFirma", fecha.toMSecsSinceEpoch ());
			QDateTime fecha2 = QDateTime (ui.fechaVencimiento->date ());
			bodyContent.insert ("fechaVencimiento", fecha2.toMSecsSinceEpoch ());
			bodyContent.insert ("monto", ui.monto->text ().toDouble ());
			bodyContent.insert ("entidad", listaEntidades[ui.nombreEntidad->currentText ()]["id"].toInt ());
			bodyContent.insert ("empresaGrupo", listaEmpresas[ui.empresaGrupo->currentText ()].toInt ());

			body.setObject (bodyContent);
			if (editing) {
				nam->put (request, body.toJson ());
			}
			else {
				nam->post (request, body.toJson ());
			}
			ui.saveButton->setEnabled (false);
		}
	}
}

void LineaDeCredito::onCancelClicked () {
	clearFields ();
}

void LineaDeCredito::onEditClicked () {
	if (ui.tableWidget->selectedItems ().length () > 0) {
		int row = ui.tableWidget->selectedItems ().at (0)->row ();
		editingID = ui.tableWidget->item (row, 8)->text ().toInt ();
		editing = true;
		ui.codigoLinea->setText (ui.tableWidget->item (row, 2)->text ());
		ui.nombreEntidad->setCurrentText (ui.tableWidget->item (row, 3)->text ());
		ui.empresaGrupo->setCurrentText (ui.tableWidget->item (row, 5)->text ());
		ui.fechaFirma->setDate (QDate::fromString (ui.tableWidget->item (row, 0)->text (), "dd/MM/yyyy"));
		ui.moneda->setCurrentText (ui.tableWidget->item (row, 6)->text ());
		ui.fechaVencimiento->setDate (QDate::fromString (ui.tableWidget->item (row, 1)->text (), "dd/MM/yyyy"));
		ui.monto->setText (ui.tableWidget->item (row, 7)->text ());
	}
	else {
		QMessageBox::warning (this, QString::fromLatin1 ("Línea de crédito sin seleccionar"), QString::fromLatin1 ("Por favor seleccione la línea de crédito que desea editar"));
	}
}

void LineaDeCredito::onDeleteClicked () {
	if(ui.tableWidget->selectedItems().length() > 0) {
		int row = ui.tableWidget->selectedItems ().at (0)->row ();
		int id = ui.tableWidget->item (row, 8)->text ().toInt ();
		QMessageBox::StandardButton answer = QMessageBox::question (this, QString::fromLatin1("Eliminar"), QString::fromLatin1("¿Eliminar la línea de crédito \"") + ui.tableWidget->item(row,2)->text() + "\"?" );
		if (answer == QMessageBox::StandardButton::Yes) {
			QNetworkAccessManager* nam = new QNetworkAccessManager (this);
			connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
				if (reply->error ()) {
					QMessageBox::critical (this, "Error", QString::fromStdString ("No se pudo borrar la línea de crédito"));
				}
				else {
					QMessageBox::information (this, QString::fromLatin1("Éxito"), QString::fromLatin1 ("Línea de crédito borrada con éxito"));
				}
				loadLineasDeCredito ();
				reply->deleteLater ();
			});
			QNetworkRequest request;
			request.setUrl (QUrl (this->targetAddress + "/lineaDeCredito/" + QString::number(id) ));
			request.setRawHeader ("token", this->token.toUtf8 ());
			nam->deleteResource (request);
		}
	}
	else {
		QMessageBox::warning (this, QString::fromLatin1("Línea de crédito sin seleccionar"), QString::fromLatin1("Por favor seleccione la línea de crédito que desea borrar"));
	}
}

void LineaDeCredito::loadEntidadesFinancieras () {
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
			current.insert ("tipoDeEntidad", QString::number (entidad.toObject ().value ("tipos_de_entidad").toObject ().value ("id").toInt ()));
			listaEntidades.insert (entidad.toObject ().value ("nombreEntidad").toString (), current);
		}
		clearFields ();
		reply->deleteLater ();
	});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/entidadFinanciera?status=1"));

	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}

void LineaDeCredito::loadTiposDeEntidad () {
	listaTiposEntidades.clear ();
	ui.tipoEntidad->clear ();
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
		clearFields ();
		reply->deleteLater ();
	});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/tipo_entidad?status=1"));

	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}

void LineaDeCredito::loadEmpresasGrupo () {
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
		clearFields ();
		reply->deleteLater ();
	});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/empresas?status=1"));

	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}

void LineaDeCredito::loadLineasDeCredito () {
	QNetworkAccessManager* nam = new QNetworkAccessManager (this);
	// request callback
	connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
		QByteArray binReply = reply->readAll ();
		if (reply->error ()) {
			// if there is a server error
			QJsonDocument errorJson = QJsonDocument::fromJson (binReply);
			QMessageBox::critical (this, "Internal server error", "Por favor enviar un reporte de error con una captura de pantalla de esta venta.\n" + QString::fromStdString (errorJson.toJson ().toStdString ()) + QString::fromLatin1("\n\nSi necesita que los datos esten disponibles por favor reinicie la aplicación"));
			return;
		}
		QJsonDocument res = QJsonDocument::fromJson(binReply);
		refreshTable (res);
		clearFields ();
		reply->deleteLater ();
	});

	QNetworkRequest request;
	// FIXME: Check if everything is needed (no status or status 1)
	//request.setUrl (QUrl (targetAddress + "/lineaDeCredito"));
	request.setUrl (QUrl (targetAddress + "/lineaDeCredito?status=1"));

	request.setHeader (QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
	// setup authentication token
	request.setRawHeader ("token", token.toUtf8 ());

	nam->get(request);
}

void LineaDeCredito::setTableHeaders () {
	ui.tableWidget->setColumnCount (9);
	QStringList headers;
	// Set column names
	headers << QString::fromLatin1 ("Fecha Firma") << QString::fromLatin1("Fecha Vencimiento") << QString::fromLatin1 ("Código Línea de Crédito") << QString::fromLatin1 ("Nombre Entidad") 
			<< QString::fromLatin1 ("Entidad") << QString::fromLatin1 ("Empresa Grupo") << QString::fromLatin1 ("Moneda") << QString::fromLatin1 ("Monto");
	ui.tableWidget->setHorizontalHeaderLabels (headers);
	// Get table widget size
	QSize tableSize = ui.tableWidget->size ();

	double totalWidth = tableSize.width ();
	// Set column width to a proper size
	ui.tableWidget->horizontalHeader ()->setSectionResizeMode (0, QHeaderView::ResizeMode::Stretch);
	ui.tableWidget->horizontalHeader ()->setSectionResizeMode (1, QHeaderView::ResizeMode::Stretch);
	ui.tableWidget->horizontalHeader ()->setSectionResizeMode (2, QHeaderView::ResizeMode::Stretch);
	ui.tableWidget->horizontalHeader ()->setSectionResizeMode (3, QHeaderView::ResizeMode::Stretch);
	ui.tableWidget->horizontalHeader ()->setSectionResizeMode (4, QHeaderView::ResizeMode::Stretch);
	ui.tableWidget->horizontalHeader ()->setSectionResizeMode (5, QHeaderView::ResizeMode::Stretch);
	ui.tableWidget->horizontalHeader ()->setSectionResizeMode (6, QHeaderView::ResizeMode::Stretch);
	ui.tableWidget->horizontalHeader ()->setSectionResizeMode (7, QHeaderView::ResizeMode::Stretch);

	//ui.tableWidget->setColumnWidth (0, 0.13 * totalWidth ); // fecha firma
	//ui.tableWidget->setColumnWidth (1, 0.13 * totalWidth ); // fecha vencimiento
	//ui.tableWidget->setColumnWidth (2, 0.12 * totalWidth ); // codigo linea de credito
	//ui.tableWidget->setColumnWidth (3, 0.19 * totalWidth ); // nombre entidad
	//ui.tableWidget->setColumnWidth (4, 0.10 * totalWidth ); // tipo de entidad
	//ui.tableWidget->setColumnWidth (5, 0.10 * totalWidth ); // empresa grupo
	//ui.tableWidget->setColumnWidth (6, 0.08 * totalWidth ); // moneda
	//ui.tableWidget->setColumnWidth (7, 0.15 * totalWidth ); // monto
	//ui.tableWidget->setColumnWidth (8, 0);   // ID
	ui.tableWidget->hideColumn (8);

	// Remove row numbers
	ui.tableWidget->verticalHeader ()->hide ();
}

void LineaDeCredito::refreshTable (QJsonDocument data) {
	ui.tableWidget->clear ();
	ui.tableWidget->setRowCount (0);
	setTableHeaders ();

	//ui.tableWidget->setSortingEnabled (false);
	//ui.tableWidget->horizontalHeader ()->sortIndicatorOrder ();
	foreach (QJsonValue row, data.object ().value ("lineasDeCredito").toArray ()) {
		QJsonDocument current;
		current.setObject (row.toObject ());
		lineasDeCredito.insert (row.toObject ().value ("id").toInt (), current);

		ui.tableWidget->insertRow (ui.tableWidget->rowCount ());
		QDateTime date = QDateTime::fromMSecsSinceEpoch (row.toObject ().value ("fechaFirma").toVariant ().toLongLong ());
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 0, new QTableWidgetItem (date.toString ("dd/MM/yyyy")));
		//ui.tableWidget->item (ui.tableWidget->rowCount () - 1, 0)->setFlags (Qt::NoItemFlags);
		QDateTime date2 = QDateTime::fromMSecsSinceEpoch (row.toObject ().value ("fechaVencimiento").toVariant ().toLongLong ());
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 1, new QTableWidgetItem (date2.toString ("dd/MM/yyyy")));
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 2, new QTableWidgetItem (row.toObject ().value ("codigo").toString ()));
		//ui.tableWidget->item (ui.tableWidget->rowCount () - 1, 2)->setTextAlignment (Qt::AlignCenter);
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 3, new QTableWidgetItem (row.toObject ().value ("entidades_financiera").toObject ().value ("nombreEntidad").toString ()));
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 4, new QTableWidgetItem (row.toObject ().value ("entidades_financiera").toObject ().value ("tipos_de_entidad").toObject ().value ("tipoDeEntidad").toString ()));
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 5, new QTableWidgetItem (row.toObject ().value ("empresas_grupo").toObject ().value ("empresa").toString ()));
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 6, new QTableWidgetItem (row.toObject ().value ("moneda").toString ()));
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 7, new QTableWidgetItem (QString::number (row.toObject ().value ("monto").toDouble (), 'g', 15)));
		
		ui.tableWidget->item (ui.tableWidget->rowCount () - 1, 7)->setTextAlignment (Qt::AlignmentFlag::AlignRight);	// Align monto to the right

		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 8, new QTableWidgetItem (QString::number (row.toObject ().value ("id").toInt ())));

		// TODO: color the ones that have reached its limit
		if (!row.toObject ().value ("estado").toBool ()) {
			for (int col = 0; col < 7; col++) {
				ui.tableWidget->item (ui.tableWidget->rowCount () - 1, col)->setBackgroundColor ("#ff0000");
				ui.tableWidget->item (ui.tableWidget->rowCount () - 1, col)->setTextColor ("#ffffff");
			}
		}
	}
	//ui.tableWidget->setSortingEnabled (true);
}

void LineaDeCredito::resetValidarors () {
	for (int i = 0; i < 2; i++) {
		dataIsCorrect[i] = false;
	}
}

bool LineaDeCredito::check () {
	if (!dataIsCorrect[0]) {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El código de la línea de crédito es necesario"));
		ui.codigoLinea->setFocus ();
		ui.codigoLinea->selectAll ();
		return false;
	}
	if (ui.nombreEntidad->currentText () == "") {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Seleccione una entidad financiera"));
		ui.nombreEntidad->setFocus ();
		return false;
	}
	if (ui.empresaGrupo->currentText () == "") {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Seleccione una empresa"));
		ui.empresaGrupo->setFocus ();
		return false;
	}
	if (ui.moneda->currentText () == "") {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Seleccione una moneda"));
		ui.moneda->setFocus ();
		return false;
	}
	if (!dataIsCorrect[1]) {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Es necesario que el monto sea mayor a cero"));
		ui.monto->setFocus ();
		ui.monto->selectAll ();
		return false;
	}
	return true;
}

void LineaDeCredito::clearFields () {
	ui.codigoLinea->setText ("");
	ui.monto->setText ("");
	ui.label_31->setText ("Monto (-)");

	ui.nombreEntidad->setCurrentIndex(-1);
	ui.tipoEntidad->setCurrentIndex(-1);
	ui.empresaGrupo->setCurrentIndex(-1);
	ui.moneda->setCurrentIndex(-1);
	ui.fechaFirma->setDate (QDate::currentDate ());
	ui.fechaVencimiento->setDate (QDate::currentDate ());

	ui.codigoLinea->setFocus ();

	editing = false;
	editingID = 0;
}
