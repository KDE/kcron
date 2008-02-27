
set(kcron_crontablib_SRCS
   cthost.cpp 
   ctcron.cpp 
   ctmonth.cpp 
   ctminute.cpp 
   cthour.cpp 
   ctdom.cpp 
   ctdow.cpp 
   cttask.cpp 
   ctunit.cpp 
   ctvariable.cpp
   
   ctGlobalCron.cpp  
)

kde4_add_library(kcron_crontablib STATIC ${kcron_crontablib_SRCS})

target_link_libraries(kcron_crontablib
	${KDE4_KDEUI_LIBS}
)
