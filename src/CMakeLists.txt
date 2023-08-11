project(dohdocs)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( dohdocs dohdocs dohdocs.cpp )
target_include_directories( dohdocs PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( dohdocs ${CMAKE_SOURCE_DIR}/../ricardian )