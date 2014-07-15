FIND_PACKAGE( ROOT REQUIRED)





AUX_SOURCE_DIRECTORY( src library_sources )
SET(serialDict_name SerialComDict.cxx)


SOURCE_GROUP(Headers  C:/projects/git/serialCommunicationClass/serial_communication/inc/*.h)


INCLUDE_DIRECTORIES(  ${ROOT_INCLUDE_DIR} )

# fill list of header files 
set(SerialCom_HEADERS 
  Chiller_Medingen_C20.h
  CommonDeviceController.h
  Keithley2410.h
  SerialCom.h
  TempInterlock.h
  tti_CPX400.h
  )

SET(MY_INCLUDE_DIRECTORIES ./serial_communication/inc ./inc ../../serial_communication/inc ${ROOT_INCLUDE_DIR} )

# set everything needed for the root dictonary and create the
# dictionary
set(SerialCom_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/${serialDict_name}) 

ROOT_GENERATE_DICTIONARY_OLD("${SerialCom_HEADERS}" "${SerialCom_DICTIONARY}" "${MY_INCLUDE_DIRECTORIES}")
ADD_LIBRARY( ${PROJECT_NAME} SHARED ${library_sources}  ${serialDict_name} )

set(test_header
${PROJECT_SOURCE_DIR}/serial_communication/inc/Chiller_Medingen_C20.h)

SOURCE_GROUP(Headers  inc/*.h)

target_link_libraries( ${PROJECT_NAME} ${EUDAQ_THREADS_LIB} ${ROOT_LIBRARIES} ${ADDITIONAL_LIBRARIES})


INSTALL(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
