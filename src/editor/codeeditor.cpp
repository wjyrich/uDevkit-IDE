#include "codeeditor.h"

#include <QBoxLayout>
#include <QFile>
#include <QDebug>
#include <QApplication>
#include <QFileInfo>

#include "edbee/edbee.h"
#include "edbee/io/textdocumentserializer.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textgrammar.h"
#include "edbee/models/texteditorconfig.h"
#include "edbee/texteditorcontroller.h"

bool CodeEditor::initialized = false;

CodeEditor::CodeEditor(QWidget *parent) : Editor(parent)
{
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);

    if (!initialized)
    {
        // get the edbee instance
        edbee::Edbee* tm = edbee::Edbee::instance();

        // configure your paths
        tm->setKeyMapPath( QApplication::applicationDirPath()+"/../contrib/edbee-data/keymaps/");
        tm->setGrammarPath(  QApplication::applicationDirPath()+"/../contrib/edbee-data/syntaxfiles/" );
        tm->setThemePath( QApplication::applicationDirPath()+"/../contrib/edbee-data/themes/" );

        // initialize the library
        tm->init();
        tm->autoShutDownOnAppExit();

        initialized = true;
    }

    _editorWidget =  new edbee::TextEditorWidget();
    QFont font = _editorWidget->config()->font();
    font.setFamily("monospace");
    font.setStyleHint(QFont::Monospace);
    font.setPixelSize(13);
    _editorWidget->config()->setFont(font);
    _editorWidget->config()->setThemeName("RtIDE");

    layout->addWidget(_editorWidget);
    setLayout(layout);
}

bool CodeEditor::modified() const
{
    return !_editorWidget->textDocument()->isPersisted();
}

int CodeEditor::openFile(const QString &filePath)
{
    QFile file (filePath);
    QFileInfo info(file);
    if (!info.isReadable() || !info.isFile())
        return -1;
    _editorWidget->textDocument()->setText("");

    edbee::TextDocumentSerializer serializer( _editorWidget->textDocument() );
    if( !serializer.load( &file ) )
        qDebug() << QString("Error opening file!\n%1").arg(serializer.errorString());

    edbee::TextGrammarManager* grammarManager = edbee::Edbee::instance()->grammarManager();
    edbee::TextGrammar* grammar = grammarManager->detectGrammarWithFilename( filePath );
    _editorWidget->textDocument()->setLanguageGrammar( grammar );

    setFilePath(filePath);

    return 0;
}
