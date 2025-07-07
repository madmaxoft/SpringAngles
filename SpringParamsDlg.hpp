#pragma once

#include <QDialog>





// fwd:
namespace Ui {
class SpringParamsDlg;
}





struct SpringParams
{
	double mLength;
	double mForce;
};





class SpringParamsDlg:
	public QDialog
{
	Q_OBJECT

	using Super = QDialog;


public:

	/** Shows the dialog and returns thevalues the user provides. */
	static std::optional<SpringParams> ask(QWidget * aParent, bool aIsFixedLength, double aLength);


private:

	/** The Qt-managed UI. */
	std::unique_ptr<Ui::SpringParamsDlg> mUI;


	explicit SpringParamsDlg(QWidget * aParent, bool aIsFixedLength, double aLength);
	~SpringParamsDlg();

	double length() const;
	double force() const;
};
