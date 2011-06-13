# tests
add_executable ( coroutine coroutine.cc )
#target_link_libraries ( coroutine pthread )
target_link_libraries ( coroutine libmixed )

add_executable ( fiber fiber.cc )
#target_link_libraries ( coroutine pthread )
target_link_libraries ( fiber libmixed )

add_executable ( message_queue message_queue.cc )
target_link_libraries ( message_queue libmixed )

