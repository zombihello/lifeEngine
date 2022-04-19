# 	---------------------------------
#	[in] 	WXWIDGETS_PATH					- root dir WxWidgets
#	[out] 	WXWIDGETS_INCLUDE				- dir with includes
#	[out]	WXWIDGETS_LIB					- lib WxWidgets
#	[out]	WXWIDGETS_NET_LIB       		- lib WxWidgets net
#	[out]	WXWIDGETS_XML_LIB       		- lib WxWidgets xml
#	[out]	WXWIDGETS_ADV_LIB       		- lib WxWidgets adv
#	[out]	WXWIDGETS_AUI_LIB       		- lib WxWidgets aui
#	[out]	WXWIDGETS_CORE_LIB      		- lib WxWidgets core
#	[out]	WXWIDGETS_GL_LIB        		- lib WxWidgets GL
#	[out]	WXWIDGETS_HTML_LIB      		- lib WxWidgets html
#	[out]	WXWIDGETS_MEDIA_LIB     		- lib WxWidgets media
#	[out]	WXWIDGETS_PROPGRID_LIB  		- lib WxWidgets propgrid
#	[out]	WXWIDGETS_QA_LIB        		- lib WxWidgets qa
#	[out]	WXWIDGETS_RIBBON_LIB    		- lib WxWidgets ribbon
#	[out]	WXWIDGETS_RICHTEXT_LIB  		- lib WxWidgets richtext
#	[out]	WXWIDGETS_STC_LIB       		- lib WxWidgets stc
#	[out]	WXWIDGETS_WEBVIEW_LIB   		- lib WxWidgets webview
#	[out]	WXWIDGETS_XRC_LIB       		- lib WxWidgets xrc
#	[out]	WXWIDGETS_DEBUG_LIB				- debug lib WxWidgets
#	[out]	WXWIDGETS_NET_DEBUG_LIB       	- debug lib WxWidgets net
#	[out]	WXWIDGETS_XML_DEBUG_LIB       	- debug lib WxWidgets xml
#	[out]	WXWIDGETS_ADV_DEBUG_LIB       	- debug lib WxWidgets adv
#	[out]	WXWIDGETS_AUI_DEBUG_LIB       	- debug lib WxWidgets aui
#	[out]	WXWIDGETS_CORE_DEBUG_LIB      	- debug lib WxWidgets core
#	[out]	WXWIDGETS_GL_DEBUG_LIB        	- debug lib WxWidgets GL
#	[out]	WXWIDGETS_HTML_DEBUG_LIB      	- debug lib WxWidgets html
#	[out]	WXWIDGETS_MEDIA_DEBUG_LIB     	- debug lib WxWidgets media
#	[out]	WXWIDGETS_PROPGRID_DEBUG_LIB  	- debug lib WxWidgets propgrid
#	[out]	WXWIDGETS_QA_DEBUG_LIB        	- debug lib WxWidgets qa
#	[out]	WXWIDGETS_RIBBON_DEBUG_LIB    	- debug lib WxWidgets ribbon
#	[out]	WXWIDGETS_RICHTEXT_DEBUG_LIB  	- debug lib WxWidgets richtext
#	[out]	WXWIDGETS_STC_DEBUG_LIB       	- debug lib WxWidgets stc
#	[out]	WXWIDGETS_WEBVIEW_DEBUG_LIB   	- debug lib WxWidgets webview
#	[out]	WXWIDGETS_XRC_DEBUG_LIB       	- debug lib WxWidgets xrc
#	[out]	WXWIDGETS_FOUND					- is found WxWidgets
# 	---------------------------------

set( WXWIDGETS_FOUND OFF )
SET( WXWIDGETS_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${WXWIDGETS_PATH} )

#
# Set path suffixed to libs for current platform
#
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( WXWIDGETS_LIB_PATH_SUFFIXES "lib/vc_x64_dll" )
	elseif( PLATFORM_32BIT )
		set( WXWIDGETS_LIB_PATH_SUFFIXES "lib/vc_dll" )
	endif()
	
	add_definitions( -DWXUSINGDLL )
	add_definitions( -D__WXMSW__ )
else()
	message( SEND_ERROR "WxWidgets :: Unknown platform" )
	return()
endif()

