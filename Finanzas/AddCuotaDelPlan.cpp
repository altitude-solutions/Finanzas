#include "AddCuotaDelPlan.h"

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


AddCuotaDelPlan::AddCuotaDelPlan (QWidget* parent): QDialog (parent) {
	ui.setupUi (this);
	this->editing = false;
	this->editing_ID = 0;
	currentDue = nullptr;

	connect (ui.cancelButton, &QPushButton::clicked, this, &AddCuotaDelPlan::reject);
	connect (ui.addButton, &QPushButton::clicked, this, &AddCuotaDelPlan::onSaveClicked);

	connect (ui.pagoCapital, &QLineEdit::returnPressed, ui.addButton, &QPushButton::click);
	connect (ui.pagoInteres, &QLineEdit::returnPressed, ui.addButton, &QPushButton::click);

	//connect (ui.pagoMonto, &NumberInput::valueChanged, this, &AddCuotaDelPlan::ivaAutofill);
}

AddCuotaDelPlan::~AddCuotaDelPlan () {
	delete currentDue;
}

void AddCuotaDelPlan::setValidationParams (QString targetURL, QString token, Operacion* op, CuotasPlanDePagos *lastDue, bool editing, CuotasPlanDePagos *due) {
	delete currentDue;
	// Set auth data
	this->targetURL = targetURL;
	this->token = token;
	// Set op type
	operationType = op->getOperationType ();
	// set operation id
	this->parentOp_ID = op->getID ();
	if (lastDue == nullptr) {
		ui.fechaPago->setMinimumDate (op->getSignDate ());
		int addedMonths = 0;
		switch (op->getFrequency ()) {
		case OperacionesFinancieras::FrecuenciaDePagos::Mensual:
			addedMonths = 1;
			break;
		case OperacionesFinancieras::FrecuenciaDePagos::Bimensual:
			addedMonths = 2;
			break;
		case OperacionesFinancieras::FrecuenciaDePagos::Trimestral:
			addedMonths = 3;
			break;
		case OperacionesFinancieras::FrecuenciaDePagos::Semestral:
			addedMonths = 6;
			break;
		case OperacionesFinancieras::FrecuenciaDePagos::Anual:
			addedMonths = 12;
			break;
		case OperacionesFinancieras::FrecuenciaDePagos::NONE:
			break;
		}
		ui.fechaPago->setDate (op->getSignDate ().addMonths (addedMonths));
		ui.numeroCuota->setValue (1);
	}
	else {
		ui.fechaPago->setMinimumDate (lastDue->getDueDate ());
		int addedMonths = 0;
		switch (op->getFrequency ()) {
		case OperacionesFinancieras::FrecuenciaDePagos::Mensual:
			addedMonths = 1;
			break;
		case OperacionesFinancieras::FrecuenciaDePagos::Bimensual:
			addedMonths = 2;
			break;
		case OperacionesFinancieras::FrecuenciaDePagos::Trimestral:
			addedMonths = 3;
			break;
		case OperacionesFinancieras::FrecuenciaDePagos::Semestral:
			addedMonths = 6;
			break;
		case OperacionesFinancieras::FrecuenciaDePagos::Anual:
			addedMonths = 12;
			break;
		case OperacionesFinancieras::FrecuenciaDePagos::NONE:
			break;
		}
		ui.fechaPago->setDate (lastDue->getDueDate ().addMonths (addedMonths));
		ui.numeroCuota->setValue (lastDue->getDueNumber () + 1);
	}
	ui.pagoMonto->setFocus ();
	this->editing = editing;
	if (editing) {
		this->editing_ID = due != nullptr ? due->getID () : 0;
		ui.numeroCuota->setValue (due != nullptr ? due->getDueNumber () : 1);
		ui.fechaPago->setDate (due != nullptr ? due->getDueDate () : op->getSignDate ());
		ui.pagoMonto->setValue (due != nullptr ? due->getTotal () : 0);
		ui.pagoCapital->setValue (due != nullptr ? due->getCapital () : 0);
		ui.pagoInteres->setValue (due != nullptr ? due->getInterest () : 0);
		ui.pagoIva->setValue (due != nullptr ? due->getIva () : 0);		
		
		this->currentDue = new CuotasPlanDePagos (this);

		currentDue->setID (due->getID ());

		qDebug () << "current due id inner =" << due->getID ();
		qDebug () << "second ID =" << currentDue->getID();

		ui.label->setText (QString::fromLatin1 ("Editar Cuota"));
		setWindowTitle (QString::fromLatin1 (("Editar Cuota")));
		ui.addButton->setText (QString::fromLatin1 ("Actualizar"));
	}
	else {
		this->currentDue = new CuotasPlanDePagos (this);
		ui.label->setText (QString::fromLatin1 ("Añadir Cuota"));
		setWindowTitle (QString::fromLatin1 (("Añadir Cuota")));
		ui.addButton->setText (QString::fromLatin1 ("Registrar"));
	}

	connect (currentDue, &CuotasPlanDePagos::notifyValidationStatus, this, &AddCuotaDelPlan::catchError);

	connect (ui.pagoCapital, &NumberInput::valueChanged, this, &AddCuotaDelPlan::capitalChanged);

	//connect (ui.pagoInteres, &NumberInput::valueChanged, this, &AddCuotaDelPlan::interestChanged);
}

