#include "YiEventSystem.h"
#include "../../Common/GameTimer.h"
YiEventSystem* YiEventSystem::p_self = NULL;
EVENT_DEFINE g_GlobalEvent[] = {
	{NONE,"NONE"},
	{SERVER_SHUT_DOWN,"SHUT_DOWN"},
	{GAME_WORLD_SYN,"WORLD_SYN"},
	{PLAYER_ENTER_WORLD,"ENTER_WORLD"},
	{PLAYER_EXIT_WORLD,"EXIT_WORLD"},
	{PLAYER_OPEN_FIRE,"OPEN_FIRE"},
	{PLAYER_THROW_GRENADE, "THROW_GRENADE"},
	{OBJECT_DELETE,"OBJECT_DELETE"},
	{PLAYER_HIT,"HIT"},
	{PLAYER_RESPWAN,"RESPWAN"},
	{GAME_OVER,"GAME_OVER"},
	{PLAYER_COLOR_ZONE,"COLOR_ZONE"},
	{GRENADE_DAMAGE_RANGE,"DAMAGE_RANGE"},
	{Bullet_HIT_FLOOR,"BHF"}
};
YiEventSystem::YiEventSystem()
{
	p_self = this;
	Initial();
}

YiEventSystem::~YiEventSystem()
{
}

void YiEventSystem::Initial()
{
	int nEventNum = sizeof(g_GlobalEvent) / sizeof(EVENT_DEFINE);

	for (int i = 0; i < nEventNum; i++)
	{
		m_mapEventIndex_AsName[g_GlobalEvent[i].szEvent] = &(g_GlobalEvent[i]);

		m_mapEventIndex_AsID[g_GlobalEvent[i].idEvent] = &(g_GlobalEvent[i]);
	}
}

void YiEventSystem::_PushEvent(const EVENT& event)
{
	if (!event.pEventDef) return;

	if (event.pEventDef->delayProcess)
	{
		m_delayQueueEvent.push_back(event);
	}
	else
	{
		m_queueEvent.push_back(event);
	}
}

void YiEventSystem::_ProcessEvent(const EVENT& event)
{
	EVENT_DEFINE* pEventDef = event.pEventDef;
	if (!pEventDef) return;


	if (!(pEventDef->listFuncNotify.empty()))
	{
		EVENT_DEFINE::REGISTER_STRUCT::iterator it;
		for (it = pEventDef->listFuncNotify.begin(); it != pEventDef->listFuncNotify.end(); it++)
		{
			if ((*it).first)((*it).first)(&event, (*it).second);
		}
	}
}

void YiEventSystem::PushEvent(GAME_EVENT_ID id, const std::vector<std::string>& vParam)
{
	if (m_mapEventIndex_AsID.find(id) == m_mapEventIndex_AsID.end()) return;

	EVENT event;
	event.pEventDef = m_mapEventIndex_AsID[id];

	event.vArg = vParam;

	_PushEvent(event);
}

void YiEventSystem::PushEvent(GAME_EVENT_ID id)
{
	if (m_mapEventIndex_AsID.find(id) == m_mapEventIndex_AsID.end()) return;

	EVENT event;
	event.pEventDef = m_mapEventIndex_AsID[id];

	_PushEvent(event);
}

void YiEventSystem::PushEvent(GAME_EVENT_ID id, int iArg0)
{
	if (m_mapEventIndex_AsID.find(id) == m_mapEventIndex_AsID.end()) return;

	EVENT event;
	event.pEventDef = m_mapEventIndex_AsID[id];

	char szTemp[32];
	_snprintf_s(szTemp, 32, "%d", iArg0);
	event.vArg.push_back(szTemp);

	_PushEvent(event);
}

void YiEventSystem::PushEvent(GAME_EVENT_ID id, float fArg0)
{
	if (m_mapEventIndex_AsID.find(id) == m_mapEventIndex_AsID.end()) return;

	EVENT event;
	event.pEventDef = m_mapEventIndex_AsID[id];

	char szTemp[32];
	_snprintf_s(szTemp, 32, "%f", fArg0);
	event.vArg.push_back(szTemp);

	_PushEvent(event);
}