#
# Find pathes
#
find_path( 		WXWIDGETS_INCLUDE
				NAMES "wx/wx.h"
				PATH_SUFFIXES include
				PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_path( 		WXWIDGETS_MSVC_INCLUDE
				NAMES "wx/setup.h"
				PATH_SUFFIXES include/msvc
				PATHS ${WXWIDGETS_SEARCH_PATHS} )		

# Release				
find_library( 	WXWIDGETS_LIB 
                NAMES wxbase31u
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_NET_LIB 
                NAMES wxbase31u_net
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_XML_LIB 
                NAMES wxbase31u_xml
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_ADV_LIB 
                NAMES wxmsw31u_adv
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_AUI_LIB 
                NAMES wxmsw31u_aui
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_CORE_LIB 
                NAMES wxmsw31u_core
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_GL_LIB 
                NAMES wxmsw31u_gl
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_HTML_LIB 
                NAMES wxmsw31u_html
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_MEDIA_LIB 
                NAMES wxmsw31u_media
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_PROPGRID_LIB 
                NAMES wxmsw31u_propgrid
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_QA_LIB 
                NAMES wxmsw31u_qa
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_RIBBON_LIB 
                NAMES wxmsw31u_ribbon
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_RICHTEXT_LIB 
                NAMES wxmsw31u_richtext
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_STC_LIB 
                NAMES wxmsw31u_stc
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_WEBVIEW_LIB 
                NAMES wxmsw31u_webview
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_XRC_LIB 
                NAMES wxmsw31u_xrc
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
				
# Debug
find_library( 	WXWIDGETS_DEBUG_LIB 
                NAMES wxbase31ud
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_NET_DEBUG_LIB 
                NAMES wxbase31ud_net
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_XML_DEBUG_LIB 
                NAMES wxbase31ud_xml
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_ADV_DEBUG_LIB 
                NAMES wxmsw31ud_adv
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_AUI_DEBUG_LIB 
                NAMES wxmsw31ud_aui
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_CORE_DEBUG_LIB 
                NAMES wxmsw31ud_core
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_GL_DEBUG_LIB 
                NAMES wxmsw31ud_gl
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_HTML_DEBUG_LIB 
                NAMES wxmsw31ud_html
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_MEDIA_DEBUG_LIB 
                NAMES wxmsw31ud_media
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_PROPGRID_DEBUG_LIB 
                NAMES wxmsw31ud_propgrid
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_QA_DEBUG_LIB 
                NAMES wxmsw31ud_qa
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_RIBBON_DEBUG_LIB 
                NAMES wxmsw31ud_ribbon
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_RICHTEXT_DEBUG_LIB 
                NAMES wxmsw31ud_richtext
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_STC_DEBUG_LIB 
                NAMES wxmsw31ud_stc
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_WEBVIEW_DEBUG_LIB 
                NAMES wxmsw31ud_webview
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )
find_library( 	WXWIDGETS_XRC_DEBUG_LIB 
                NAMES wxmsw31ud_xrc
                PATH_SUFFIXES ${WXWIDGETS_LIB_PATH_SUFFIXES}
                PATHS ${WXWIDGETS_SEARCH_PATHS} )			
				    		
if ( 	NOT WXWIDGETS_INCLUDE OR NOT WXWIDGETS_MSVC_INCLUDE OR
		NOT WXWIDGETS_LIB OR NOT WXWIDGETS_NET_LIB OR NOT WXWIDGETS_XML_LIB OR NOT WXWIDGETS_ADV_LIB OR NOT WXWIDGETS_AUI_LIB OR NOT WXWIDGETS_CORE_LIB OR
		NOT WXWIDGETS_GL_LIB OR NOT WXWIDGETS_HTML_LIB OR NOT WXWIDGETS_MEDIA_LIB OR NOT WXWIDGETS_PROPGRID_LIB OR NOT WXWIDGETS_QA_LIB OR 
		NOT WXWIDGETS_RIBBON_LIB OR NOT WXWIDGETS_RICHTEXT_LIB OR NOT WXWIDGETS_STC_LIB OR NOT WXWIDGETS_WEBVIEW_LIB OR NOT WXWIDGETS_XRC_LIB OR 
		NOT WXWIDGETS_DEBUG_LIB OR NOT WXWIDGETS_NET_DEBUG_LIB OR NOT WXWIDGETS_XML_DEBUG_LIB OR NOT WXWIDGETS_ADV_DEBUG_LIB OR NOT WXWIDGETS_AUI_DEBUG_LIB OR 
		NOT WXWIDGETS_CORE_DEBUG_LIB OR NOT WXWIDGETS_GL_DEBUG_LIB OR NOT WXWIDGETS_HTML_DEBUG_LIB OR NOT WXWIDGETS_MEDIA_DEBUG_LIB OR NOT WXWIDGETS_PROPGRID_DEBUG_LIB OR
		NOT WXWIDGETS_QA_DEBUG_LIB OR NOT WXWIDGETS_RIBBON_DEBUG_LIB OR NOT WXWIDGETS_RICHTEXT_DEBUG_LIB OR NOT WXWIDGETS_STC_DEBUG_LIB OR NOT WXWIDGETS_WEBVIEW_DEBUG_LIB OR NOT WXWIDGETS_XRC_DEBUG_LIB )
    return()
else()
	set( WXWIDGETS_INCLUDE ${WXWIDGETS_INCLUDE} ${WXWIDGETS_MSVC_INCLUDE} )
	set( WXWIDGETS_FOUND ON )
endif()
