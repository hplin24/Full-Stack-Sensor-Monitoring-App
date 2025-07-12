{
  "targets": [
    {
      "target_name": "DHT11Reader",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [ "src/addon/dht11.cpp", "src/addon/DHT11Reader.cpp", "src/addon/Logger.cpp" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}