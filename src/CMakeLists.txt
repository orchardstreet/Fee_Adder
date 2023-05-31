add_executable(Payment_Manager filesystem.c list.c payment_manager.c utils.c validate.c)
TARGET_LINK_LIBRARIES( Payment_Manager ${CMAKE_THREAD_LIBS_INIT} ${GTK_LIBRARIES} )



