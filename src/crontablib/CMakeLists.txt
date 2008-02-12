add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

set(kcron_crontablib_SRCS
   cthost.cpp 
   ctcron.cpp 
   ctmonth.cpp 
   ctdom.cpp 
   ctdow.cpp 
   cttask.cpp 
   ctvariable.cpp 
)

kde4_add_library(kcron_crontablib STATIC ${kcron_crontablib_SRCS})

target_link_libraries(kcron_crontablib
	${KDE4_KDEUI_LIBS}
)
