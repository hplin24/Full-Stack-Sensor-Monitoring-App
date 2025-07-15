declare module 'DHT11Reader' {
  interface DHT11Data {
    temp: number;
    rh: number;
  }

  interface DHT11Reader {

    InitComPort(): Number;

    /**
     * Checks if the COM port is initialized correctly.
     * @returns true if initialize successfully, false otherwise
     */
    IsComPortOk(): boolean;

    /**
     * Retrieves data from the STM32/DHT11 sensor and mutates the given object.
     * @param data An object with `temp` and `rh` fields, which will be updated in-place.
     */
    RetrieveData(data: DHT11Data): void;

    ReInitComPort(): Number;
  }

  const reader: DHT11Reader;
  export = reader;
}