include_directories(${CMAKE_CURRENT_SOURCE_DIR})

### reader-proxy

add_executable(atc-reader-proxy reader-proxy.cc)

target_link_libraries(atc-reader-proxy
  PUBLIC
  PRIVATE
  FairMQ::FairMQ
  AliceO2::Framework
  ROOT::Core
  ms_gsl::ms_gsl
  arrow::arrow_shared
  arrow::gandiva_shared
  )

install(TARGETS atc-reader-proxy RUNTIME DESTINATION bin)

### writer-proxy

add_executable(atc-writer-proxy writer-proxy.cc)

target_link_libraries(atc-writer-proxy
  PUBLIC
  PRIVATE
  FairMQ::FairMQ
  AliceO2::Framework
  ROOT::Core
  ms_gsl::ms_gsl
  arrow::arrow_shared
  arrow::gandiva_shared
  )

install(TARGETS atc-writer-proxy RUNTIME DESTINATION bin)

### scl-proxy

if(TOFBUF_FOUND)

  add_executable(atc-scl-proxy scl-proxy.cc ${TOFBUF_SOURCES})
  
  target_include_directories(atc-scl-proxy PRIVATE ${TOFBUF_INCLUDE_DIRS})

  target_link_libraries(atc-scl-proxy
    PUBLIC
    PRIVATE
    FairMQ::FairMQ
    AliceO2::Framework
    ROOT::Core
    ms_gsl::ms_gsl
    arrow::arrow_shared
    arrow::gandiva_shared
    )
  
  install(TARGETS atc-scl-proxy RUNTIME DESTINATION bin)

endif()
