#include "NumberInput.h"

NumberInput::NumberInput (QWidget* parent) : QLineEdit (parent) {
	connect (this, &QLineEdit::textChanged, this, &NumberInput::validateNumber);
	connect (this, &QLineEdit::editingFinished, this, &NumberInput::updateForPrecision);
	this->value = 0;
	previous = "0";
	precision = 2;
}

NumberInput::~NumberInput () {

}

void NumberInput::setValue (double value) {
	this->setText (QString::number (value, 'f', this->precision));
	this->value = this->text().toDouble ();
	if (this->previous.toDouble () != value) {
		emit valueChanged (this->value);
	}
	this->previous = QString::number (value);
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
			this->value = toEvaluate;
			if (QString::number(this->value) != this->previous) {
				emit valueChanged (this->value);
			}
			this->previous = QString::number (value);
		}
		else {
			this->setText (this->previous);
		}
	}
	else {
		this->value = 0;
		if (QString::number (this->value) != this->previous) {
			emit valueChanged (this->value);
		}
		this->previous = QString::number (value);
	}
}
