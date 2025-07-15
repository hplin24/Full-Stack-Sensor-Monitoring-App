{
  "targets": [
    {
      "target_name": "stm32_dht11",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [ "src/addon/stm32_dht11.cpp", "src/addon/DHT11Reader.cpp", "src/addon/Logger.cpp" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}