void AddCuotaDelPlan::onSaveClicked () {
	ui.addButton->setEnabled (false);
	if (currentDue != nullptr) {
		double diff = ui.pagoMonto->getValue () - (ui.pagoCapital->getValue () + ui.pagoInteres->getValue () + ui.pagoIva->getValue ());
		if (abs (diff) < 1e-9) {
			if (ui.pagoMonto->getValue () <= 0) {
				QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto total del pago debe ser mayor a cero"));
				ui.pagoMonto->setFocus ();
				ui.addButton->setEnabled (true);
				return;
			}
			if (ui.pagoCapital->getValue () <= 0) {
				QMessageBox::critical (this, "Error", QString::fromLatin1 ("El pago de capital debe ser mayor a cero"));
				ui.pagoMonto->setFocus ();
				ui.addButton->setEnabled (true);
				return;
			}
			if (ui.pagoInteres->getValue () <= 0) {
				QMessageBox::critical (this, "Error", QString::fromLatin1 ("El pago de interés debe ser mayor a cero"));
				ui.pagoMonto->setFocus ();
				ui.addButton->setEnabled (true);
				return;
			}
			if (ui.pagoIva->getValue () <= 0 && (this->operationType == OperacionesFinancieras::TiposDeOperacion::CasoLeasing || this->operationType == OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack) ) {
				QMessageBox::critical (this, "Error", QString::fromLatin1 ("El pago de IVA debe ser mayor a cero"));
				ui.pagoMonto->setFocus ();
				ui.addButton->setEnabled (true);
				return;
			}
			currentDue->setDueNumber (ui.numeroCuota->value ());
			currentDue->setDueDate (ui.fechaPago->date ());
			currentDue->setTotal (ui.pagoMonto->getValue ());
			currentDue->setCapital (ui.pagoCapital->getValue ());
			currentDue->setInterest (ui.pagoInteres->getValue ());
			currentDue->setIva (ui.pagoIva->getValue ());
			
			currentDue->setParentID (this->parentOp_ID);

			if (this->editing) {
				currentDue->update (this->targetURL, this->token);
			}
			else {
				currentDue->save (this->targetURL, this->token);
			}
			//currentDue->deleteLater ();
		}
		else {
			QMessageBox::critical (this, "Error", "La suma de los pagos debe ser igual al monto total del pago");
			ui.addButton->setEnabled (true);
		}
	}
}

void AddCuotaDelPlan::ivaAutofill (double ammount) {
	//if (this->operationType == OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack || this->operationType == OperacionesFinancieras::TiposDeOperacion::CasoLeasing) {
	//	ui.pagoIva->setValue (0.13 * ammount);
	//}
}

void AddCuotaDelPlan::catchError (DueValidationError errorCode, QString message) {
	ui.addButton->setEnabled (true);
	if (currentDue != nullptr) {
		switch (errorCode) {
		case DueValidationError::SERVER_SIDE_ERROR:
			QMessageBox::critical (this, QString::fromLatin1("Error"), message);
			break;
		case DueValidationError::NO_ERROR:
			QMessageBox::information (this, QString::fromLatin1 ("Éxito"), QString::fromLatin1 ("Guardado con éxito"));
			emit this->accept ();
			break;
		}
	}
}

void AddCuotaDelPlan::capitalChanged (double capital) {
	if (this->operationType == OperacionesFinancieras::TiposDeOperacion::CasoLeasing || this->operationType == OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack) {
		ui.pagoIva->setValue (0.13 * capital / 0.87);
	}
	ui.pagoInteres->setValue (ui.pagoMonto->getValue () - capital - ui.pagoIva->getValue ());
}

void AddCuotaDelPlan::interestChanged (double interest) {
	//ui.pagoIva->setValue (ui.pagoMonto->getValue () - interest - ui.pagoCapital->getValue ());

	//ui.pagoCapital->setValue (ui.pagoMonto->getValue () - interest - ui.pagoIva->getValue ());
}