void YiEventSystem::PushEvent(GAME_EVENT_ID id, const char* szArg0)
{
	if (m_mapEventIndex_AsID.find(id) == m_mapEventIndex_AsID.end()) return;

	EVENT event;
	event.pEventDef = m_mapEventIndex_AsID[id];

	event.vArg.push_back(szArg0);

	_PushEvent(event);
}

void YiEventSystem::PushEvent(GAME_EVENT_ID id, const char* szArg0, const char* szArg1)
{
	if (m_mapEventIndex_AsID.find(id) == m_mapEventIndex_AsID.end()) return;

	EVENT event;
	event.pEventDef = m_mapEventIndex_AsID[id];

	event.vArg.push_back(szArg0);
	event.vArg.push_back(szArg1);

	_PushEvent(event);
}

void YiEventSystem::PushEvent(GAME_EVENT_ID id, const char* szArg0, const char* szArg1, int nArg2)
{
	if (m_mapEventIndex_AsID.find(id) == m_mapEventIndex_AsID.end()) return;

	EVENT event;
	event.pEventDef = m_mapEventIndex_AsID[id];

	event.vArg.push_back(szArg0);
	event.vArg.push_back(szArg1);
	char szTemp[32];
	_snprintf_s(szTemp, 32, "%d", nArg2);
	event.vArg.push_back(szTemp);

	_PushEvent(event);
}

void YiEventSystem::PushEvent(GAME_EVENT_ID id, int iArg0, int iArg1)
{
	if (m_mapEventIndex_AsID.find(id) == m_mapEventIndex_AsID.end()) return;

	EVENT event;
	event.pEventDef = m_mapEventIndex_AsID[id];

	char szTemp[32];

	_snprintf_s(szTemp, 32, "%d", iArg0);
	event.vArg.push_back(szTemp);
	_snprintf_s(szTemp, 32, "%d", iArg1);
	event.vArg.push_back(szTemp);

	_PushEvent(event);
}

void YiEventSystem::PushEvent(GAME_EVENT_ID id, const char* szArg0, const char* szArg1, int iArg2, int iArg3)
{
	if (m_mapEventIndex_AsID.find(id) == m_mapEventIndex_AsID.end()) return;

	EVENT event;
	event.pEventDef = m_mapEventIndex_AsID[id];

	event.vArg.push_back(szArg0);
	event.vArg.push_back(szArg1);

	char szTemp[32];
	_snprintf_s(szTemp, 32, "%d", iArg2);
	event.vArg.push_back(szTemp);
	_snprintf_s(szTemp, 32, "%d", iArg3);
	event.vArg.push_back(szTemp);

	_PushEvent(event);
}

void YiEventSystem::RegisterEventHandle(const std::string& nameEvent, FUNC_EVENT_HANDLE funHandle, unsigned long long uOwnerData)
{
	if (!funHandle) return;

	EVENT_DEFINE* pEvent = m_mapEventIndex_AsName[nameEvent];
	if (!pEvent) return;

	pEvent->listFuncNotify.push_back(std::make_pair(funHandle, uOwnerData));
}

void YiEventSystem::ProcessAllEvent()
{
	/*if (!(m_delayQueueEvent.empty()))
	{
		const UINT WORK_STEP = 2;
		NCL::GameTimer t;
		t.GetTimeDeltaSeconds();
		t.Tick();
		float updateTime = t.GetTimeDeltaSeconds();
		if (updateTime >= WORK_STEP)
		{
		
			const EVENT& event = *(m_delayQueueEvent.begin());

			_ProcessEvent(event);

			m_delayQueueEvent.erase(m_delayQueueEvent.begin());
		}
	}*/

	register std::list< EVENT >::iterator it;
	for (it = m_queueEvent.begin(); it != m_queueEvent.end(); it++)
	{
		const EVENT& event = *it;

		
		bool bMultiPushed = false;
		for (register std::list< EVENT >::iterator itPrev = m_queueEvent.begin(); itPrev != it; itPrev++)
		{
			if (*itPrev == *it)
			{
				bMultiPushed = true;
				break;
			}
		}

		if (bMultiPushed) continue;
		_ProcessEvent(event);
	}
	m_queueEvent.clear();
}
