SET(BOOST_LIBS "")
LIST(APPEND BOOST_LIBS "${CMAKE_SOURCE_DIR}/depends/${DEPENDS_PREFIX}/lib/libboost_filesystem-mt-x64.a")
LIST(APPEND BOOST_LIBS "${CMAKE_SOURCE_DIR}/depends/${DEPENDS_PREFIX}/lib/libboost_program_options-mt-x64.a")
LIST(APPEND BOOST_LIBS "${CMAKE_SOURCE_DIR}/depends/${DEPENDS_PREFIX}/lib/libboost_system-mt-x64.a")
LIST(APPEND BOOST_LIBS "${CMAKE_SOURCE_DIR}/depends/${DEPENDS_PREFIX}/lib/libboost_thread-mt-x64.a")
LIST(APPEND BOOST_LIBS "${CMAKE_SOURCE_DIR}/depends/${DEPENDS_PREFIX}/lib/libboost_chrono-mt-x64.a")
LIST(APPEND BOOST_LIBS "${CMAKE_SOURCE_DIR}/depends/${DEPENDS_PREFIX}/lib/libboost_nowide-mt-x64.a")

SET(OPENSSL_LIBS "")
LIST(APPEND OPENSSL_LIBS "${CMAKE_SOURCE_DIR}/depends/${DEPENDS_PREFIX}/lib/libcrypto.a")
LIST(APPEND OPENSSL_LIBS "${CMAKE_SOURCE_DIR}/depends/${DEPENDS_PREFIX}/lib/libssl.a")
LIST(APPEND OPENSSL_LIBS "-ldl")


SET(Z_LIBS "")
LIST(APPEND Z_LIBS "${CMAKE_SOURCE_DIR}/depends/${DEPENDS_PREFIX}/lib/libz.a")

SET(LIB_INCLUDE "")
LIST(APPEND LIB_INCLUDE "${CMAKE_SOURCE_DIR}/depends/${DEPENDS_PREFIX}/include")