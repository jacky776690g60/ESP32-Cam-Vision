idf_component_register(
    SRCS 
        "main.c" 
        "camera/camera.c" 
        "httpserver/httpserver.c" 
        "httpserver/fileshandler.c" 
        "wifi/wifi.c" 
        "utility/utility.cpp"
    INCLUDE_DIRS "."
    EMBED_TXTFILES 
        "partitions.csv"
)

# In your main CMakeLists.txt, add this after including the main component
spiffs_create_partition_image(spiffs spiffs FLASH_IN_PROJECT)
