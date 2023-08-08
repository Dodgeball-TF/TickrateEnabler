#include "eiface.h"
#include "sourcehook/sourcehook_impl.h"
#include <ISmmPlugin.h>
#include "tier0/icommandline.h"

PLUGIN_GLOBALVARS();
SH_DECL_HOOK0(IServerGameDLL, GetTickInterval, const, 0, float);

class TickRatePlugin : public ISmmPlugin
{
public:
	bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late);
	bool Unload(char *error, size_t maxlen);

public:
	const char *GetAuthor();
	const char *GetName();
	const char *GetDescription();
	const char *GetURL();
	const char *GetLicense();
	const char *GetVersion();
	const char *GetDate();
	const char *GetLogTag();
};

IServerGameDLL *server = nullptr;
TickRatePlugin g_TickrateEnabler;

PLUGIN_EXPOSE(TickRatePlugin, g_TickRatePlugin);
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(TickRatePlugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_TickrateEnabler);

float GetTickInterval()
{
	float tickinterval = 0.01f;
	RETURN_META_VALUE(MRES_SUPERCEDE, tickinterval);
}

const char *TickrateEnabler::GetPluginDescription(void)
{
	return "TickrateEnabler for TF2";
}

bool TickRatePlugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	PLUGIN_SAVEVARS();
	// don't need to check METAMOD_PLAPI_VERSION here
	GET_V_IFACE_ANY(GetServerFactory, server, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);

	server = (IServerGameDLL *)gameServerFactory("ServerGameDLL012", NULL);
	if (!server)
	{
		Warning("Failed to get a pointer on ServerGameDLL.\n");
		return false;
	}

	SH_ADD_HOOK(IServerGameDLL, GetTickInterval, server, SH_STATIC(GetTickInterval), false);

	return true;
}

void TickRatePlugin::Unload(void)
{
	SH_REMOVE_HOOK(IServerGameDLL, GetTickInterval, server, SH_STATIC(GetTickInterval), false);
}
