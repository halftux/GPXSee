#ifndef OPTDIALOG_H
#define OPTDIALOG_H

#include <QDialog>
#include "palette.h"
#include "units.h"

namespace Ui {
class optDialog;
}
struct Options {
	// Appearance
	Palette palette;
	int trackWidth;
	int routeWidth;
	Qt::PenStyle trackStyle;
	Qt::PenStyle routeStyle;
	int graphWidth;
	bool pathAntiAliasing;
	bool graphAntiAliasing;
	// POI
	int poiRadius;
	// System
	bool useOpenGL;
	// Print/Export
	bool printName;
	bool printDate;
	bool printDistance;
	bool printTime;
	bool printMovingTime;
	bool printItemCount;
	bool separateGraphPage;

	Units units;
};

class optDialog : public QDialog
{
	Q_OBJECT

public:
	explicit optDialog(Options *options, QWidget *parent = 0);
	~optDialog();

private slots:
	void on_pushButton_color_base_clicked();
public slots:
	void accept();

private:
	Options *_options;
	QColor _baseColor;
	Ui::optDialog *ui;

};

#endif // OPTDIALOG_H
