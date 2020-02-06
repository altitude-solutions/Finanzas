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
			ui.label_31->setText ("Monto ($us)");
		}
	});

	// ===============================================
	// Set fecha firma as fecha vencimiento minimum date
	// ===============================================
	connect (ui.fechaFirma, &QDateEdit::dateChanged, this, [&](QDate date) {
		ui.fechaVencimiento->setMinimumDate (date);
	});

	// Set validator to false
	dataIsCorrect = new bool[2];
	resetValidarors ();
}

LineaDeCredito::~LineaDeCredito() {
	delete[] dataIsCorrect;
}

// On tab selected slot
// Use it to setup the current tab
void LineaDeCredito::onTabSelected () {
	// Tab setup
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

// save linea de credito
void LineaDeCredito::onSaveClicked () {
	if ( check() ) {
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
		request.setUrl (QUrl (targetAddress + "/lineaDeCredito"));
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
		nam->post (request, body.toJson ());

		ui.saveButton->setEnabled (false);
	}
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
	ui.tableWidget->setColumnCount (8);
	QStringList headers;
	// Set column names
	headers << QString::fromLatin1 ("Fecha Firma") << QString::fromLatin1("Fecha Vencimiento") << QString::fromLatin1 ("Código Línea de Crédito") << QString::fromLatin1 ("Nombre Entidad") 
			<< QString::fromLatin1 ("Entidad") << QString::fromLatin1 ("Empresa Grupo") << QString::fromLatin1 ("Moneda") << QString::fromLatin1 ("Monto");
	ui.tableWidget->setHorizontalHeaderLabels (headers);
	// Set column width to a proper size	// TODO: Dynamic sizes
	ui.tableWidget->setColumnWidth (0, 220);
	ui.tableWidget->setColumnWidth (1, 220);
	ui.tableWidget->setColumnWidth (2, 220);
	ui.tableWidget->setColumnWidth (3, 290);
	ui.tableWidget->setColumnWidth (4, 185);
	ui.tableWidget->setColumnWidth (5, 243);
	ui.tableWidget->setColumnWidth (6, 200);
	ui.tableWidget->setColumnWidth (7, 250);

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
		QDateTime date = QDateTime::fromMSecsSinceEpoch(row.toObject ().value ("fechaFirma").toVariant().toLongLong() );
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 0, new QTableWidgetItem ( date.toString("dd/MM/yyyy") ));
		//ui.tableWidget->item (ui.tableWidget->rowCount () - 1, 0)->setFlags (Qt::NoItemFlags);
		QDateTime date2 = QDateTime::fromMSecsSinceEpoch(row.toObject ().value ("fechaVencimiento").toVariant().toLongLong() );
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 1, new QTableWidgetItem ( date2.toString("dd/MM/yyyy") ));
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 2, new QTableWidgetItem ( row.toObject ().value ("codigo").toString() ) );
		//ui.tableWidget->item (ui.tableWidget->rowCount () - 1, 2)->setTextAlignment (Qt::AlignCenter);
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 3, new QTableWidgetItem ( row.toObject ().value ("entidades_financiera").toObject().value("nombreEntidad").toString()));
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 4, new QTableWidgetItem ( row.toObject ().value ("entidades_financiera").toObject ().value ("tipos_de_entidad").toObject().value("tipoDeEntidad").toString() ) );
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 5, new QTableWidgetItem ( row.toObject ().value ("empresas_grupo").toObject().value ("empresa").toString () ) );
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 6, new QTableWidgetItem ( row.toObject ().value ("moneda").toString() ) );
		ui.tableWidget->setItem (ui.tableWidget->rowCount () - 1, 7, new QTableWidgetItem ( QString::number( row.toObject ().value ("monto").toDouble(), 'g', 15 ) ) );

		// TODO: color the ones that have reached its limit
		if (!row.toObject ().value ("estado").toBool ()) {
			for (int col = 0; col < 7; col++) {
				ui.tableWidget->item (ui.tableWidget->rowCount() -1 , col)->setBackgroundColor ("#ff0000");
				ui.tableWidget->item (ui.tableWidget->rowCount() -1 , col)->setTextColor("#ffffff");
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
	else if (!dataIsCorrect[1]) {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("Es necesario que el monto sea mayor a cero"));
		ui.monto->setFocus ();
		ui.monto->selectAll ();
		return false;
	}
	else {
		return true;
	}
}

void LineaDeCredito::clearFields () {
	ui.codigoLinea->setText ("");
	ui.monto->setText ("");
	ui.codigoLinea->setFocus ();
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
		foreach (QJsonValue entidad, okJson.object().value("entidades").toArray() ) {
			ui.nombreEntidad->addItem ( entidad.toObject().value("nombreEntidad").toString() );
			QHash<QString, QString> current;
			current.insert ("id", QString::number( entidad.toObject ().value ("id").toInt () ) );
			current.insert ("nombreEntidad", entidad.toObject ().value ("nombreEntidad").toString ());
			current.insert ("tipoDeEntidad", QString::number (entidad.toObject ().value ("tipos_de_entidad").toObject ().value ("id").toInt ()));
			listaEntidades.insert (entidad.toObject ().value ("nombreEntidad").toString (), current);
		}
		reply->deleteLater ();
	});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/entidadFinanciera?status=1"));

	request.setRawHeader ("token", this->token.toUtf8());
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
			listaTiposEntidades.insert (QString::number (entidad.toObject ().value ("id").toInt ()), entidad.toObject ().value ("tipoDeEntidad").toString () );
		}
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
			listaEmpresas.insert (entidad.toObject ().value ("empresa").toString(), QString::number( entidad.toObject ().value ("id").toInt ()));
		}
		reply->deleteLater ();
	});
	QNetworkRequest request;
	request.setUrl (QUrl (targetAddress + "/empresas?status=1"));

	request.setRawHeader ("token", this->token.toUtf8 ());
	request.setRawHeader ("Content-Type", "application/json");
	nam->get (request);
}