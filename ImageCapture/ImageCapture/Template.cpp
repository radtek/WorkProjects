#include "Template.h"
#include <QtGlobal>
#include <QDir>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

Template::Template(void)
{
}

Template::~Template(void)
{
}

QStringList Template::AllTemplates(void)
{
	QStringList templates;
	QDir dir(TEMPLATES_DIRECTORY_NAME);
	QString dir_name = dir.absolutePath();
	if (dir.exists()) {
		QStringList filters;
		filters << "*" << TEMPLATES_FILE_EXTENSION;
		dir.setFilter(QDir::Files);
		dir.setNameFilters(filters);
		templates = dir.entryList();
		for (int i = 0; i < templates.size(); i++) {
			templates[i] = dir.absolutePath() + "/" + templates[i];
		}
	}
	return templates;
}

QString Template::TemplateName(QString &template_path)
{
	QFile file(template_path);
	QString name = "";
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QXmlStreamReader xml(&file);
		QXmlStreamAttributes attr;
		QXmlStreamReader::TokenType token;
		QString str;
		while(xml.readNextStartElement()) {
			if (xml.name() == "Template") {
				name = xml.attributes().value("Name").toString();
			}
		}	
		file.close();
	}
	return name; 
}

QString Template::TemplateDescription(QString &template_path)
{
	QFile file(template_path);
	QString description = "";
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QXmlStreamReader xml(&file);
		QXmlStreamAttributes attr;
		QXmlStreamReader::TokenType token;
		QString str;
		while(xml.readNextStartElement()) {
			if (xml.name() == "Template") {
				description = xml.attributes().value("Description").toString();
			}
		}	
		file.close();
	}
	return description; 
}

bool Template::Initialize(const QString &template_path)
{
	QString test;
	QFile file(template_path);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QXmlStreamReader xml(&file);
		while(xml.readNextStartElement()) {
			test = xml.name().toString();
			if (xml.name() == "Template") {
				name = xml.attributes().value("Name").toString();
				description = xml.attributes().value("Description").toString();
			} else if (xml.name() == "Buttons") {
				while(xml.readNextStartElement()) {
					test = xml.name().toString();
					if (xml.name() == "Button") {
						BUTTON *button = new BUTTON;
						button->name = xml.attributes().value("Name").toString();
						button->command = xml.attributes().value("Command").toString();
						if ((button->name != "")&&(button->command != "")) {
							buttons.push_back(button);
						}
					}
					xml.skipCurrentElement();
				}
			} else if (xml.name() == "Sections") {
				while(xml.readNextStartElement()) {
					test = xml.name().toString();
					if (xml.name() == "Section") {
						SECTION *section = new SECTION;
						section->name = xml.attributes().value("Name").toString();
						sections.push_back(section);
					}
					xml.skipCurrentElement();
				}
			} else {
				xml.skipCurrentElement();
			}
		}
		file_path = template_path;
		file.close();
		return true;
	}
	return false; 
}

void Template::AddPicture(const QString &section, const QString &picture)
{
	foreach (SECTION *s, sections) {
		if (s->name == section) {
			s->pictures.push_back(picture);
		}
	}
}
