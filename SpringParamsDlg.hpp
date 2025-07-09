#pragma once

#include <QDialog>





// fwd:
namespace Ui {
class SpringParamsDlg;
}





struct SpringParams
{
	double mIdealLength;
	double mForce;
};





class SpringParamsDlg:
	public QDialog
{
	Q_OBJECT

	using Super = QDialog;


public:

	/** Shows the dialog and returns thevalues the user provides. */
	static std::optional<SpringParams> ask(QWidget * aParent, double aIdealLength, double aForce);


private:

	/** The Qt-managed UI. */
	std::unique_ptr<Ui::SpringParamsDlg> mUI;


	explicit SpringParamsDlg(QWidget * aParent, double aIdealLength, double aForce);
	~SpringParamsDlg();

	double idealLength() const;
	double force() const;
};
