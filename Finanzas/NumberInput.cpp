#include "NumberInput.h"

NumberInput::NumberInput (QWidget* parent) : QLineEdit (parent) {
	connect (this, &QLineEdit::textChanged, this, &NumberInput::validateNumber);
	connect (this, &QLineEdit::editingFinished, this, &NumberInput::updateForPrecision);
	this->value = -1;
	previous = "-1";
	precision = 2;
}

NumberInput::~NumberInput () {

}

void NumberInput::setValue (double value) {
	if(QString::number(value, 'f',  this->precision) !=  QString::number(this->value, 'f', this->precision)) {
		this->setText (QString::number (value, 'f', this->precision));
	}
	//this->value = this->text().toDouble ();
	//if (this->previous.toDouble () != value) {
		//emit valueChanged (this->value);
	//}
	//this->previous = QString::number (value);
}

double NumberInput::getValue () {
	return this->value;
}

void NumberInput::setPrecision (int prec) {
	if (prec > 0) {
		this->precision = prec;;
	}
}

int NumberInput::getPrecision () {
	return this->precision;
}

void NumberInput::updateForPrecision () {
	this->setText (QString::number (this->value, 'f', this->precision));
	this->value = QString::number (this->value, 'f', this->precision).toDouble ();
}

void NumberInput::validateNumber (QString data) {
	if (data != "") {
		bool isOk = true;
		double toEvaluate = data.toDouble (&isOk);
		if (isOk) {
			if (QString::number (toEvaluate, 'f', this->precision) != QString::number (this->value, 'f', this->precision)) {
				this->value = toEvaluate;
				emit valueChanged (this->value);
			}
			this->previous = QString::number (value, 'f', this->precision);
		}
		else {
			this->setText (this->previous);
		}
	}
	else {
		//if (this->value != this->previous) {
		if (QString::number (0, 'f', this->precision) != QString::number (this->value, 'f', this->precision)) {
			this->value = 0;
			emit valueChanged (this->value);
		}
		this->previous = QString::number (value, 'f', this->precision);
	}
}
