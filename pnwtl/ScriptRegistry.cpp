/**
 * @file ScriptRegistry.cpp
 * @brief Script Registry
 * @author Simon Steele
 * @note Copyright (c) 2006 Simon Steele <s.steele@pnotepad.org>
 *
 * Programmers Notepad 2 : The license file (license.[txt|html]) describes 
 * the conditions under which this source may be modified / distributed.
 */

#include "stdafx.h"
#include "scriptregistry.h"

typedef std::map<tstring, tstring> tstringmap;

ScriptRegistry::ScriptRegistry()
{
	m_sink = NULL;
}

ScriptRegistry::~ScriptRegistry()
{
	clear();
}

void ScriptRegistry::Add(const char* group, Script* script)
{
	ScriptGroup* pGroup = getOrMakeGroup(group);
	pGroup->Add(script);

	if(m_sink)
		m_sink->OnScriptAdded(pGroup, script);
}

void ScriptRegistry::Remove(const char* group, Script* script)
{
	ScriptGroup* pGroup = getOrMakeGroup(group);
	pGroup->Remove(script);

	if(m_sink)
		m_sink->OnScriptRemoved(pGroup, script);
}

void ScriptRegistry::Add(const char* group, const char* name, const char* scriptref)
{
	ScriptGroup* pGroup = getOrMakeGroup(group);
	Script* theScript = pGroup->Add(name, scriptref);

	if(m_sink)
		m_sink->OnScriptAdded(pGroup, theScript);
}

ScriptGroup* ScriptRegistry::getOrMakeGroup(const char* group)
{
	ScriptGroup* pGroup(NULL);

	for(group_list_t::iterator i = m_groups.begin(); i != m_groups.end(); ++i)
	{
		if(_tcsicmp((*i)->GetName(), group) == 0)
		{
			pGroup = (*i);
			break;
		}
	}

	if(!pGroup)
	{
		pGroup = new ScriptGroup(group);
		m_groups.push_back(pGroup);
	}

	return pGroup;
}

void ScriptRegistry::Clear()
{
	clear();
}

const group_list_t& ScriptRegistry::GetGroups()
{
	return m_groups;
}

void ScriptRegistry::RegisterRunner(const char* id, extensions::IScriptRunner* runner)
{
	m_runners.insert(s_runner_map::value_type(tstring(id), runner));
}

void ScriptRegistry::RemoveRunner(const char* id)
{
	s_runner_map::iterator i = m_runners.find(tstring(id));
	if(i != m_runners.end())
		m_runners.erase(i);
}

extensions::IScriptRunner* ScriptRegistry::GetRunner(const char* id)
{
	s_runner_map::const_iterator i = m_runners.find(tstring(id));
	if(i != m_runners.end())
	{
		return (*i).second;
	}
	else
		return NULL;
}

void ScriptRegistry::EnableSchemeScripts(const char* scheme, const char* runnerId)
{
	m_scriptableSchemes.insert(tstringmap::value_type(tstring(scheme), tstring(runnerId)));
}

bool ScriptRegistry::SchemeScriptsEnabled(const char* scheme)
{
	return m_scriptableSchemes.find(tstring(scheme)) != m_scriptableSchemes.end();
}

bool ScriptRegistry::SchemeScriptsEnabled(const char* scheme, tstring& runner)
{
	tstringmap::const_iterator i = m_scriptableSchemes.find(tstring(scheme));
	if(i != m_scriptableSchemes.end())
	{
		runner = (*i).second;
		return true;
	}
	return false;
}

void ScriptRegistry::SetEventSink(IScriptRegistryEventSink* sink)
{
	m_sink = sink;
}

void ScriptRegistry::clear()
{
	for(group_list_t::iterator i = m_groups.begin(); i != m_groups.end(); ++i)
	{
		delete (*i);
	}
	m_groups.clear();
}

//////////////////////////////////////////////////////////////////////////
// ScriptGroup
//////////////////////////////////////////////////////////////////////////

ScriptGroup::ScriptGroup(LPCTSTR name)
{
	m_name = name;
}

ScriptGroup::~ScriptGroup()
{
	clear();
}

Script* ScriptGroup::Add(const char* name, const char* scriptref)
{
	Script* pScript = new Script(name, scriptref);
	Add(pScript);
	return pScript;
}

void ScriptGroup::Add(Script* script)
{
	m_scripts.push_back(script);
}

void ScriptGroup::Remove(Script* script)
{
	m_scripts.remove(script);
}

void ScriptGroup::Clear()
{
	clear();
}

const script_list_t& ScriptGroup::GetScripts()
{
	return m_scripts;
}

LPCTSTR ScriptGroup::GetName() const
{
	return m_name.c_str();
}

void ScriptGroup::clear()
{
	for(script_list_t::iterator i = m_scripts.begin(); i != m_scripts.end(); ++i)
	{
		delete (*i);
	}
	m_scripts.clear();
}

//////////////////////////////////////////////////////////////////////////
// Script
//////////////////////////////////////////////////////////////////////////

void Script::Run()
{
	tstring str(ScriptRef);
	size_t rindex = str.find(':');
	if(rindex == -1)
		return;

	tstring runner_id = str.substr(0, rindex);
	extensions::IScriptRunner* runner = ScriptRegistry::GetInstanceRef().GetRunner(runner_id.c_str());
	if(!runner)
	{
		UNEXPECTED("No ScriptRunner for this script type!");
		return;
	}

	tstring script = str.substr(rindex+1);
	runner->RunScript(script.c_str());
}

//////////////////////////////////////////////////////////////////////////
// DocScript
//////////////////////////////////////////////////////////////////////////

void DocScript::Run()
{
	extensions::IScriptRunner* runner = ScriptRegistry::GetInstanceRef().GetRunner(m_runner.c_str());
	if(!runner)
	{
		UNEXPECTED("No ScriptRunner for this script type!");
		return;
	}

	runner->RunDocScript(m_doc);
}