find_package(Qt6Test ${QT_MIN_VERSION} CONFIG REQUIRED)

add_executable(taskvalidatortest taskvalidatortest.cpp
    ../src/taskvalidator.cpp
    ../src/task.cpp
    ../src/crontablib/cttask.cpp
    ../src/crontablib/ctunit.cpp
    ../src/crontablib/ctmonth.cpp
    ../src/crontablib/ctdom.cpp
    ../src/crontablib/ctdow.cpp
    ../src/crontablib/cthour.cpp
    ../src/crontablib/ctminute.cpp
    ../src/crontablib/ctHelper.cpp
)

target_include_directories(taskvalidatortest PRIVATE ../src ../src/crontablib)
target_link_libraries(taskvalidatortest Qt6::Test Qt6::Gui Qt6::Qml KF6::I18n)
add_test(NAME taskvalidatortest COMMAND taskvalidatortest)
