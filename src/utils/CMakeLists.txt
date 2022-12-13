if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_sources(Swiftray PRIVATE
  updater_sparkle.mm
)
endif ()
