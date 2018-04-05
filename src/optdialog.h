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
	QColor waypointColor;
	QColor poiColor;
	int waypointSize;
	int poiSize;
	int graphWidth;
	bool pathAntiAliasing;
	bool graphAntiAliasing;
	// Data
	int elevationFilter;
	int speedFilter;
	int heartRateFilter;
	int cadenceFilter;
	int powerFilter;
	bool outlierEliminate;
	qreal pauseSpeed;
	int pauseInterval;
	// POI
	int poiRadius;
	// System
	bool useOpenGL;
	int pixmapCache;
	// Print/Export
	bool hiresPrint;
	bool printName;
	bool printDate;
	bool printDistance;
	bool printTime;
	bool printMovingTime;
	bool printItemCount;
	bool separateGraphPage;
	// Map
	int mapOpacity;
	QColor backgroundColor;

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
	void on_pushButton_color_waypoints_clicked();
	void on_pushButton_color_pois_clicked();
	void on_pushButton_bcolor_map_clicked();

public slots:
	void accept();

private:
	Options *_options;
	QColor _baseColor;
	QColor _backgroundColor;
	QColor _waypointColor;
	QColor _poiColor;
	Ui::optDialog *ui;

};

#endif // OPTDIALOG_H
