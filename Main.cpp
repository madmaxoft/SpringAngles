#include "MainWindow.hpp"

#include <QApplication>
#include <QFile>





int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	// If there's a file arg, open the file:
	bool wasFirstSkipped = false;
	for (const auto & a: QApplication::arguments())
	{
		if (!wasFirstSkipped)
		{
			wasFirstSkipped = true;
			continue;
		}
		if (QFile::exists(a))
		{
			w.fileOpenByName(a);
		}
	}

	w.showMaximized();
	return a.exec();
}
