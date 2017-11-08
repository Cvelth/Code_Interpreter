#pragma once
#include <QWidget>
class CodeWidget;
class QVBoxLayout;
class Editor : public QWidget {
	Q_OBJECT
private:
	CodeWidget *m_code_editor;
	QVBoxLayout *m_layout;
public:
	Editor(QWidget *parent = Q_NULLPTR);
	~Editor();
};