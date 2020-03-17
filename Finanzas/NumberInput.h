#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QString>

class NumberInput : public QLineEdit {
	Q_OBJECT

public:
	NumberInput(QWidget*parent);
	~NumberInput();

	void setValue (double value);
	double getValue ();

	void setPrecision (int prec);
	int getPrecision ();

signals:
	void valueChanged (double value);

private slots:
	void validateNumber (QString data);
	void updateForPrecision ();

private:
	double value;
	QString previous;

	int precision;
};
