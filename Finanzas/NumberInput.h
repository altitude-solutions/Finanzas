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

signals:
	void valueChanged (double value);

private slots:
	void validateNumber (QString data);

private:
	double value;
	QString previous;
};
