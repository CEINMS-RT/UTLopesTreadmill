add_library( PluginUTLOPESTreadmill SHARED src/UTLOPESTreadmillTwinCAT.cpp )


TARGET_LINK_LIBRARIES( PluginUTLOPESTreadmill
	PluginInterface
	AdsClient
	MapTools
)

target_include_directories(PluginUTLOPESTreadmill PUBLIC src)