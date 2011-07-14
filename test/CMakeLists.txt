# tests
add_executable ( coroutine coroutine.cc )
#target_link_libraries ( coroutine pthread )
target_link_libraries ( coroutine libmixed )

add_executable ( fiber fiber.cc )
#target_link_libraries ( coroutine pthread )
target_link_libraries ( fiber libmixed )

add_executable ( private_list private_list.cc )
target_link_libraries ( private_list libmixed )

add_executable ( message_queue message_queue.cc )
target_link_libraries ( message_queue libmixed )

add_executable ( mt-pipe mt-pipe.cc )
target_link_libraries ( mt-pipe pthread )

add_executable ( mt-message_queue mt-message_queue.cc )
target_link_libraries ( mt-message_queue libmixed )
target_link_libraries ( mt-message_queue pthread )

add_executable ( pthread-message_queue pthread-message_queue.cc )
target_link_libraries ( pthread-message_queue libmixed )
target_link_libraries ( pthread-message_queue pthread )

add_executable ( pthread-epoller-server pthread-epoller-server.cc )
target_link_libraries ( pthread-epoller-server libmixed )
target_link_libraries ( pthread-epoller-server pthread )

add_executable ( pthread-epoller-client pthread-epoller-client.cc )
target_link_libraries ( pthread-epoller-client libmixed )
target_link_libraries ( pthread-epoller-client pthread )

add_executable ( scheduler scheduler.cc )
target_link_libraries ( scheduler libmixed )
#target_link_libraries ( scheduler pthread )

add_executable ( fiber-messaging fiber-messaging.cc )
target_link_libraries ( fiber-messaging libmixed )
target_link_libraries ( fiber-messaging pthread )

add_executable ( fiber-server fiber-server.cc )
target_link_libraries ( fiber-server libmixed )
target_link_libraries ( fiber-server pthread )

add_executable ( fiber-client fiber-client.cc )
target_link_libraries ( fiber-client libmixed )
target_link_libraries ( fiber-client pthread )

