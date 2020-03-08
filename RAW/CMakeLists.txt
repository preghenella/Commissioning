add_executable(atc-file-proxy src/file-proxy.cc)
target_link_libraries(atc-file-proxy
  PUBLIC
  PRIVATE
  FairMQ::FairMQ
  AliceO2::Framework
  ROOT::Core
  ms_gsl::ms_gsl
  arrow::arrow_shared
  arrow::gandiva_shared
  )
install(TARGETS atc-file-proxy RUNTIME DESTINATION bin)

