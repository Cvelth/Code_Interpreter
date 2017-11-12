#include "Editor.hpp"
#include "CodeWidget.hpp"
#include <QLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QSplitter>
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
	m_buttons = new QHBoxLayout();
	m_new = new QPushButton("New");
	m_buttons->addWidget(m_new, 1);
	m_open = new QPushButton("Open");
	m_buttons->addWidget(m_open, 1);
	m_save = new QPushButton("Save");
	m_buttons->addWidget(m_save, 1);
	m_saveAs = new QPushButton("Save as");
	m_buttons->addWidget(m_saveAs, 1);
	m_buttons->addStretch(3);
	m_compile = new QPushButton("Compile");
	m_buttons->addWidget(m_compile, 1);

	m_code_editor = new CodeWidget();
	m_result_editor = new CodeWidget();
	m_result_editor->hide();
	m_result_editor->setReadOnly(true);

	m_editor = new QSplitter();
	m_editor->addWidget(m_code_editor);
	m_editor->addWidget(m_result_editor);

	m_upper_layout = new QVBoxLayout();
	m_upper_layout->addLayout(m_buttons);
	m_upper_layout->addWidget(m_editor);
	m_upper_widget = new QWidget();
	m_upper_widget->setLayout(m_upper_layout);

	m_error_widget = new QPlainTextEdit();

	m_splitter = new QSplitter(Qt::Orientation::Vertical);
	m_splitter->addWidget(m_upper_widget);
	m_splitter->addWidget(m_error_widget);
	m_splitter->setStretchFactor(0, 5);
	m_splitter->setStretchFactor(1, 2);

	m_main_layout = new QVBoxLayout();
	m_main_layout->addWidget(m_splitter);	
	setLayout(m_main_layout);

	resize(1280, 720);
	setFont(QFont("Consolas", 15));
	clear();

	connect(m_new, &QPushButton::clicked, this, &Editor::clear);
	connect(m_open, &QPushButton::clicked, this, &Editor::open);
	connect(m_save, &QPushButton::clicked, this, &Editor::save);
	connect(m_saveAs, &QPushButton::clicked, this, &Editor::saveAs);
	connect(m_compile, &QPushButton::clicked, this, &Editor::compile);
}
Editor::~Editor() {
	delete m_new;
	delete m_open;
	delete m_save;
	delete m_saveAs;
	delete m_compile;

	delete m_buttons;
	delete m_upper_layout;
	delete m_main_layout;

	delete m_code_editor;
	delete m_result_editor;
	delete m_editor;

	delete m_error_widget;

	delete m_upper_widget;
	delete m_splitter;

	if (m_current_file) delete m_current_file;
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
	m_current_file->close();
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
#include "..\Interpreter\Interpreter.hpp"
void Editor::compile() {
	try {
		m_error_widget->clear();
		m_error_widget->appendPlainText("Compilation has started...");

		m_error_widget->appendPlainText("Lexical Analysis has started...");
		auto tokens = lexical_analysis(m_code_editor->getSource().toStdString());
		m_error_widget->appendPlainText("Lexical Analysis was successful...");

		m_error_widget->appendPlainText("Syntax Analysis has started...");
		auto syntax = syntax_analysis(tokens);
		m_error_widget->appendPlainText("Syntax Analysis was successful...");

		m_error_widget->appendPlainText("Semantic Analysis has started...");
		auto result = semantic_analysis(syntax);
		m_error_widget->appendPlainText("Semantic Analysis was successful...");

		m_error_widget->appendPlainText("Code Recreation has started...");
		auto code = code_recreation(syntax, result);
		m_error_widget->appendPlainText("Code Recreation was successful...");

		m_result_editor->setPlainText(QString::fromStdString(code));
		m_result_editor->show();

		m_error_widget->appendPlainText("Compilation was successful...");
	} catch (std::exception &e) {
		m_error_widget->appendPlainText(QString("\t") + e.what());
		m_error_widget->appendPlainText("Error has occured. Terminating compilation...");
	}
}