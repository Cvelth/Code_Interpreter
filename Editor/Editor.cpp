#include "Editor.hpp"
#include "CodeWidget.hpp"
#include <QLayout>
#include <QPushButton>
#include <QMessageBox>
bool Editor::eventFilter(QObject *obj, QEvent *event) {
	if (event->type() == QEvent::KeyPress) {
		auto key = static_cast<QKeyEvent*>(event);
		if (key->key() == Qt::Key::Key_S && key->modifiers() == Qt::ControlModifier) {
			save();
			event->accept();
			return true;
		}
	}
	return QObject::eventFilter(obj, event);
}
Editor::Editor(QWidget *parent)	: QWidget(parent) {
	QHBoxLayout *m_buttons = new QHBoxLayout();
	QPushButton *m_new = new QPushButton("New");
	m_buttons->addWidget(m_new, 1);
	QPushButton *m_open = new QPushButton("Open");
	m_buttons->addWidget(m_open, 1);
	QPushButton *m_save = new QPushButton("Save");
	m_buttons->addWidget(m_save, 1);
	QPushButton *m_saveAs = new QPushButton("Save as");
	m_buttons->addWidget(m_saveAs, 1);
	m_buttons->addStretch(3);

	m_code_editor = new CodeWidget();
	m_layout = new QVBoxLayout();
	m_layout->addLayout(m_buttons);
	m_layout->addWidget(m_code_editor);
	setLayout(m_layout);

	resize(1280, 720);
	setFont(QFont("Consolas", 15));
	clear();

	connect(m_new, &QPushButton::clicked, this, &Editor::clear);
	connect(m_open, &QPushButton::clicked, this, &Editor::open);
	connect(m_save, &QPushButton::clicked, this, &Editor::save);
	connect(m_saveAs, &QPushButton::clicked, this, &Editor::saveAs);
}
Editor::~Editor() {
	delete m_new;
	//delete m_open;
	//delete m_save;
	//delete m_saveAs;

	delete m_code_editor;
}
void Editor::clear() {
	m_current_file = nullptr;
	m_code_editor->clear();
}
#include <QFile>
#include <QFileDialog>
void Editor::open() {
	auto filename = QFileDialog::getOpenFileName(this, tr("Open source file"), "../test_code/",
												 tr("Perl source files (*.plx *.pm);;C++ source files (*.hpp *.cpp);;"
													"C source files (*.h *.c);;Assembler source files (*.asm *.s)"));
	if (filename == "")
		return;
	m_current_file = new QFile(filename);
	if (!m_current_file->open(QIODevice::ReadOnly))
		QMessageBox(QMessageBox::Warning, "File cannot be opened", "Some error occured during file openning process", 
					QMessageBox::NoButton, this).exec();
	else
		m_code_editor->document()->setPlainText(m_current_file->readAll());
}
void Editor::save() {
	if (m_current_file) {
		if (!m_current_file->open(QIODevice::WriteOnly))
			QMessageBox(QMessageBox::Warning, "File cannot be opened", "Some error occured during file openning process",
						QMessageBox::NoButton, this).exec();
		else {
			m_current_file->write(m_code_editor->document()->toPlainText().toUtf8());
			m_current_file->flush();
			m_current_file->close();
		}
	} else
		saveAs();
}
void Editor::saveAs() {
	auto filename = QFileDialog::getSaveFileName(this, tr("Save source file"), "../test_code/",
												 tr("Perl source files (*.plx *.pm);;C++ source files (*.hpp *.cpp);;"
													"C source files (*.h *.c);;Assembler source files (*.asm *.s)"));
	if (filename == "")
		return;
	m_current_file = new QFile(filename);
	if (!m_current_file->open(QIODevice::WriteOnly))
		QMessageBox(QMessageBox::Warning, "File cannot be opened", "Some error occured during file openning process",
					QMessageBox::NoButton, this).exec();
	else {
		m_current_file->write(m_code_editor->document()->toPlainText().toUtf8());
		m_current_file->flush();
		m_current_file->close();
	}
}