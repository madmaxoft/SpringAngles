#pragma once

#include <QDialog>




// fwd:
namespace Ui {
class PointCoordsDlg;
}





/** Dialog for asking the user for a point's coords. */
class PointCoordsDlg:
	public QDialog
{
	Q_OBJECT

	using Super = QDialog;


public:

	/** Shows the dialog with the specified values prefilled.
	Returns the values the user provided, or nullopt if the user cancelled. */
	static std::optional<QPointF> ask(
		QWidget * aParent,
		QPointF aCoords
	);


private:

	/** The Qt-managed UI. */
	std::unique_ptr<Ui::PointCoordsDlg> mUI;

	explicit PointCoordsDlg(QWidget * aParent, QPointF aCoords);
	~PointCoordsDlg();

	/** Returns the coordinates currently entered by the user. */
	QPointF coords() const;
};
