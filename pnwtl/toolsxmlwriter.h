#ifndef toolsxmlwriter_h__included
#define toolsxmlwriter_h__included

#include "include/genx/genx.h"
#include "include/pngenx.h"

class ToolsXMLWriter : public GenxXMLWriter
{
public:
	void writeTool(ToolDefinition* tool)
	{
		genxStartElement(m_eTool);
		addAttributeConvertUTF8(m_aName, tool->Name.c_str());
		addAttributeConvertUTF8(m_aCommand, tool->Command.c_str());
		addAttributeConvertUTF8(m_aFolder, tool->Folder.c_str());
		addAttributeConvertUTF8(m_aParams, tool->Params.c_str());
		addAttributeConvertUTF8(m_aParsePattern, tool->CustomParsePattern.c_str());
		
		// No UTF-8 Here...
		genxAddAttribute(m_aShortcut, (utf8)IntToTString(tool->Shortcut).c_str());
		genxAddAttribute(m_aFlags, (utf8)IntToTString(tool->GetFlags()).c_str());
		genxAddAttribute(m_aIndex, (utf8)IntToTString(tool->Index).c_str());
		genxEndElement(m_writer);
	}

	void beginSchemeTools()
	{
		genxStartElementLiteral(m_writer, NULL, u("schemetools"));
	}

	void endSchemeTools()
	{
		pop();
	}

	void beginScheme(LPCTSTR name)
	{
		genxStartElement(m_eScheme);
		genxAddAttribute(m_aName, u(name));
	}

	void endScheme()
	{
		pop();
	}

	void beginGlobal()
	{
		genxStartElementLiteral(m_writer, NULL, u("global"));
	}

	void endGlobal()
	{
		pop();
	}

	void beginAllProjects()
	{
		genxStartElementLiteral(m_writer, NULL, u("allProjects"));
	}

	void endAllProjects()
	{
		pop();
	}

	void beginProject(LPCTSTR id)
	{
		genxStartElement(m_eProject);
		genxAddAttribute(m_aPID, u(id));
	}

	void endProject()
	{
		pop();
	}

protected:
	virtual void initXmlBits()
	{
		genxStatus s;

		m_eScheme = genxDeclareElement(m_writer, NULL, u("scheme"), &s);
		m_eTool = genxDeclareElement(m_writer, NULL, u("tool"), &s);
		m_eProject = genxDeclareElement(m_writer, NULL, u("project"), &s);

		PREDECLARE_ATTRIBUTES()
			ATT("name", m_aName);
			ATT("command", m_aCommand);
			ATT("folder", m_aFolder);
			ATT("params", m_aParams);
			ATT("shortcut", m_aShortcut);
			ATT("parsepattern", m_aParsePattern);
			ATT("flags", m_aFlags);
			ATT("index", m_aIndex);
			ATT("projectid", m_aPID);
		END_ATTRIBUTES();
	}

protected:
	genxElement m_eScheme;
	genxElement m_eTool;
	genxElement m_eProject;
	genxAttribute m_aName;
	genxAttribute m_aCommand;
	genxAttribute m_aFolder;
	genxAttribute m_aParams;
	genxAttribute m_aShortcut;
	genxAttribute m_aParsePattern;
	genxAttribute m_aFlags;
	genxAttribute m_aIndex;
	genxAttribute m_aPID;
};

#endif // #ifndef toolsxmlwriter_h__included