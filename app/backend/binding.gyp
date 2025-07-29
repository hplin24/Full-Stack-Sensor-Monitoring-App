{
  "targets": [
    {
      "target_name": "stm32_dht11_reader",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "src/addon/bindings.cpp",
        "src/addon/STM32DHT11Reader.cpp",
        "src/addon/Logger.cpp",
        "src/addon/WMI.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}