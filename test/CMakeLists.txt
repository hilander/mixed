# tests
add_executable ( coroutine coroutine.cc )
target_link_libraries ( coroutine 
  pthread
  libmixed
  gtest
)
add_test( coroutine ${CMAKE_BINARY_DIR}/bin/coroutine )

add_executable ( private_list private_list.cc )
target_link_libraries ( private_list 
  libmixed 
  gtest
)
add_test( private_list ${CMAKE_BINARY_DIR}/bin/private_list )

add_executable ( fiber_yield yield.cc )
target_link_libraries ( fiber_yield 
  libmixed 
  gtest
)

add_executable ( fiber_nullfork fiber_nullfork.cc )
target_link_libraries ( fiber_nullfork 
  libmixed 
)

add_executable ( pthread_nullfork pthread_nullfork.cc )
target_link_libraries ( pthread_nullfork 
  libmixed 
)

add_executable ( pthread_yield pthread_yield.cc )
target_link_libraries ( pthread_yield 
  libmixed 
  gtest
)

add_executable ( fiber_write fiber-write.cc )
target_link_libraries ( fiber_write 
  libmixed 
  gtest
)

add_executable ( blocking_write blocking-write.cc )
target_link_libraries ( blocking_write 
  libmixed 
  gtest
)

add_executable ( scheduler scheduler.cc )
target_link_libraries ( scheduler 
  libmixed 
  gtest
)
add_test( scheduler ${CMAKE_BINARY_DIR}/bin/scheduler )

add_executable ( fiber fiber.cc )
target_link_libraries ( fiber 
  libmixed 
  gtest
)
add_test( fiber ${CMAKE_BINARY_DIR}/bin/fiber )

add_executable ( message_queue message_queue.cc )
target_link_libraries ( message_queue 
  libmixed 
  gtest
)
add_test( message_queue ${CMAKE_BINARY_DIR}/bin/message_queue )

add_executable ( mt-pipe mt-pipe.cc )
target_link_libraries ( mt-pipe 
  libmixed 
  gtest
)
add_test( mt-pipe ${CMAKE_BINARY_DIR}/bin/mt-pipe )

add_executable ( mt-message_queue mt-message_queue.cc )
target_link_libraries ( mt-message_queue 
  libmixed 
  gtest
)
add_test( mt-message_queue ${CMAKE_BINARY_DIR}/bin/mt-message_queue )

add_executable ( fiber-messaging fiber-messaging.cc )
target_link_libraries ( fiber-messaging 
  libmixed 
  gtest
)
add_test( fiber-messaging ${CMAKE_BINARY_DIR}/bin/fiber-messaging )

add_executable ( y1 y1.cc )
target_link_libraries ( y1 
  libmixed 
  gtest
)
add_test( y1 ${CMAKE_BINARY_DIR}/bin/y1 )

add_executable ( pthread-message_queue pthread-message_queue.cc )
target_link_libraries ( pthread-message_queue libmixed )
target_link_libraries ( pthread-message_queue pthread )

add_executable ( pthread-epoller-server pthread-epoller-server.cc )
target_link_libraries ( pthread-epoller-server libmixed )
target_link_libraries ( pthread-epoller-server pthread )

add_executable ( pthread-epoller-client pthread-epoller-client.cc )
target_link_libraries ( pthread-epoller-client libmixed )
target_link_libraries ( pthread-epoller-client pthread )

add_executable ( fiber-server fiber-server.cc )
target_link_libraries ( fiber-server libmixed )
target_link_libraries ( fiber-server pthread )

add_executable ( fiber-client fiber-client.cc )
target_link_libraries ( fiber-client libmixed )
target_link_libraries ( fiber-client pthread )

