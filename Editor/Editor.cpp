#include "Editor.hpp"
#include "CodeWidget.hpp"
#include <QLayout>
Editor::Editor(QWidget *parent)	: QWidget(parent) {
	m_code_editor = new CodeWidget();
	m_layout = new QVBoxLayout();
	m_layout->addWidget(m_code_editor);
	setLayout(m_layout);
	resize(1280, 720);
}
Editor::~Editor() {
	delete m_code_editor;
}